"""
============================================================================
 main.py — hier fängt alles an
============================================================================
 Startreihenfolge (die Reihenfolge ist wichtig!):

   1. Protokoll einschalten          — damit man Fehler sieht
   2. Datenbank öffnen               — die Statistik muss zuerst da sein
   3. Strom: INA219 + IR-LEDs        — vor der Kamera, sie braucht das IR-Licht
   4. Kamera-Thread starten          — Bild, Aufnahme, Tag/Nacht
   5. Lichtschranke                  — hängt am Kamera-Auslöser
   6. Telemetrie-Thread              — Akku messen und Notaus überwachen
   7. Website                        — läuft im Haupt-Thread, für immer

 Zum Ausprobieren von Hand:
     cd /opt/birdycam && python3 -m birdycam.main

 Im Normalbetrieb macht das systemd:
     sudo systemctl start birdycam
     sudo journalctl -u birdycam -f        <- Protokoll live mitlesen
============================================================================
"""

import logging
import signal
import sys
import time
from pathlib import Path

from . import einstellungen as cfg
from . import kamera, lichtschranke, ringspeicher, statistik, strom, web

log = logging.getLogger("birdycam")


def protokoll_einrichten() -> None:
    logging.basicConfig(
        level=getattr(logging, cfg.LOG_LEVEL, logging.INFO),
        format="%(asctime)s  %(levelname)-8s %(name)-22s %(message)s",
        datefmt="%H:%M:%S",
        stream=sys.stdout,
    )


def aufraeumen(*_) -> None:
    log.info("Beende BirdyCam…")
    try:
        kamera.aufraeumen()
        lichtschranke.aufraeumen()
        strom.aufraeumen()
    except Exception as e:
        log.warning("Beim Aufräumen: %s", e)
    log.info("Tschüss.")
    sys.exit(0)


def main() -> None:
    protokoll_einrichten()

    log.info("=" * 58)
    log.info("  BirdyCam startet")
    log.info("=" * 58)

    # Sauber beenden, wenn systemd oder Strg+C das Programm stoppt
    signal.signal(signal.SIGTERM, aufraeumen)
    signal.signal(signal.SIGINT, aufraeumen)

    # --- 1. Liegt der Datenordner überhaupt auf der SSD? ----------------
    daten = Path(cfg.DATEN_PFAD)
    try:
        daten.mkdir(parents=True, exist_ok=True)
        pruef = daten / ".schreibtest"
        pruef.write_text("ok")
        pruef.unlink()
        log.info("Datenordner beschreibbar: %s", daten)
    except OSError as e:
        log.critical("Datenordner %s nicht beschreibbar: %s", daten, e)
        log.critical("Ist die SSD eingebunden? `lsblk` und `df -h` pruefen.")
        sys.exit(1)

    # --- 2. Datenbank ---------------------------------------------------
    statistik.start()
    statistik.aufraeumen(tage=180)

    # --- 3. Strom, IR-Licht ---------------------------------------------
    strom.start()
    volt, ampere = strom.messen()
    if volt > 1.0:
        log.info("Akku: %.2f V, %.3f A (%s)", volt, ampere,
                 "laedt" if ampere > 0.02 else "entlaedt")
    else:
        log.warning("Keine Akkumessung — laeuft der Pi am Netzteil?")

    # --- 4. Kamera ------------------------------------------------------
    kamera.starten()
    time.sleep(3)                      # der Kamera einen Moment geben
    if not kamera.zustand()["kamera_laeuft"]:
        log.error("Die Kamera laeuft nicht. Website startet trotzdem,")
        log.error("damit du den Akkustand sehen kannst.")

    # --- 5. Lichtschranke ----------------------------------------------
    lichtschranke.start(ausloeser=kamera.ausloesen)

    # --- 6. Telemetrie --------------------------------------------------
    strom.telemetrie_starten(kamera.ist_nacht)

    # --- 7. Website (blockiert für immer) -------------------------------
    log.info("-" * 58)
    log.info("  Alles bereit.  ->  http://birdycam.local:%d/", cfg.WEB_PORT)
    log.info("-" * 58)

    try:
        web.starten()
    except Exception as e:
        log.critical("Website abgestuerzt: %s", e)
        aufraeumen()


if __name__ == "__main__":
    main()
