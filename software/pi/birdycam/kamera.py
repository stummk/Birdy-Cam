"""
============================================================================
 kamera.py — Bild, Aufnahme, Livestream
============================================================================
 Die Kamera liefert gleichzeitig ZWEI Bilder:

   "haupt"    1920 x 1080   ->  H.264-Encoder  ->  Ringpuffer  ->  MP4-Clip
   "vorschau"  640 x 480    ->  Bewegungserkennung + Livestream

 Warum zwei? Weil Bewegungserkennung auf einem kleinen Bild fast nichts
 kostet, das gespeicherte Video aber Full HD sein soll. Der Chip im Pi kann
 beide Größen parallel ausgeben — das kostet keine Rechenzeit.

 DER WICHTIGSTE TRICK: der Ringpuffer (CircularOutput2)
 -----------------------------------------------------
 Der H.264-Encoder läuft IMMER und schreibt in einen Ringpuffer, der die
 letzten 3 Sekunden behält. Kommt ein Vogel, wird der Puffer zuerst in die
 Datei geschüttet und dann weiter aufgenommen. So ist der ANFLUG mit im
 Clip — sonst sieht man den Vogel immer erst, wenn er schon im Kasten sitzt.

 NACHTRUHE
 ---------
 Nachts werden Kamera und Encoder ganz abgeschaltet (spart ~1,2 W).
 Bricht die Lichtschranke, fährt die Kamera in ~1,5 s hoch. Der Vorlauf
 fehlt dann — nachts kommt aber sowieso niemand an.
============================================================================
"""

import io
import logging
import threading
import time
from pathlib import Path

import numpy as np

from . import einstellungen as cfg
from . import ringspeicher, statistik, strom

log = logging.getLogger(__name__)


# ===========================================================================
#  Livestream-Ausgabe
# ===========================================================================
class StreamAusgabe(io.BufferedIOBase):
    """Hält immer nur das neueste JPEG. Wer zuschaut, holt sich das hier ab."""

    def __init__(self):
        self.bild: bytes | None = None
        self.bedingung = threading.Condition()

    def write(self, puffer):
        with self.bedingung:
            self.bild = bytes(puffer)
            self.bedingung.notify_all()
        return len(puffer)

    def naechstes_bild(self, timeout: float = 5.0) -> bytes | None:
        with self.bedingung:
            if not self.bedingung.wait(timeout):
                return None
            return self.bild


# ===========================================================================
#  Zustand
# ===========================================================================
_picam = None
_h264 = None
_ring = None                       # CircularOutput2
_mjpeg = None
_stream_ausgabe = StreamAusgabe()

_kamera_laeuft = False
_ist_nacht = False
_zuschauer = 0
_zuschauer_sperre = threading.Lock()

_aufnahme_laeuft = False
_aufnahme_start = 0.0
_aufnahme_pfad: Path | None = None
_aufnahme_ausloser = ""
_letzter_ausloeser = 0.0
_letztes_clip_ende = 0.0

_ausloese_signal = threading.Event()
_ausloese_quelle = "bild"

_helligkeit = 0
_bildrate_ist = 0.0
_laeuft = True


# ===========================================================================
#  Kamera an / aus
# ===========================================================================
def _kamera_starten() -> bool:
    global _picam, _h264, _ring, _kamera_laeuft

    if _kamera_laeuft:
        return True

    try:
        from picamera2 import Picamera2
        from picamera2.encoders import H264Encoder
        from picamera2.outputs import CircularOutput2

        if _picam is None:
            _picam = Picamera2()

            bilddauer = int(1_000_000 / cfg.BILDRATE)
            konfig_args = dict(
                main={"size": (cfg.AUFNAHME_BREITE, cfg.AUFNAHME_HOEHE),
                      "format": "YUV420"},
                lores={"size": (cfg.VORSCHAU_BREITE, cfg.VORSCHAU_HOEHE),
                       "format": "YUV420"},
                controls={"FrameDurationLimits": (bilddauer, bilddauer)},
                # Der Zero 2 W hat nur 512 MB RAM. Ein Full-HD-Bild in
                # YUV420 belegt ~3 MB — bei den üblichen 6 Puffern wären
                # das schon 18 MB nur für die Warteschlange. 4 reichen und
                # lassen mehr Luft für Python, Flask und den Encoder.
                buffer_count=4,
            )

            if cfg.BILD_SPIEGELN or cfg.BILD_DREHEN:
                from libcamera import Transform
                konfig_args["transform"] = Transform(
                    hflip=1 if cfg.BILD_SPIEGELN else 0,
                    vflip=1 if cfg.BILD_DREHEN else 0)

            konfig = _picam.create_video_configuration(**konfig_args)
            _picam.configure(konfig)
            log.info("Kamera konfiguriert: %dx%d @ %d fps, Vorschau %dx%d",
                     cfg.AUFNAHME_BREITE, cfg.AUFNAHME_HOEHE, cfg.BILDRATE,
                     cfg.VORSCHAU_BREITE, cfg.VORSCHAU_HOEHE)

        _h264 = H264Encoder(bitrate=cfg.BITRATE)
        _ring = CircularOutput2(buffer_duration_ms=cfg.VORLAUF_MS)

        # start_recording startet Kamera UND Encoder in einem Schritt.
        _picam.start_recording(_h264, _ring, name="main")

        _kamera_laeuft = True
        log.info("Kamera laeuft (Vorlauf-Puffer: %d ms)", cfg.VORLAUF_MS)
        return True

    except Exception as e:
        log.error("Kamera startet nicht: %s", e)
        log.error("Pruefe: Flachbandkabel richtig drin?")
        log.error("Pruefe: `rpicam-hello --list-cameras` zeigt die Kamera?")
        _kamera_laeuft = False
        return False


def _kamera_stoppen() -> None:
    global _kamera_laeuft, _h264, _ring

    if not _kamera_laeuft or _picam is None:
        return
    try:
        if _aufnahme_laeuft:
            _aufnahme_beenden()
        _stream_stoppen()
        _picam.stop_recording()
    except Exception as e:
        log.warning("Fehler beim Stoppen der Kamera: %s", e)

    _h264 = None
    _ring = None
    _kamera_laeuft = False
    log.info("Kamera aus (Nachtruhe).")


# ===========================================================================
#  Aufnahme
# ===========================================================================
def _foto_speichern() -> None:
    """Ein Standbild für die Galerie."""
    if _picam is None:
        return
    pfad = ringspeicher.fotos.naechster_pfad()
    try:
        _picam.capture_file(str(pfad), name="main", format="jpeg")
    except Exception:
        # Rückfallebene: das kleine Bild nehmen, das klappt immer
        try:
            _picam.capture_file(str(pfad), name="lores", format="jpeg")
        except Exception as e:
            log.warning("Foto fehlgeschlagen: %s", e)


def _aufnahme_starten(quelle: str) -> bool:
    global _aufnahme_laeuft, _aufnahme_start, _aufnahme_pfad, _aufnahme_ausloser

    if _aufnahme_laeuft or _ring is None:
        return False

    if time.monotonic() - _letztes_clip_ende < cfg.CLIP_PAUSE_S:
        return False

    _aufnahme_pfad = ringspeicher.clips.naechster_pfad()
    try:
        from picamera2.outputs import PyavOutput
        # Genau hier wird der Ringpuffer in die Datei geschüttet:
        # die 3 Sekunden VOR diesem Moment sind mit drin.
        _ring.open_output(PyavOutput(str(_aufnahme_pfad)))
    except Exception as e:
        log.error("Clip %s laesst sich nicht anlegen: %s", _aufnahme_pfad, e)
        return False

    _aufnahme_laeuft = True
    _aufnahme_start = time.monotonic()
    _aufnahme_ausloser = quelle
    log.info("CLIP START: %s (Ausloeser: %s)", _aufnahme_pfad.name, quelle)

    _foto_speichern()
    return True


def _aufnahme_beenden() -> None:
    global _aufnahme_laeuft, _letztes_clip_ende

    if not _aufnahme_laeuft or _ring is None:
        return

    dauer = time.monotonic() - _aufnahme_start
    try:
        _ring.close_output()
    except Exception as e:
        log.warning("Fehler beim Schliessen des Clips: %s", e)

    _aufnahme_laeuft = False
    _letztes_clip_ende = time.monotonic()

    groesse = 0
    if _aufnahme_pfad is not None:
        try:
            groesse = _aufnahme_pfad.stat().st_size
        except OSError:
            pass
        statistik.clip_eintragen(_aufnahme_pfad.name, round(dauer, 1),
                                 groesse, _ist_nacht, _aufnahme_ausloser)

    log.info("CLIP ENDE: %.1f s, %.1f MB", dauer, groesse / 1_048_576)


def ausloesen(quelle: str = "lichtschranke") -> None:
    """Von außen aufrufen (Lichtschranke), um eine Aufnahme anzustoßen."""
    global _ausloese_quelle
    _ausloese_quelle = quelle
    _ausloese_signal.set()


# ===========================================================================
#  Livestream
# ===========================================================================
def _stream_starten() -> bool:
    global _mjpeg
    if _mjpeg is not None or not _kamera_laeuft or _picam is None:
        return _mjpeg is not None
    try:
        from picamera2.encoders import MJPEGEncoder
        from picamera2.outputs import FileOutput

        stream_name = "main" if cfg.STREAM_QUELLE == "haupt" else "lores"
        _mjpeg = MJPEGEncoder()
        _picam.start_encoder(_mjpeg, FileOutput(_stream_ausgabe), name=stream_name)
        log.info("Livestream an (Quelle: %s)", stream_name)
        return True
    except Exception as e:
        log.error("Livestream startet nicht: %s", e)
        _mjpeg = None
        return False


def _stream_stoppen() -> None:
    global _mjpeg
    if _mjpeg is None or _picam is None:
        return
    try:
        _picam.stop_encoder(_mjpeg)
        log.info("Livestream aus (niemand schaut mehr zu).")
    except Exception as e:
        log.warning("Fehler beim Stoppen des Livestreams: %s", e)
    _mjpeg = None


def zuschauer_kommt() -> StreamAusgabe:
    """Die Website ruft das auf, wenn jemand den Stream öffnet.

    Das ist die Umsetzung von "nur streamen, wenn sich jemand verbindet":
    Der MJPEG-Encoder läuft wirklich nur, solange mindestens einer zuschaut.
    """
    global _zuschauer
    with _zuschauer_sperre:
        _zuschauer += 1
        if _zuschauer == 1:
            if not _kamera_laeuft:
                _kamera_starten()          # nachts extra hochfahren
            _stream_starten()
    return _stream_ausgabe


def zuschauer_geht() -> None:
    global _zuschauer
    with _zuschauer_sperre:
        _zuschauer = max(0, _zuschauer - 1)
        if _zuschauer == 0:
            _stream_stoppen()


def hat_zuschauer() -> bool:
    return _zuschauer > 0


# ===========================================================================
#  Bewegungserkennung
# ===========================================================================
_vorheriges_bild = None


def _bewegung_pruefen() -> tuple[bool, int]:
    """(Bewegung erkannt?, Helligkeit 0-255)"""
    global _vorheriges_bild

    if _picam is None:
        return False, 0
    try:
        # Bei YUV420 stehen die Helligkeitswerte in den ersten Zeilen.
        # Farbe interessiert uns nicht — Helligkeit reicht und ist billiger.
        roh = _picam.capture_array("lores")
        jetzt = roh[:cfg.VORSCHAU_HOEHE, :cfg.VORSCHAU_BREITE]
        # Jeden zweiten Punkt nehmen: viermal weniger Rechenarbeit,
        # für Bewegungserkennung völlig ausreichend.
        klein = jetzt[::2, ::2].astype(np.int16)
    except Exception as e:
        log.warning("Kein Vorschaubild: %s", e)
        return False, 0

    hell = int(klein.mean())

    bewegung = False
    if _vorheriges_bild is not None and _vorheriges_bild.shape == klein.shape:
        # "Mittlerer quadratischer Fehler": große Unterschiede zählen
        # stärker als viele kleine. Rauschen fällt dadurch weniger auf.
        mse = float(np.square(klein - _vorheriges_bild).mean())
        bewegung = mse > cfg.BEWEGUNG_SCHWELLE

    _vorheriges_bild = klein
    return bewegung, hell


def _bewegung_zuruecksetzen() -> None:
    """Nach Tag/Nacht-Wechsel, sonst löst der Lichtsprung Fehlalarm aus."""
    global _vorheriges_bild
    _vorheriges_bild = None


# ===========================================================================
#  Tag / Nacht
# ===========================================================================
def _nacht_pruefen(hell: int) -> None:
    global _ist_nacht

    war_nacht = _ist_nacht
    if not _ist_nacht and hell < cfg.NACHT_SCHWELLE:
        _ist_nacht = True
    elif _ist_nacht and hell > cfg.NACHT_SCHWELLE + cfg.NACHT_HYSTERESE:
        _ist_nacht = False

    if war_nacht != _ist_nacht:
        log.info("Umschalten auf %s (Helligkeit %d)",
                 "NACHT" if _ist_nacht else "TAG", hell)
        _bewegung_zuruecksetzen()
        if not _ist_nacht:
            strom.ir_aus()


def ist_nacht() -> bool:
    return _ist_nacht


# ===========================================================================
#  Die Hauptschleife
# ===========================================================================
def schleife() -> None:
    """Läuft in einem eigenen Thread, für immer."""
    global _helligkeit, _bildrate_ist, _letzter_ausloeser

    if not _kamera_starten():
        log.error("Ohne Kamera kein Betrieb. Der Thread beendet sich.")
        return

    zaehler, fenster_start = 0, time.monotonic()

    while _laeuft:
        # ---------------- NACHTRUHE ----------------
        if _ist_nacht and cfg.NACHTRUHE_AN and not hat_zuschauer():
            _nachtruhe()
            continue

        # ---------------- TAGBETRIEB ----------------
        if not _kamera_laeuft and not _kamera_starten():
            time.sleep(10)
            continue

        # IR-Licht bei Nacht (wenn wir wach sind, z. B. weil jemand zuschaut)
        if _ist_nacht and not strom.ir_ist_an():
            strom.ir_an()

        bewegung, hell = _bewegung_pruefen()
        _helligkeit = hell
        _nacht_pruefen(hell)

        # Auslöser sammeln: Lichtschranke ODER Bildbewegung
        ausgeloest, quelle = False, ""
        if _ausloese_signal.is_set():
            _ausloese_signal.clear()
            ausgeloest, quelle = True, _ausloese_quelle
        elif cfg.BEWEGUNG_AN and bewegung:
            ausgeloest, quelle = True, "bild"

        if ausgeloest:
            _letzter_ausloeser = time.monotonic()
            if not _aufnahme_laeuft:
                _aufnahme_starten(quelle)

        # Aufnahme beenden?
        if _aufnahme_laeuft:
            ruhe_vorbei = time.monotonic() - _letzter_ausloeser > cfg.NACHLAUF_S
            zu_lang = time.monotonic() - _aufnahme_start > cfg.CLIP_MAX_S
            if ruhe_vorbei or zu_lang:
                _aufnahme_beenden()

        # Bildrate für die Anzeige
        zaehler += 1
        if time.monotonic() - fenster_start >= 5.0:
            _bildrate_ist = zaehler / (time.monotonic() - fenster_start)
            zaehler, fenster_start = 0, time.monotonic()

        # ~5 Prüfungen pro Sekunde reichen und sparen Strom.
        time.sleep(0.2)


def _nachtruhe() -> None:
    """Kamera aus, auf die Lichtschranke warten, gelegentlich nach dem
    Tageslicht schauen."""
    global _helligkeit

    strom.ir_aus()
    _kamera_stoppen()

    geweckt = _ausloese_signal.wait(timeout=cfg.NACHT_CHECK_MINUTEN * 60)

    if geweckt:
        # --- Ein Vogel! Kamera hochfahren und aufnehmen. ---
        _ausloese_signal.clear()
        log.info("Nachts geweckt (%s) — fahre Kamera hoch.", _ausloese_quelle)
        strom.ir_an()
        if _kamera_starten():
            time.sleep(0.4)                  # Belichtung einschwingen lassen
            _bewegung_zuruecksetzen()
            if _aufnahme_starten(_ausloese_quelle):
                ende = time.monotonic() + cfg.NACHT_CLIP_S
                while time.monotonic() < ende and _laeuft:
                    time.sleep(0.2)
                _aufnahme_beenden()
        strom.ir_aus()
    else:
        # --- Zeit für den Helligkeitstest: wird es schon Tag? ---
        if _kamera_starten():
            time.sleep(0.5)
            _, hell = _bewegung_pruefen()
            _helligkeit = hell
            _nacht_pruefen(hell)
            log.debug("Nachtcheck: Helligkeit %d", hell)


# ===========================================================================
#  Für die Website
# ===========================================================================
def zustand() -> dict:
    return {
        "kamera_laeuft": _kamera_laeuft,
        "nacht": _ist_nacht,
        "helligkeit": _helligkeit,
        "ir_an": strom.ir_ist_an(),
        "aufnahme_laeuft": _aufnahme_laeuft,
        "zuschauer": _zuschauer,
        "pruefungen_pro_s": round(_bildrate_ist, 1),
        "aufloesung": f"{cfg.AUFNAHME_BREITE}x{cfg.AUFNAHME_HOEHE}",
        "bildrate": cfg.BILDRATE,
    }


def starten() -> threading.Thread:
    t = threading.Thread(target=schleife, name="kamera", daemon=True)
    t.start()
    return t


def aufraeumen() -> None:
    global _laeuft
    _laeuft = False
    time.sleep(0.4)
    _kamera_stoppen()
