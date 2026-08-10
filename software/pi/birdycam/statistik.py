"""
============================================================================
 statistik.py — Die Datenbank
============================================================================
 Wir benutzen SQLite. Das ist eine ganze Datenbank in einer einzigen Datei,
 und Python hat sie schon eingebaut — nichts zu installieren.

 Wichtig ist der "WAL-Modus" (Write-Ahead Log): Damit übersteht die
 Datenbank einen Stromausfall mitten im Schreiben, ohne kaputtzugehen.
 Bei einer Solaranlage ist das keine Theorie.

 Drei Tabellen:
   besuche      — jeder Vogel, der durch die Lichtschranke geflogen ist
   clips        — jede Videoaufnahme
   telemetrie   — Akkuspannung und Ladestrom über die Zeit
============================================================================
"""

import logging
import sqlite3
import threading
import time
from pathlib import Path

from . import einstellungen as cfg

log = logging.getLogger(__name__)

_sperre = threading.Lock()          # SQLite mag keine zwei Schreiber gleichzeitig
_verbindung: sqlite3.Connection | None = None


def start() -> None:
    """Datenbank öffnen und Tabellen anlegen, falls sie fehlen."""
    global _verbindung
    pfad = Path(cfg.DATEN_PFAD) / "birdycam.db"
    pfad.parent.mkdir(parents=True, exist_ok=True)

    _verbindung = sqlite3.connect(str(pfad), check_same_thread=False)
    _verbindung.row_factory = sqlite3.Row

    with _sperre:
        _verbindung.execute("PRAGMA journal_mode=WAL")     # stromausfallfest
        _verbindung.execute("PRAGMA synchronous=NORMAL")   # schont die SSD
        _verbindung.executescript("""
            CREATE TABLE IF NOT EXISTS besuche (
                id        INTEGER PRIMARY KEY,
                ts        INTEGER NOT NULL,      -- Unix-Zeit des Einflugs
                dauer_ms  INTEGER,               -- NULL = noch drin / nur Durchflug
                art       TEXT NOT NULL          -- 'besuch' oder 'durchflug'
            );
            CREATE INDEX IF NOT EXISTS idx_besuche_ts ON besuche(ts);

            CREATE TABLE IF NOT EXISTS clips (
                id       INTEGER PRIMARY KEY,
                ts       INTEGER NOT NULL,
                datei    TEXT NOT NULL,
                dauer_s  REAL,
                groesse  INTEGER,
                nacht    INTEGER DEFAULT 0,
                ausloser TEXT                    -- 'lichtschranke' oder 'bild'
            );
            CREATE INDEX IF NOT EXISTS idx_clips_ts ON clips(ts);

            CREATE TABLE IF NOT EXISTS telemetrie (
                ts      INTEGER PRIMARY KEY,
                volt    REAL,
                ampere  REAL,
                nacht   INTEGER
            );
        """)
        _verbindung.commit()
    log.info("Datenbank bereit: %s", pfad)


def _schreiben(sql: str, werte: tuple = ()) -> int | None:
    if _verbindung is None:
        return None
    try:
        with _sperre:
            zeiger = _verbindung.execute(sql, werte)
            _verbindung.commit()
            return zeiger.lastrowid
    except sqlite3.Error as e:
        log.error("Datenbankfehler beim Schreiben: %s", e)
        return None


def _lesen(sql: str, werte: tuple = ()) -> list[sqlite3.Row]:
    if _verbindung is None:
        return []
    try:
        with _sperre:
            return _verbindung.execute(sql, werte).fetchall()
    except sqlite3.Error as e:
        log.error("Datenbankfehler beim Lesen: %s", e)
        return []


# ---------------------------------------------------------------------------
#  Eintragen
# ---------------------------------------------------------------------------
def besuch_eintragen(ts: int, dauer_ms: int | None, art: str = "besuch") -> None:
    _schreiben("INSERT INTO besuche (ts, dauer_ms, art) VALUES (?,?,?)",
               (ts, dauer_ms, art))
    log.info("Besuch eingetragen (%s, Dauer %s)", art,
             f"{dauer_ms/1000:.1f} s" if dauer_ms else "unbekannt")


def besuch_dauer_nachtragen(dauer_ms: int) -> None:
    """Beim Ausflug die Aufenthaltsdauer beim letzten offenen Besuch eintragen.

    "Offen" heißt: Wir haben den Einflug gesehen, aber noch keinen Ausflug —
    dauer_ms ist dort noch NULL.
    """
    _schreiben(
        "UPDATE besuche SET dauer_ms = ? WHERE id = ("
        "  SELECT id FROM besuche WHERE dauer_ms IS NULL AND art = 'besuch' "
        "  ORDER BY ts DESC LIMIT 1)",
        (dauer_ms,))


def clip_eintragen(datei: str, dauer_s: float, groesse: int,
                   nacht: bool, ausloser: str) -> None:
    _schreiben(
        "INSERT INTO clips (ts, datei, dauer_s, groesse, nacht, ausloser) "
        "VALUES (?,?,?,?,?,?)",
        (int(time.time()), datei, dauer_s, groesse, int(nacht), ausloser))


def telemetrie_eintragen(volt: float, ampere: float, nacht: bool) -> None:
    _schreiben(
        "INSERT OR REPLACE INTO telemetrie (ts, volt, ampere, nacht) VALUES (?,?,?,?)",
        (int(time.time()), round(volt, 3), round(ampere, 4), int(nacht)))


def aufraeumen(tage: int = 180) -> None:
    """Alte Telemetrie löschen. Besuche behalten wir für immer — sie sind winzig."""
    grenze = int(time.time()) - tage * 86400
    _schreiben("DELETE FROM telemetrie WHERE ts < ?", (grenze,))


# ---------------------------------------------------------------------------
#  Auswerten — das, was die Website zeigt
# ---------------------------------------------------------------------------
def uebersicht() -> dict:
    """Alle Zahlen für die Startseite in einem Rutsch."""
    heute_start = int(time.mktime(time.localtime()[:3] + (0, 0, 0, 0, 0, -1)))

    gesamt = _lesen("SELECT COUNT(*) AS n FROM besuche")
    heute = _lesen("SELECT COUNT(*) AS n FROM besuche WHERE ts >= ?", (heute_start,))
    ersterletzter = _lesen(
        "SELECT MIN(ts) AS erster, MAX(ts) AS letzter FROM besuche WHERE ts >= ?",
        (heute_start,))
    dauer = _lesen(
        "SELECT AVG(dauer_ms) AS d FROM besuche "
        "WHERE ts >= ? AND dauer_ms IS NOT NULL", (heute_start,))

    # Anflüge je Stunde, heute — das Balkendiagramm
    stunden = [0] * 24
    for zeile in _lesen(
            "SELECT CAST(strftime('%H', ts, 'unixepoch', 'localtime') AS INTEGER) AS h,"
            " COUNT(*) AS n FROM besuche WHERE ts >= ? GROUP BY h", (heute_start,)):
        if zeile["h"] is not None and 0 <= zeile["h"] < 24:
            stunden[zeile["h"]] = zeile["n"]

    # Die letzten 14 Tage — daran sieht man den Schlupftag der Küken
    verlauf = [
        {"tag": z["tag"], "anzahl": z["n"]}
        for z in _lesen(
            "SELECT date(ts,'unixepoch','localtime') AS tag, COUNT(*) AS n "
            "FROM besuche GROUP BY tag ORDER BY tag DESC LIMIT 14")
    ][::-1]

    return {
        "besuche_gesamt": gesamt[0]["n"] if gesamt else 0,
        "besuche_heute": heute[0]["n"] if heute else 0,
        "erster_heute": ersterletzter[0]["erster"] if ersterletzter else None,
        "letzter_heute": ersterletzter[0]["letzter"] if ersterletzter else None,
        "dauer_schnitt_s": round((dauer[0]["d"] or 0) / 1000, 1) if dauer else 0,
        "stunden": stunden,
        "verlauf": verlauf,
    }


def letzte_telemetrie(stunden: int = 24) -> list[dict]:
    grenze = int(time.time()) - stunden * 3600
    return [dict(z) for z in _lesen(
        "SELECT ts, volt, ampere FROM telemetrie WHERE ts >= ? ORDER BY ts",
        (grenze,))]
