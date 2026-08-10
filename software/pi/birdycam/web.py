"""
============================================================================
 web.py — Die Website
============================================================================
 Ein kleiner Webserver (Flask) direkt auf dem Pi. Kein Internet, keine
 Cloud, kein Abo: Wer im gleichen WLAN ist, ruft die Adresse auf und
 sieht alles.

 Adressen:
   /                     die Website
   /stream.mjpg          der Livestream (startet die Kamera bei Bedarf)
   /api/status           alle Zahlen als JSON
   /api/liste?typ=clips  Liste der Aufnahmen
   /api/telemetrie       Akkuverlauf der letzten 24 h
   /medien/clips/xy.mp4  eine Datei zum Ansehen oder Herunterladen
============================================================================
"""

import logging
import time
from pathlib import Path

from flask import (Flask, Response, abort, jsonify, render_template,
                   request, send_from_directory)

from . import einstellungen as cfg
from . import kamera, lichtschranke, ringspeicher, statistik, strom

log = logging.getLogger(__name__)

app = Flask(__name__)
_start_zeit = time.time()

# Flask soll seine eigenen Zugriffsmeldungen nicht in unser Protokoll spammen
logging.getLogger("werkzeug").setLevel(logging.WARNING)


# ---------------------------------------------------------------------------
@app.route("/")
def seite():
    return render_template("index.html")


# ---------------------------------------------------------------------------
@app.route("/api/status")
def api_status():
    volt, ampere = strom.letzte_werte()
    daten = statistik.uebersicht()
    daten.update({
        "kamera": kamera.zustand(),
        "lichtschranke": lichtschranke.zustand(),
        "akku": {
            "volt": round(volt, 2),
            "ampere": round(ampere, 3),
            "prozent": strom.prozent(),
            "laedt": strom.laedt(),
            "vorhanden": volt > 1.0,
        },
        "speicher": {
            "clips": ringspeicher.clips.belegt(),
            "clips_max": cfg.RING_CLIPS,
            "fotos": ringspeicher.fotos.belegt(),
            "fotos_max": cfg.RING_FOTOS,
            "frei_prozent": ringspeicher.platz_frei_prozent(),
        },
        "laufzeit_s": int(time.time() - _start_zeit),
        "zeit": int(time.time()),
    })
    return jsonify(daten)


# ---------------------------------------------------------------------------
@app.route("/api/liste")
def api_liste():
    typ = request.args.get("typ", "clips")
    if typ == "clips":
        return jsonify(ringspeicher.clips.dateien())
    if typ == "fotos":
        return jsonify(ringspeicher.fotos.dateien())
    abort(404)


@app.route("/api/telemetrie")
def api_telemetrie():
    stunden = request.args.get("stunden", default=24, type=int)
    return jsonify(statistik.letzte_telemetrie(max(1, min(stunden, 168))))


# ---------------------------------------------------------------------------
@app.route("/medien/<typ>/<name>")
def medien(typ: str, name: str):
    """Eine Aufnahme ausliefern.

    Die MP4-Clips laufen dadurch direkt im Browser — auch das Vorspulen,
    weil send_from_directory Teilanfragen ("Range") beantwortet.
    """
    if typ not in ("clips", "fotos"):
        abort(404)
    # Sicherheit: nur unsere eigenen Dateinamen zulassen, kein ".." o.ä.
    if "/" in name or "\\" in name or ".." in name:
        abort(403)
    if not (name.endswith(".mp4") or name.endswith(".jpg")):
        abort(403)

    ordner = Path(cfg.DATEN_PFAD) / typ
    if not (ordner / name).is_file():
        abort(404)
    return send_from_directory(ordner, name, conditional=True)


# ---------------------------------------------------------------------------
@app.route("/stream.mjpg")
def stream():
    """Der Livestream.

    Hier passiert die Umsetzung von "nur streamen, wenn sich jemand
    verbindet": Erst wenn diese Adresse aufgerufen wird, startet der
    MJPEG-Encoder — und wenn der letzte Zuschauer weg ist, hört er auf.
    """
    ausgabe = kamera.zuschauer_kommt()

    def bilder():
        try:
            while True:
                bild = ausgabe.naechstes_bild(timeout=10.0)
                if bild is None:
                    break
                yield (b"--birdyframe\r\n"
                       b"Content-Type: image/jpeg\r\n"
                       b"Content-Length: " + str(len(bild)).encode() + b"\r\n\r\n"
                       + bild + b"\r\n")
        finally:
            kamera.zuschauer_geht()

    return Response(bilder(),
                    mimetype="multipart/x-mixed-replace; boundary=birdyframe",
                    headers={"Cache-Control": "no-store, no-cache",
                             "Connection": "close"})


# ---------------------------------------------------------------------------
@app.errorhandler(404)
def nicht_gefunden(e):
    return jsonify({"fehler": "Gibt es nicht."}), 404


def starten() -> None:
    log.info("Website laeuft auf http://0.0.0.0:%d/", cfg.WEB_PORT)
    log.info("Im Heimnetz erreichbar als http://birdycam.local:%d/", cfg.WEB_PORT)
    # threaded=True ist wichtig: sonst blockiert der Livestream die Website.
    app.run(host="0.0.0.0", port=cfg.WEB_PORT, threaded=True,
            debug=False, use_reloader=False)
