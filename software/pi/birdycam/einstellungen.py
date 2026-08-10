"""
============================================================================
 BirdyCam — Einstellungen
============================================================================
 Das ist die einzige Datei, die du normalerweise anfasst.
 Alles hier kann man ändern, ohne den Rest zu verstehen.

 Nach jeder Änderung:
     sudo systemctl restart birdycam
============================================================================
"""

# ---------------------------------------------------------------------------
# 1. Netzwerk
# ---------------------------------------------------------------------------
# Das WLAN wird NICHT hier eingetragen, sondern beim Einrichten des Pi
# (Raspberry Pi Imager oder `sudo raspi-config`). Siehe Tutorial Schritt 1.

WEB_PORT = 8080          # Website:  http://birdycam.local:8080/
                         # Port 80 würde root-Rechte brauchen — 8080 ist einfacher.

# ---------------------------------------------------------------------------
# 2. Bild
# ---------------------------------------------------------------------------
# Auflösung der AUFNAHMEN (Full HD).
AUFNAHME_BREITE = 1920
AUFNAHME_HOEHE = 1080

# Bilder pro Sekunde. 15 reicht für Vögel völlig und halbiert
# gegenüber 30 die Encoder-Arbeit und die Dateigröße.
BILDRATE = 15

# Datenrate des H.264-Encoders in Bit pro Sekunde.
# 4 Mbit/s ist bei 1080p15 eine gute Qualität. Mehr = schöner + größer.
BITRATE = 4_000_000

# Auflösung des kleinen Zweitbilds. Wird für zwei Dinge benutzt:
# Bewegungserkennung UND Livestream.
VORSCHAU_BREITE = 640
VORSCHAU_HOEHE = 480

# Woher kommt der Livestream?
#   "vorschau" = 640x480, flüssig und sparsam  (empfohlen)
#   "haupt"    = 1920x1080, schön, aber mehr Strom und Bandbreite
STREAM_QUELLE = "vorschau"

# Bild spiegeln / auf den Kopf stellen (je nach Einbaulage der Kamera)
BILD_SPIEGELN = False
BILD_DREHEN = False

# ---------------------------------------------------------------------------
# 3. Bewegungserkennung (im Bild)
# ---------------------------------------------------------------------------
# Achtung: Gezählt werden die Besuche von der LICHTSCHRANKE, nicht von hier.
# Die Bilderkennung löst nur zusätzlich Aufnahmen aus — z. B. wenn im Kasten
# gefüttert wird, ohne dass jemand durch das Loch fliegt.

BEWEGUNG_AN = True

# Wie stark muss sich das Bild ändern? Größer = unempfindlicher.
# Sinnvoll: 4 (sehr empfindlich) bis 25 (nur große Bewegungen).
BEWEGUNG_SCHWELLE = 8.0

# ---------------------------------------------------------------------------
# 4. Clips
# ---------------------------------------------------------------------------
# So lange VOR dem Auslöser wird mitgespeichert (der Anflug!).
# Das ist der Trick, ohne den man den Vogel immer erst im Kasten sieht.
VORLAUF_MS = 3000

# Nach dem letzten Auslöser noch so viele Sekunden weiter aufnehmen.
NACHLAUF_S = 4

# Ein Clip ist höchstens so lang.
CLIP_MAX_S = 25

# Kürzeste Pause zwischen zwei Clips (verhindert 100 Clips pro Minute).
CLIP_PAUSE_S = 5

# ---------------------------------------------------------------------------
# 5. Speicher (Round-Robin-Ringe)
# ---------------------------------------------------------------------------
# Wo liegen die Daten? Auf der SSD, nicht auf der SD-Karte.
DATEN_PFAD = "/srv/birdycam"

# So viele Dateien werden im Kreis überschrieben. Die älteste fällt raus.
RING_CLIPS = 200         # ~1 GB bei 1080p/4 Mbit
RING_FOTOS = 1000        # ~250 MB

# ---------------------------------------------------------------------------
# 6. Nachtsicht
# ---------------------------------------------------------------------------
# Ab welcher Helligkeit gilt es als Nacht? (0 = stockdunkel, 255 = gleißend)
NACHT_SCHWELLE = 45

# Damit es in der Dämmerung nicht dauernd hin und her springt:
NACHT_HYSTERESE = 25

# IR-LED-Helligkeit 0.0 bis 1.0. 0.3 = 30 %.
# Höher = helleres Nachtbild, aber mehr Strom und mehr Wärme im Kasten.
IR_HELLIGKEIT = 0.30

# --- Nachtruhe: der größte Stromsparer ---
# Nachts werden Kamera und Encoder ganz abgeschaltet. Bricht die
# Lichtschranke, fährt die Kamera in ~1,5 s hoch und nimmt auf.
# Der Vorlauf fehlt dann — nachts kommt aber sowieso niemand an.
NACHTRUHE_AN = True

# Wie lange wird nach einem nächtlichen Auslöser aufgenommen?
NACHT_CLIP_S = 15

# Wie oft schaut sie nachts nach, ob es schon hell wird? (Minuten)
NACHT_CHECK_MINUTEN = 10

# ---------------------------------------------------------------------------
# 7. Lichtschranke
# ---------------------------------------------------------------------------
LICHTSCHRANKE_AN = True

# Kürzere Unterbrechungen werden ignoriert (Insekten, Zittern, Wind).
MIN_UNTERBRECHUNG_MS = 30

# Längere Unterbrechungen sind kein Vogel (Blatt im Loch, Verschmutzung).
MAX_UNTERBRECHUNG_MS = 2000

# Zwei Unterbrechungen innerhalb dieser Zeit werden als
# "Einflug ... Ausflug" gedeutet -> daraus wird die Aufenthaltsdauer.
BESUCH_MAX_DAUER_S = 900

# Manche Lichtschranken-Module liefern LOW, wenn der Strahl frei ist,
# andere HIGH. Wenn der Zähler dauernd hochläuft: hier umstellen.
LICHTSCHRANKE_INVERTIERT = False

# ---------------------------------------------------------------------------
# 8. Strom & Akku
# ---------------------------------------------------------------------------
AKKU_MESSEN = True

# Widerstand des Messwiderstands auf dem INA219-Modul (fast immer 0,1 Ohm).
INA219_SHUNT_OHM = 0.1

# Unter dieser Spannung fährt der Pi sauber herunter.
# Danach trennt der Laderegler die Last und schaltet sie bei Sonne
# wieder ein -> der Pi bootet von selbst. Siehe Schaltplan 3.4.
AKKU_NOTAUS_VOLT = 11.0

# Damit ein kurzer Spannungseinbruch (z. B. beim SSD-Schreiben) nicht
# gleich zum Herunterfahren führt: so viele Messungen hintereinander
# müssen zu niedrig sein.
NOTAUS_MESSUNGEN = 10

# Wie oft wird der Akku gemessen? (Sekunden)
TELEMETRIE_INTERVALL_S = 30

# Wie oft wird ein Messwert dauerhaft gespeichert? (Sekunden)
TELEMETRIE_SPEICHERN_S = 300

# ---------------------------------------------------------------------------
# 9. Pins  (siehe Schaltplan 3.2 — nur ändern, wenn du umgesteckt hast)
# ---------------------------------------------------------------------------
PIN_IR_LED = 18          # Pin 12 -> MOSFET-Modul SIG (echter Hardware-PWM)
PIN_LICHTSCHRANKE = 17   # Pin 11 <- Lichtschranke OUT
# INA219 hängt an I2C: Pin 3 (SDA) und Pin 5 (SCL)

# ---------------------------------------------------------------------------
# 10. Protokoll
# ---------------------------------------------------------------------------
# "DEBUG" zeigt alles (gut zur Fehlersuche), "INFO" nur das Wichtige.
LOG_LEVEL = "INFO"
