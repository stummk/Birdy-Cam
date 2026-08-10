# BirdyCam — Software für den Raspberry Pi (Variante A)

Python-Programm für die Nistkasten-Kamera: Bewegungsclips in Full HD, Livestream auf
Abruf, Nachtsicht, Vogelzählung per Lichtschranke und eine Website mit Statistik.

**Vollständige Anleitung:** [../../docs/05-software-tutorial.md](../../docs/05-software-tutorial.md)

---

## Schnellstart

```bash
sudo ./install.sh
sudo nano /opt/birdycam/birdycam/einstellungen.py
sudo systemctl start birdycam
```

Dann öffnen: `http://birdycam.local:8080/`

Mitlesen, was das Programm tut:

```bash
sudo journalctl -u birdycam -f
```

---

## Voraussetzungen

| | |
|---|---|
| Hardware | Raspberry Pi Zero 2 W(H), NoIR-Kamera, USB-SSD |
| System | Raspberry Pi OS **Lite (32-bit)**, Bookworm oder neuer |
| Sensoren | INA219 (I2C), IR-Lichtschranke (GPIO17), IR-LEDs (GPIO18) |

Das Installationsskript installiert alles Nötige:
`python3-picamera2`, `python3-flask`, `python3-numpy`, `python3-gpiozero`, `python3-lgpio`,
`python3-av`, `smbus2`, `i2c-tools`.

> **Kein venv.** `picamera2` kommt von apt und passt zur libcamera-Version des Systems.
> Mit pip installiert gibt es fast immer Versionskonflikte. Deshalb läuft das Programm im
> System-Python.

---

## Aufbau

```
software/pi/
├── install.sh                   Installationsskript (gesprächig, mehrfach ausführbar)
├── systemd/birdycam.service     Autostart-Dienst
└── birdycam/
    ├── einstellungen.py         ⭐ ALLE Einstellungen — nur diese Datei anfassen
    ├── main.py                  Startreihenfolge
    ├── kamera.py                Bild, Clips, Livestream, Tag/Nacht, Nachtruhe
    ├── lichtschranke.py         Vögel zählen (Ein-/Ausflug, Aufenthaltsdauer)
    ├── strom.py                 INA219, IR-LED-PWM, Notabschaltung
    ├── ringspeicher.py          Round-Robin-Dateien
    ├── statistik.py             SQLite-Datenbank
    ├── web.py                   Flask-Website und Stream-Endpunkt
    └── templates/index.html     Die Website
```

**Pfade nach der Installation:**

| Was | Wo |
|---|---|
| Programm | `/opt/birdycam/` |
| Einstellungen | `/opt/birdycam/birdycam/einstellungen.py` |
| Daten (Clips, Fotos, Datenbank) | `/srv/birdycam/` |
| Dienst | `/etc/systemd/system/birdycam.service` |

`/srv` liegt automatisch auf der SSD, weil der Pi von der SSD bootet — es muss nichts
extra eingebunden werden.

---

## Wie es funktioniert

Die Kamera liefert **zwei Bilder gleichzeitig**:

| Stream | Größe | Wofür |
|---|---|---|
| `main` | 1920×1080 | H.264-Encoder → Ringpuffer → MP4-Clip |
| `lores` | 640×480 | Bewegungserkennung **und** Livestream |

**Der Vorlauf-Trick:** Der H.264-Encoder läuft immer und schreibt in einen
`CircularOutput2`, der die letzten 3 Sekunden behält — wie ein Eimer, der oben überläuft.
Löst die Lichtschranke aus, wird der Eimer zuerst in die Datei gekippt. Deshalb ist der
Anflug im Clip, obwohl die Aufnahme erst später begann.

**Zwei Auslöser, die verschiedene Dinge sehen:**

- **Lichtschranke** — jemand fliegt durch das Loch. Exakt, und **nur sie zählt** für die
  Statistik. Sonnenflecken interessieren sie nicht.
- **Bildvergleich** — im Kasten bewegt sich etwas (Füttern, Nestbau). Löst zusätzlich
  Aufnahmen aus, zählt aber nicht.

**Nachtruhe:** Wird es dunkel, schalten Kamera und Encoder ganz ab (~1,2 W gespart).
Bricht die Lichtschranke, fährt die Kamera in ~1,5 s hoch und nimmt 15 s auf. Alle 10
Minuten wird kurz geprüft, ob es schon hell wird.

**Livestream nur bei Zuschauern:** Der MJPEG-Encoder startet erst, wenn `/stream.mjpg`
aufgerufen wird, und stoppt, wenn der letzte Zuschauer weg ist.

---

## Die wichtigsten Einstellungen

Alle in `einstellungen.py`, danach `sudo systemctl restart birdycam`.

| Einstellung | Standard | Wirkung |
|---|---|---|
| `BEWEGUNG_SCHWELLE` | 8.0 | größer = unempfindlicher gegen Sonnenflecken |
| `BEWEGUNG_AN` | True | auf `False`: nur die Lichtschranke löst aus |
| `VORLAUF_MS` | 3000 | wie viel vor dem Auslöser im Clip ist |
| `BILDRATE` | 15 | 10 spart Strom, 30 ist flüssiger |
| `BITRATE` | 4 Mbit | kleiner = kleinere Dateien, gröberes Bild |
| `IR_HELLIGKEIT` | 0.30 | Nachtsicht-Helligkeit; `1.0` schaltet PWM aus |
| `NACHTRUHE_AN` | True | der größte Stromsparer |
| `RING_CLIPS` | 200 | wie viele Clips behalten werden |
| `AKKU_NOTAUS_VOLT` | 11.0 | darunter fährt der Pi sauber herunter |
| `LICHTSCHRANKE_INVERTIERT` | False | umstellen, wenn der Zähler ohne Vögel hochläuft |

---

## Schnittstellen

| Adresse | Rückgabe |
|---|---|
| `/` | die Website |
| `/stream.mjpg` | Livestream (MJPEG) |
| `/api/status` | alle Zahlen als JSON |
| `/api/liste?typ=clips` | Liste der Aufnahmen (`clips` oder `fotos`) |
| `/api/telemetrie?stunden=48` | Akkuverlauf |
| `/medien/clips/clip_047.mp4` | die Datei |

Beschreibung mit Beispielantwort: [../../docs/06-website-und-daten.md](../../docs/06-website-und-daten.md)

---

## Fehlersuche

```bash
sudo journalctl -u birdycam -b --no-pager | tail -50   # was ist passiert?
rpicam-hello --list-cameras                            # Kamera erkannt?
sudo i2cdetect -y 1                                    # INA219 auf 0x40?
findmnt -no SOURCE /                                   # läuft es von der SSD?
df -h /srv/birdycam                                    # Platz da?
```

| Symptom | Ursache |
|---|---|
| Pi startet ständig neu | **Häufigster Fehler:** 5-V-Versorgung zu schwach. 3-A-Wandler, kurzes dickes Kabel, ggf. USB-Y-Kabel für die SSD |
| „Datenordner nicht beschreibbar" | SSD nicht eingebunden |
| Kamera startet nicht | Flachbandkabel; **Zero-Kabel** benutzt (22-pol ↔ 15-pol)? |
| Clip nicht abspielbar | `sudo apt install python3-av` |
| Keine Akkuanzeige | I2C aktiviert? INA219 an **3V3**, Pin 3/5? |
| Besuchszähler läuft ohne Vögel hoch | Lichtschranke justieren, Streulicht abschirmen |
| „Out of memory" | `AUFNAHME_HOEHE = 720` oder `BILDRATE = 10` |

Ausführlich: [../../docs/07-wartung-und-fehlersuche.md](../../docs/07-wartung-und-fehlersuche.md)

---

## ⚠️ Status dieses Codes

Vollständig und sorgfältig geschrieben, aber **nicht auf echter Hardware ausgeführt** — er
ist am Schreibtisch entstanden. Deshalb ist das Tutorial in acht einzeln prüfbare Schritte
zerlegt: Wenn etwas klemmt, weiß man sofort, welcher Baustein es ist.

Verifiziert sind die verwendeten Schnittstellen gegen die offizielle Quelle:
[`CircularOutput2`](https://github.com/raspberrypi/picamera2/blob/main/picamera2/outputs/circularoutput2.py),
[`PyavOutput`](https://github.com/raspberrypi/picamera2/blob/main/picamera2/outputs/__init__.py),
[Bewegungserkennung mit `lores`-Stream](https://github.com/raspberrypi/picamera2/blob/main/examples/capture_motion.py).

Rechne beim ersten Mal mit Nacharbeit. Das ist normal — auch bei Profis.
