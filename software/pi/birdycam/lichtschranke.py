"""
============================================================================
 lichtschranke.py — Vögel zählen
============================================================================
 Ein Infrarot-Strahl liegt quer im Einflugloch. Fliegt ein Vogel durch,
 bricht er ihn für etwa 100 bis 300 Millisekunden.

 Daraus baut dieses Modul echte Zahlen:

     Unterbrechung 1  ->  Einflug     (Uhrzeit merken)
     Unterbrechung 2  ->  Ausflug     (Dauer = Zeit dazwischen)

 Warum das besser ist als Bilderkennung: Ein Sonnenfleck, der durch den
 Kasten wandert, sieht im Bild wie Bewegung aus. Der IR-Strahl im Loch
 interessiert sich dafür nicht. Deshalb sind die Zahlen auf der Website
 Messwerte und keine Schätzungen.
============================================================================
"""

import logging
import time
from typing import Callable

from . import einstellungen as cfg
from . import statistik

log = logging.getLogger(__name__)

_sensor = None
_ausloeser: Callable[[str], None] | None = None

# Zustand
_unterbrochen_seit: float | None = None   # wann wurde der Strahl gebrochen?
_vogel_drin_seit: float | None = None     # wann ist er eingeflogen?

# Zähler, nur für die Anzeige
zaehler_durchfluege = 0
zaehler_ignoriert = 0


def start(ausloeser: Callable[[str], None] | None = None) -> bool:
    """Lichtschranke einschalten.

    ausloeser: wird gerufen, wenn ein Vogel erkannt wurde. Die Kamera
               hängt daran und fängt dann eine Aufnahme an.
    """
    global _sensor, _ausloeser
    _ausloeser = ausloeser

    if not cfg.LICHTSCHRANKE_AN:
        log.info("Lichtschranke ist in den Einstellungen abgeschaltet.")
        return False

    try:
        from gpiozero import DigitalInputDevice

        # Die meisten Module ziehen den Ausgang auf LOW, wenn der Strahl
        # gebrochen ist. "aktiv" soll bei uns "gebrochen" heißen.
        aktiv_bei = True if cfg.LICHTSCHRANKE_INVERTIERT else False

        _sensor = DigitalInputDevice(
            cfg.PIN_LICHTSCHRANKE,
            pull_up=None,
            active_state=aktiv_bei,
            bounce_time=0.005,
        )
        _sensor.when_activated = _strahl_gebrochen
        _sensor.when_deactivated = _strahl_wieder_frei

        log.info("Lichtschranke bereit an GPIO%d (Strahl ist gerade %s)",
                 cfg.PIN_LICHTSCHRANKE,
                 "GEBROCHEN" if _sensor.value else "frei")
        if _sensor.value:
            log.warning("Der Strahl ist beim Start schon gebrochen!")
            log.warning("Sender/Empfaenger justieren, oder "
                        "LICHTSCHRANKE_INVERTIERT umstellen.")
        return True

    except Exception as e:
        log.error("Lichtschranke nicht verfuegbar: %s", e)
        return False


# ---------------------------------------------------------------------------
#  Die zwei Ereignisse
# ---------------------------------------------------------------------------
def _strahl_gebrochen() -> None:
    global _unterbrochen_seit
    _unterbrochen_seit = time.monotonic()


def _strahl_wieder_frei() -> None:
    global _unterbrochen_seit, _vogel_drin_seit
    global zaehler_durchfluege, zaehler_ignoriert

    if _unterbrochen_seit is None:
        return

    dauer_ms = (time.monotonic() - _unterbrochen_seit) * 1000
    _unterbrochen_seit = None

    # --- Filter: war das überhaupt ein Vogel? ---
    if dauer_ms < cfg.MIN_UNTERBRECHUNG_MS:
        zaehler_ignoriert += 1
        log.debug("Zu kurz (%.0f ms) — ignoriert (Insekt? Zittern?)", dauer_ms)
        return

    if dauer_ms > cfg.MAX_UNTERBRECHUNG_MS:
        zaehler_ignoriert += 1
        log.info("Zu lang (%.0f ms) — ignoriert. Sitzt etwas im Loch?", dauer_ms)
        return

    zaehler_durchfluege += 1
    jetzt_wand = int(time.time())
    jetzt_mono = time.monotonic()

    # --- Ein- oder Ausflug? ---
    if _vogel_drin_seit is None:
        # Erste Unterbrechung: jemand ist hineingeflogen.
        _vogel_drin_seit = jetzt_mono
        statistik.besuch_eintragen(jetzt_wand, None, "besuch")
        log.info("EINFLUG (Unterbrechung %.0f ms)", dauer_ms)
    else:
        aufenthalt_ms = (jetzt_mono - _vogel_drin_seit) * 1000

        if aufenthalt_ms / 1000 > cfg.BESUCH_MAX_DAUER_S:
            # Zu lange her — wahrscheinlich haben wir einen Ausflug verpasst.
            # Dann werten wir das hier als neuen Einflug.
            log.info("Vorherigen Einflug verworfen (zu lange her). "
                     "Zaehle als neuen Einflug.")
            _vogel_drin_seit = jetzt_mono
            statistik.besuch_eintragen(jetzt_wand, None, "besuch")
        else:
            # Ausflug: jetzt kennen wir die Aufenthaltsdauer.
            _vogel_drin_seit = None
            statistik.besuch_dauer_nachtragen(int(aufenthalt_ms))
            log.info("AUSFLUG — war %.1f s drin", aufenthalt_ms / 1000)

    # Kamera anwerfen
    if _ausloeser is not None:
        try:
            _ausloeser("lichtschranke")
        except Exception as e:
            log.error("Ausloeser-Fehler: %s", e)


# ---------------------------------------------------------------------------
def vogel_ist_drin() -> bool:
    return _vogel_drin_seit is not None


def zustand() -> dict:
    return {
        "aktiv": _sensor is not None,
        "strahl_frei": (not _sensor.value) if _sensor is not None else None,
        "vogel_drin": vogel_ist_drin(),
        "durchfluege": zaehler_durchfluege,
        "ignoriert": zaehler_ignoriert,
    }


def aufraeumen() -> None:
    if _sensor is not None:
        try:
            _sensor.close()
        except Exception:
            pass
