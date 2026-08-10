"""
============================================================================
 ringspeicher.py — Round-Robin-Speicher
============================================================================
 Die Antwort auf "nach 200 Clips soll er den ältesten überschreiben".

 Es gibt feste Dateinamen clip_000.mp4 bis clip_199.mp4, und ein Zähler
 sagt, welcher als nächstes dran ist. Danach fängt er wieder bei 0 an.
 Wie ein Karussell.

 Drei Eigenschaften, die das mitbringt:
   1. Die Platte kann nie volllaufen — der Platzbedarf steht von Anfang
      an fest.
   2. Es gibt keine "Aufräum-Aktion", die man vergessen kann.
   3. Der Ordner wächst nicht. Es gibt immer genau 200 Namen.
============================================================================
"""

import json
import logging
from pathlib import Path

from . import einstellungen as cfg

log = logging.getLogger(__name__)


class Ringspeicher:
    """Verwaltet einen Ordner mit fester Anzahl durchnummerierter Dateien."""

    def __init__(self, ordner: str, praefix: str, endung: str, anzahl: int):
        self.ordner = Path(cfg.DATEN_PFAD) / ordner
        self.praefix = praefix
        self.endung = endung
        self.anzahl = anzahl
        self.ordner.mkdir(parents=True, exist_ok=True)

        self._zustand_datei = self.ordner / ".ring.json"
        self._zaehler = self._zustand_laden()

    # -- Zustand -----------------------------------------------------------
    def _zustand_laden(self) -> int:
        try:
            with open(self._zustand_datei) as f:
                return int(json.load(f).get("zaehler", 0)) % self.anzahl
        except (FileNotFoundError, ValueError, json.JSONDecodeError):
            return 0

    def _zustand_speichern(self) -> None:
        try:
            with open(self._zustand_datei, "w") as f:
                json.dump({"zaehler": self._zaehler}, f)
        except OSError as e:
            log.warning("Ringzustand nicht speicherbar: %s", e)

    # -- Benutzung ---------------------------------------------------------
    def naechster_pfad(self) -> Path:
        """Nächsten freien Platz im Ring holen und den Zähler weiterdrehen."""
        name = f"{self.praefix}_{self._zaehler:03d}.{self.endung}"
        pfad = self.ordner / name
        self._zaehler = (self._zaehler + 1) % self.anzahl
        self._zustand_speichern()
        return pfad

    def dateien(self) -> list[dict]:
        """Alle belegten Plätze, neueste zuerst. Für die Website."""
        gefunden = []
        for p in self.ordner.glob(f"{self.praefix}_*.{self.endung}"):
            try:
                st = p.stat()
            except OSError:
                continue
            # Leere oder abgebrochene Dateien überspringen
            if st.st_size < 1024:
                continue
            gefunden.append({
                "name": p.name,
                "pfad": f"{self.ordner.name}/{p.name}",
                "groesse": st.st_size,
                "zeit": int(st.st_mtime),
            })
        gefunden.sort(key=lambda d: d["zeit"], reverse=True)
        return gefunden

    def belegt(self) -> int:
        return len(self.dateien())

    def bytes_gesamt(self) -> int:
        return sum(d["groesse"] for d in self.dateien())


# ---------------------------------------------------------------------------
# Die beiden Ringe, die das Programm benutzt
# ---------------------------------------------------------------------------
clips = Ringspeicher("clips", "clip", "mp4", cfg.RING_CLIPS)
fotos = Ringspeicher("fotos", "foto", "jpg", cfg.RING_FOTOS)


def platz_frei_prozent() -> int:
    """Wie viel Platz ist auf der SSD noch frei? Für die Website."""
    import shutil
    try:
        nutzung = shutil.disk_usage(cfg.DATEN_PFAD)
        return int(nutzung.free * 100 / nutzung.total)
    except OSError:
        return 0
