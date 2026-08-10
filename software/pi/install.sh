#!/usr/bin/env bash
# ============================================================================
#  BirdyCam — Installationsskript für den Raspberry Pi
# ============================================================================
#  Aufrufen:
#     cd ~/birdy-cam/software/pi
#     chmod +x install.sh
#     sudo ./install.sh
#
#  Das Skript ist absichtlich gesprächig: Es sagt bei jedem Schritt, was es
#  tut und warum. Man kann es mehrfach laufen lassen — es überschreibt nur,
#  was nötig ist, und lässt einstellungen.py in Ruhe, wenn es sie schon gibt.
# ============================================================================

set -euo pipefail

CODE_ZIEL="/opt/birdycam"
DATEN_ZIEL="/srv/birdycam"
QUELLE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

blau()  { printf '\n\033[1;36m==> %s\033[0m\n' "$*"; }
gruen() { printf '\033[0;32m    ✓ %s\033[0m\n' "$*"; }
warn()  { printf '\033[0;33m    ! %s\033[0m\n' "$*"; }
fehler(){ printf '\033[0;31m    ✗ %s\033[0m\n' "$*"; }

if [[ $EUID -ne 0 ]]; then
  fehler "Bitte mit sudo starten:  sudo ./install.sh"
  exit 1
fi

# ---------------------------------------------------------------------------
blau "1/8  Wo läuft das System?"
# ---------------------------------------------------------------------------
WURZEL_GERAET="$(findmnt -no SOURCE / || echo unbekannt)"
echo "     Wurzelverzeichnis liegt auf: $WURZEL_GERAET"
if [[ "$WURZEL_GERAET" == /dev/mmcblk* ]]; then
  warn "Das System läuft noch von der SD-KARTE."
  warn "Für den Dauerbetrieb sollte es von der SSD booten —"
  warn "siehe Tutorial Schritt 2. Die Installation geht trotzdem weiter."
else
  gruen "System läuft nicht von der SD-Karte. Gut."
fi

# ---------------------------------------------------------------------------
blau "2/8  Systempakete installieren"
# ---------------------------------------------------------------------------
# picamera2 kommt bewusst von apt und nicht von pip: Die Version von apt
# passt zur libcamera-Version des Systems. Mit pip gibt es fast immer Ärger.
apt-get update -qq
apt-get install -y --no-install-recommends \
  python3-picamera2 \
  python3-flask \
  python3-numpy \
  python3-gpiozero \
  python3-lgpio \
  python3-av \
  i2c-tools
gruen "Pakete installiert"

# smbus2 (für den INA219) ist nicht überall als Paket vorhanden
if python3 -c "import smbus2" 2>/dev/null; then
  gruen "smbus2 ist schon da"
elif apt-get install -y python3-smbus2 2>/dev/null; then
  gruen "smbus2 über apt installiert"
else
  warn "smbus2 nicht als Paket verfügbar — nehme pip"
  pip3 install --break-system-packages smbus2
  gruen "smbus2 über pip installiert"
fi

# ---------------------------------------------------------------------------
blau "3/8  I2C einschalten (für den Akku-Sensor INA219)"
# ---------------------------------------------------------------------------
if command -v raspi-config >/dev/null; then
  raspi-config nonint do_i2c 0
  gruen "I2C eingeschaltet"
else
  warn "raspi-config nicht gefunden — I2C bitte von Hand einschalten"
fi

# ---------------------------------------------------------------------------
blau "4/8  Kleine Systemeinstellungen"
# ---------------------------------------------------------------------------
BOOTCFG="/boot/firmware/config.txt"
[[ -f "$BOOTCFG" ]] || BOOTCFG="/boot/config.txt"

zeile_setzen() {
  local zeile="$1"
  if grep -qxF "$zeile" "$BOOTCFG"; then
    echo "     schon vorhanden: $zeile"
  else
    echo "$zeile" >> "$BOOTCFG"
    gruen "hinzugefügt: $zeile"
  fi
}

if [[ -f "$BOOTCFG" ]]; then
  echo "     Bearbeite $BOOTCFG"
  # Grüne Aktivitäts-LED aus: kein sichtbares Licht am Nistkasten
  zeile_setzen "dtparam=act_led_trigger=none"
  zeile_setzen "dtparam=act_led_activelow=off"
  # Kamera-Erkennung automatisch (Standard, hier nur zur Sicherheit)
  zeile_setzen "camera_auto_detect=1"
  warn "Diese Änderungen greifen erst nach einem Neustart."
else
  warn "config.txt nicht gefunden — übersprungen"
fi

# ---------------------------------------------------------------------------
blau "5/8  Programm nach $CODE_ZIEL kopieren"
# ---------------------------------------------------------------------------
mkdir -p "$CODE_ZIEL"

# einstellungen.py NICHT überschreiben, wenn sie schon angepasst wurde
EINST_GESICHERT=0
if [[ -f "$CODE_ZIEL/birdycam/einstellungen.py" ]]; then
  cp "$CODE_ZIEL/birdycam/einstellungen.py" /tmp/birdycam_einstellungen.bak
  EINST_GESICHERT=1
  warn "Vorhandene einstellungen.py gesichert"
fi

rm -rf "$CODE_ZIEL/birdycam"
cp -r "$QUELLE/birdycam" "$CODE_ZIEL/"

if [[ $EINST_GESICHERT -eq 1 ]]; then
  cp /tmp/birdycam_einstellungen.bak "$CODE_ZIEL/birdycam/einstellungen.py"
  gruen "Deine einstellungen.py wurde behalten"
  echo "     (Neue Vorlage liegt in $QUELLE/birdycam/einstellungen.py)"
fi
gruen "Programm kopiert"

# ---------------------------------------------------------------------------
blau "6/8  Datenordner $DATEN_ZIEL anlegen"
# ---------------------------------------------------------------------------
# Weil der Pi von der SSD bootet, liegt /srv automatisch auf der SSD.
# Es muss also nichts extra eingebunden werden.
mkdir -p "$DATEN_ZIEL"/{clips,fotos}
chmod 755 "$DATEN_ZIEL"
gruen "Ordner angelegt: $DATEN_ZIEL/{clips,fotos}"

PLATZ="$(df -h --output=avail "$DATEN_ZIEL" | tail -1 | tr -d ' ')"
echo "     Freier Platz: $PLATZ"

# ---------------------------------------------------------------------------
blau "7/8  Dienst einrichten (startet automatisch beim Booten)"
# ---------------------------------------------------------------------------
cp "$QUELLE/systemd/birdycam.service" /etc/systemd/system/
systemctl daemon-reload
systemctl enable birdycam >/dev/null 2>&1
gruen "Dienst eingerichtet und für den Autostart aktiviert"

# ---------------------------------------------------------------------------
blau "8/8  Prüfen, ob die Hardware da ist"
# ---------------------------------------------------------------------------
echo "     --- Kamera ---"
if command -v rpicam-hello >/dev/null; then
  if rpicam-hello --list-cameras 2>&1 | grep -qi "Available cameras"; then
    rpicam-hello --list-cameras 2>&1 | sed 's/^/     /' | head -12
    gruen "Kamera gefunden"
  else
    fehler "Keine Kamera gefunden — Flachbandkabel prüfen!"
  fi
else
  warn "rpicam-hello nicht vorhanden"
fi

echo "     --- I2C / INA219 ---"
if command -v i2cdetect >/dev/null; then
  if i2cdetect -y 1 2>/dev/null | grep -q " 40"; then
    gruen "INA219 auf Adresse 0x40 gefunden"
  else
    warn "Kein INA219 auf 0x40. Ohne ihn gibt es keine Akkuanzeige."
    warn "Prüfen: SDA an Pin 3, SCL an Pin 5, VCC an Pin 1 (3V3!), GND an Pin 6"
  fi
else
  warn "i2cdetect nicht vorhanden"
fi

# ---------------------------------------------------------------------------
cat <<'ENDE'

============================================================================
  Fertig!
============================================================================

  JETZT NOCH EINSTELLUNGEN PRÜFEN:

      sudo nano /opt/birdycam/birdycam/einstellungen.py

  Danach starten:

      sudo systemctl start birdycam

  Zuschauen, was passiert:

      sudo journalctl -u birdycam -f

  Website öffnen (am Handy oder Computer im gleichen WLAN):

      http://birdycam.local:8080/

  Wenn .local nicht geht, die IP-Adresse benutzen:

      hostname -I

  ⚠️  Falls in Schritt 4 etwas an der config.txt geändert wurde:
      einmal `sudo reboot`, dann läuft alles.

============================================================================
ENDE
