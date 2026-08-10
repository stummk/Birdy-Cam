# 5. Software-Tutorial (Variante A — Raspberry Pi)

*(Tutorial für Variante B steht in [8. Variante ESP32](08-variante-esp32.md).)*

---

## Übersicht

| Schritt | Was du baust | Dauer |
|---|---|---|
| **1** | Pi einrichten, ins WLAN bringen | 40 min |
| **2** | Von der SSD booten, SD-Karte rausziehen | 40 min |
| **3** | Software installieren | 20 min | 
| **4** | 🎉 Website zum ersten Mal | 15 min | 
| **5** | 🎉 **Kamera dran — das erste Livebild** | 30 min | 
| **6** | Unsichtbares Licht (Handy-Trick) | 20 min | 
| **7** | Lichtschranke justieren | 30 min | 
| **8** | Akku, Panel, Probebetrieb | 45 min |

Was du brauchst: einen Computer, ein Micro-USB-Handy-Netzteil (mindestens 1,5 A), die
Teile aus der [Stückliste](02-stueckliste.md).

---

## Schritt 1 — Den Pi einrichten

### Betriebssystem auf die SD-Karte

1. [Raspberry Pi Imager](https://www.raspberrypi.com/software/) herunterladen und
   installieren.
2. Imager starten:
   - **Modell:** Raspberry Pi Zero 2 W
   - **Betriebssystem:** Raspberry Pi OS **Lite (32-bit)**
   - **Speicher:** die 16-GB-microSD (A6)

> **Warum „Lite" und warum 32-bit?** „Lite" heißt: ohne Desktop-Oberfläche. Wir brauchen
> keine — und sie würde Strom kosten. **32-bit**, weil der Zero 2 W nur **512 MB
> Arbeitsspeicher** hat; die 64-Bit-Version braucht davon merklich mehr. Bei uns läuft
> Full-HD-Video darauf, da zählt jedes Megabyte.

3. Vor dem Schreiben auf das **Zahnrad** klicken (Einstellungen vorab):

| Einstellung | Wert |
|---|---|
| Hostname | `birdycam` |
| SSH aktivieren | ✅ ja, mit Passwort |
| Benutzername | `pi` (oder wie du magst) |
| Passwort | ein eigenes, aufschreiben! |
| WLAN | SSID + Passwort, Land **DE** |
| Zeitzone | Europe/Berlin |

4. Schreiben, warten, Karte auswerfen.

### Erster Start

Karte in den Pi, Netzteil an die **PWR**-Buchse (die am Rand!). Zwei Minuten warten — der
erste Start dauert länger.

Dann vom Computer aus verbinden. Unter Windows in der PowerShell:

```bash
ssh pi@birdycam.local
```

Klappt `birdycam.local` nicht, schau im Router nach der IP-Adresse und nimm die:

```bash
ssh pi@192.168.1.87
```

Es kommt eine Frage („Are you sure…"), mit `yes` beantworten, dann das Passwort eingeben.
Wenn `pi@birdycam:~ $` erscheint: **du bist drin.**

Noch schnell alles aktualisieren:

```bash
sudo apt update && sudo apt full-upgrade -y
```

> 💡 **Was du gerade gelernt hast:** Du hast keinen Bildschirm und keine Tastatur am Pi —
> du steuerst ihn durch das Netzwerk. Das nennt man **SSH**. Alles, was du tippst, wird
> verschlüsselt zum Pi geschickt und dort ausgeführt. So werden auch die meisten Server im
> Internet bedient: Niemand steht mit einer Tastatur vor einem Rechenzentrum.

**Klappt nicht?**

| Problem | Lösung |
|---|---|
| Pi erscheint nicht im Netzwerk | WLAN-Passwort im Imager falsch? Land auf DE gestellt? 2,4-GHz-Netz nötig |
| Grüne LED blinkt nicht | Netzteil an der **PWR**-Buchse (am Rand), nicht an der inneren |
| `ssh: connect refused` | SSH im Imager aktiviert? Nochmal Karte schreiben |
| `birdycam.local` unbekannt | IP aus dem Router nehmen |

---

## Schritt 2 — Von der SSD booten

Das ist der Schritt, der die SD-Karte dauerhaft loswird. Ab jetzt läuft alles auf der SSD,
und das Ausfallrisiko der Speicherkarte ist weg.

### 2a — Ein Schalter im Pi umlegen

Der Pi Zero 2 W kann von USB booten, aber nur wenn ein Schalter im Chip gesetzt ist. Der
heißt **OTP-Bit** — *One Time Programmable*, also **einmal und für immer**.

```bash
sudo nano /boot/firmware/config.txt
```

Ganz unten anhängen:

```
program_usb_boot_mode=1
```

Speichern: `Strg+O`, `Enter`, `Strg+X`. Dann neu starten:

```bash
sudo reboot
```

Nach dem Neustart wieder einloggen und prüfen:

```bash
vcgencmd otp_dump | grep 17:
```

Steht dort eine Zahl, die auf **`a`** endet (z. B. `17:3020000a`), ist der Schalter
gesetzt. ✅

> ⚠️ **„Einmal und für immer" heißt wirklich für immer.** Man kann das Bit nicht
> zurücksetzen. Das ist unbedenklich — der Pi bootet danach weiterhin auch von SD-Karte,
> er kann nur *zusätzlich* von USB. Es geht also nichts verloren.

### 2b — Betriebssystem auf die SSD

SSD über den USB-OTG-Adapter an den **Computer** (nicht an den Pi) stecken. Im Imager
genau dieselben Einstellungen wie in Schritt 1 verwenden — gleicher Hostname, gleiches
WLAN, SSH an — und auf die **SSD** schreiben.

### 2c — Umstecken

```bash
sudo shutdown -h now
```

Warten, bis die grüne LED aus bleibt. Dann:

1. Netzteil abziehen
2. **SD-Karte herausziehen** und weglegen (nicht wegwerfen — sie ist die Rückfallebene!)
3. SSD über den OTG-Adapter an die **innere** Micro-USB-Buchse (DATA)
4. Netzteil an die **äußere** Buchse (PWR)

Der erste Start von SSD dauert länger, bis zu zwei Minuten. Dann:

```bash
ssh pi@birdycam.local
findmnt -no SOURCE /
```

Steht dort `/dev/sda2` (und nicht `/dev/mmcblk0p2`), läuft das System **von der SSD**. 🎉

> 💡 **Was du gerade gelernt hast:** SD-Karten und SSDs speichern beide in „Flash"-Zellen,
> aber eine SSD hat einen eigenen kleinen Computer eingebaut, der die Schreibvorgänge
> geschickt über alle Zellen verteilt. Eine SD-Karte kann das kaum. Deshalb hält eine SSD
> bei Dauerbetrieb *zwanzigmal* länger. Und deshalb war dieser Schritt die Mühe wert.

**Klappt nicht?**

| Problem | Lösung |
|---|---|
| Pi bootet nicht von SSD | SSD in der **inneren** Buchse? OTP-Bit gesetzt (2a)? |
| Bootet, aber stürzt ab | Netzteil zu schwach. Mindestens 1,5 A, besser 2,5 A |
| Immer noch nicht | **Rückfallebene:** SD-Karte drin lassen zum Booten, SSD nur für Daten. In `einstellungen.py` bleibt `DATEN_PFAD` dann auf dem SSD-Einbindepunkt. Funktioniert genauso gut, nur die SD-Karte bleibt im Spiel |

---

## Schritt 3 — Software installieren

Projekt auf den Pi holen und Installationsskript starten:

```bash
sudo apt install -y git
git clone <deine-projekt-url> ~/birdy-cam
cd ~/birdy-cam/software/pi
chmod +x install.sh
sudo ./install.sh
```

*(Kein Git? Dann den Ordner `software/pi` per `scp` kopieren:
`scp -r software/pi pi@birdycam.local:~/`)*

Das Skript erzählt in acht Schritten, was es tut: Pakete installieren, I2C einschalten,
grüne LED abschalten, Programm nach `/opt/birdycam` kopieren, Datenordner anlegen, Dienst
einrichten und am Ende prüfen, ob Kamera und Sensor gefunden werden.

Am Ende steht, dass die Kamera noch fehlt — **das ist in Ordnung**, die kommt in Schritt 5.

Dann einmal neu starten (wegen der LED-Einstellung):

```bash
sudo reboot
```

> 💡 **Was du gerade gelernt hast:** Ein Installationsskript ist eine Liste von Befehlen,
> die der Computer der Reihe nach abarbeitet — genau die, die man sonst von Hand tippen
> würde. Der Vorteil: Es vergisst nichts, und man kann es beim nächsten Pi wieder benutzen.

---

## Schritt 4 — Website zum ersten Mal 🎉

Programm starten:

```bash
sudo systemctl start birdycam
```

Und zuschauen, was es sagt:

```bash
sudo journalctl -u birdycam -f
```

Du solltest so etwas sehen:

```
==========================================================
  BirdyCam startet
==========================================================
Datenordner beschreibbar: /srv/birdycam
Datenbank bereit: /srv/birdycam/birdycam.db
IR-LEDs bereit an GPIO18
Keine Akkumessung — laeuft der Pi am Netzteil?
Kamera startet nicht: ...
Lichtschranke ist in den Einstellungen abgeschaltet.
----------------------------------------------------------
  Alles bereit.  ->  http://birdycam.local:8080/
----------------------------------------------------------
```

**Die Fehlermeldungen sind jetzt richtig so** — Kamera, Akkusensor und Lichtschranke sind
ja noch nicht angeschlossen. Wichtig ist die letzte Zeile.

Mit `Strg+C` das Mitlesen beenden (das Programm läuft weiter) und im Browser öffnen:

**`http://birdycam.local:8080/`**

Da ist deine Website! Livebild ist noch schwarz, alle Zähler stehen auf 0 — aber sie läuft.

> 💡 **Was du gerade gelernt hast:** Der Pi ist jetzt ein **Webserver**. Wenn du die Adresse
> aufrufst, fragt dein Browser: „Hast du eine Seite für mich?" und der Pi schickt sie. Genau
> so funktioniert jede Website im Internet — nur steht der Rechner sonst nicht im
> Wohnzimmer.
>
> Und noch etwas: `systemctl` ist der Chef über alle Programme, die automatisch starten.
> Wenn der Strom ausfällt und wieder kommt, startet er BirdyCam von selbst. Deshalb muss
> niemand hinlaufen.

**Die vier Befehle, die du dir merken solltest:**

```bash
sudo systemctl status birdycam
```

```bash
sudo systemctl restart birdycam
```

```bash
sudo journalctl -u birdycam -f
```

```bash
sudo nano /opt/birdycam/birdycam/einstellungen.py
```

---

## Schritt 5 — Die Kamera 🎉

Der schönste Schritt.

**⚠️ Zuerst herunterfahren und Strom abziehen:**

```bash
sudo shutdown -h now
```

Warten, bis die LED aus bleibt, dann Netzteil abziehen. **Niemals die Kamera bei
eingeschaltetem Pi anstecken.**

Jetzt das CSI-Kabel anschließen — genau wie in
[Schaltplan 3.7](03-schaltplan.md#kamera) beschrieben. Denk an das **Zero-Kabel** (das aus
der Kamerapackung passt nicht).

Strom an, warten, einloggen und testen:

```bash
rpicam-hello --list-cameras
```

Erscheint dort eine Kamera (z. B. `ov5647`), ist sie erkannt. Ein Testbild:

```bash
rpicam-jpeg -o test.jpg --width 1920 --height 1080
```

Dann BirdyCam neu starten und die Website öffnen:

```bash
sudo systemctl restart birdycam
```

**`http://birdycam.local:8080/`** — und da ist das Livebild. Wedel mit der Hand davor.

Wedel länger — nach ein paar Sekunden erscheint unten in der Liste ein **Clip**. Klick
drauf: Er läuft direkt im Browser. Und achte darauf, dass du dich **schon von Anfang an**
im Bild siehst — das ist der Vorlauf-Puffer!

> 💡 **Was du gerade gelernt hast:** Die Kamera liefert **zwei Bilder gleichzeitig** — ein
> großes (1920×1080) für die Aufnahme und ein kleines (640×480) zum Bewegungsuchen. Auf dem
> kleinen zu suchen ist viel billiger: Es hat 16-mal weniger Punkte zu vergleichen.
>
> **Und der Vorlauf-Trick:** Der Pi nimmt *immer* auf, wirft aber ständig weg, was älter als
> 3 Sekunden ist — wie ein Eimer, der oben überläuft. Wenn Bewegung kommt, wird der Eimer
> zuerst in die Datei gekippt. Deshalb ist der Anflug mit im Clip, obwohl die Aufnahme erst
> später gestartet wurde.

**Klappt nicht?**

| Problem | Lösung |
|---|---|
| `no cameras available` | Flachbandkabel: Bügel auf, Kabel gerade rein, Bügel zu. **Zero-Kabel** benutzt? Kontakte richtig herum? |
| Livebild schwarz, aber Kamera erkannt | Objektivdeckel ab? Genug Licht? |
| Bild verrauscht/gestreift | Kabel neu verlegen, keine Knicke |
| Bild auf dem Kopf | `BILD_DREHEN = True` in `einstellungen.py` |
| Clip lässt sich nicht abspielen | `python3-av` installiert? `sudo apt install python3-av` |
| Website hakt beim Zuschauen | Normal bei schwachem WLAN. `STREAM_QUELLE = "vorschau"` prüfen |
| „Out of memory" im Protokoll | `AUFNAHME_HOEHE = 720` probieren, oder 32-bit-OS verwenden |

---

## Schritt 6 — Unsichtbares Licht

Herunterfahren, Strom ab, MOSFET-Modul und IR-LEDs anschließen — nach
[Schaltplan 3.5](03-schaltplan.md#35-die-ir-beleuchtung).

⚠️ **Erinnerung:** Die Steuerseite des MOSFET-Moduls (`VCC`) kommt an **Pin 1 (3V3)**, die
Lastseite (`VIN+`) an **Pin 2 (5V)**. Verwechseln zerstört nichts, aber die LEDs bleiben
dunkel.

Strom an, dann testen — hier direkt von der Kommandozeile:

```bash
sudo python3 -c "
from gpiozero import PWMOutputDevice
from time import sleep
led = PWMOutputDevice(18, frequency=1000)
print('Halte die FRONTKAMERA deines Handys auf die LEDs!')
for runde in range(3):
    for i in range(0, 101, 5):
        led.value = i/100; sleep(0.05)
    for i in range(100, -1, -5):
        led.value = i/100; sleep(0.05)
led.value = 0
print('fertig')
"
```

**Das Experiment:** Halte die **Frontkamera** deines Handys auf die IR-LEDs. (Die
Frontkamera — die hintere hat oft einen Filter.)

Mit deinen Augen: **nichts**. Auf dem Handydisplay: die LEDs leuchten und werden heller und
dunkler!

> 💡 **Was du gerade gelernt hast:** Licht ist eine Welle, und die Länge der Welle bestimmt
> die Farbe. Rot hat etwa 700 Nanometer. Unsere LEDs haben 940 — weiter als rot, deshalb
> heißt es **Infrarot** („unter-rot"). Menschen sehen es nicht. Vögel auch nicht — das ist
> wichtig, denn die Kamera soll sie nachts nicht stören. Der Handysensor kann es sehen, weil
> er aus einem anderen Material gebaut ist als deine Augen.
>
> **Und wie „dimmt" man eine LED?** Man kann sie nicht halb einschalten. Stattdessen wird
> sie **1000-mal pro Sekunde** an- und ausgeschaltet. Bei 30 % „an" wirkt sie 30 % hell,
> weil unser Auge zu langsam ist, um das Flackern zu sehen. Das nennt man **PWM**.

**Der Nachttest:** Mach das Zimmer dunkel und schau auf die Website. Nach ein paar Sekunden
schaltet die Anzeige auf 🌙 **Nacht**, die IR-LEDs gehen an, und das Livebild wird
grau — aber du siehst etwas!

**Klappt nicht?**

| Problem | Lösung |
|---|---|
| Handy sieht nichts | Frontkamera? Ganz nah ran? |
| Nichts, auch nah nicht | **Ist es ein Logic-Level-MOSFET?** Ein IRF520 schaltet bei 3,3 V nicht durch |
| LEDs immer an | Steuer- und Lastseite verwechselt |
| LEDs sehr schwach | LEDs falsch herum? Bei LEDs zählt die Richtung |
| Nachtbild zeigt Streifen | `IR_HELLIGKEIT = 1.0` setzen (dann ist PWM aus) |

---

## Schritt 7 — Die Lichtschranke justieren

Das Bauteil, das die Statistik ehrlich macht.

Herunterfahren, Strom ab, anschließen nach
[Schaltplan 3.6](03-schaltplan.md#36-die-lichtschranke-im-einflugloch).

> ⚠️ **`VCC` an Pin 1 (3V3), nicht an Pin 2 (5V)!** Der Pi verträgt an GPIO-Pins maximal
> 3,3 V. Ein 5-V-Signal zerstört ihn sofort. Siehe
> [3.3](03-schaltplan.md#33-️-die-wichtigste-warnung-33-v-nicht-5-v).

In `einstellungen.py` einschalten:

```bash
sudo nano /opt/birdycam/birdycam/einstellungen.py
```

```python
LICHTSCHRANKE_AN = True
```

Dann eine Live-Anzeige zum Justieren:

```bash
sudo python3 -c "
from gpiozero import DigitalInputDevice
from time import sleep
s = DigitalInputDevice(17, pull_up=None, active_state=True)
print('Strg+C zum Beenden. Fahre mit dem Finger durch den Strahl!')
alt = None
while True:
    if s.value != alt:
        alt = s.value
        print('Strahl:', 'UNTERBROCHEN' if alt else 'frei')
    sleep(0.02)
"
```

**Justieren:** Sender und Empfänger so ausrichten, dass „frei" stabil angezeigt wird. Dann
mit dem Finger durchfahren — es muss sofort auf „UNTERBROCHEN" springen und danach wieder
zurück.

**Springt es dauernd hin und her?** Die beiden Teile schauen sich nicht genau an. Ein
Stück Strohhalm über den Empfänger schieben — das bündelt und schirmt Streulicht ab.

**Ist es genau umgekehrt** (frei = UNTERBROCHEN)? Dann in `einstellungen.py`:

```python
LICHTSCHRANKE_INVERTIERT = True
```

Neu starten und ausprobieren:

```bash
sudo systemctl restart birdycam
sudo journalctl -u birdycam -f
```

Fahre mit der Hand durch. Im Protokoll erscheint:

```
lichtschranke  EINFLUG (Unterbrechung 180 ms)
kamera         CLIP START: clip_003.mp4 (Ausloeser: lichtschranke)
```

Und nochmal:

```
lichtschranke  AUSFLUG — war 12.4 s drin
```

Auf der Website steht jetzt **🐣 Vogel ist drin**, und der Zähler ist um 1 gestiegen.

> 💡 **Was du gerade gelernt hast:** Der Pi merkt sich, *wann* der Strahl gebrochen wurde,
> und rechnet beim zweiten Mal die Differenz aus. Aus zwei einfachen Ereignissen wird so
> eine echte Messung: die Aufenthaltsdauer.
>
> Und ein Programmierer-Gedanke: Warum werden Unterbrechungen unter 30 Millisekunden
> ignoriert? Weil eine Fliege auch den Strahl bricht. Man muss dem Computer sagen, was
> *nicht* zählt — das ist oft schwieriger als zu sagen, was zählt.

---

## Schritt 8 — Akku, Panel, Probebetrieb

Der letzte Schritt vor dem Einbau.

**Reihenfolge einhalten** (siehe [Schaltplan 3.9](03-schaltplan.md#39-sicherheitsregeln)):
erst Akku an den Laderegler, dann Panel, dann Last. INA219 in die Akkuleitung, Sicherung
nicht vergessen.

INA219 prüfen:

```bash
sudo i2cdetect -y 1
```

Bei `40` muss eine `40` stehen. Dann:

```bash
sudo systemctl restart birdycam
```

Auf der Website steht jetzt der Akkustand — und die spannende Zeile:

> ⚡ Die Sonne lädt gerade mit **0,84 A**.

> 🧒 **Das Experiment, das alles erklärt:** Halte die Hand über das Panel und schau auf die
> Website. Der Ladestrom bricht sofort ein. Hand weg — er kommt zurück. Jetzt weißt du,
> warum das Panel nicht in den Schatten gehört, und du hast es *gemessen*, nicht geglaubt.

> 💡 **Was du gerade gelernt hast:** Der INA219 misst den Strom über einen winzigen
> Widerstand (0,1 Ohm). Fließt Strom durch einen Widerstand, entsteht eine kleine Spannung
> daran — bei 1 Ampere sind es 0,1 Volt. Aus dieser Spannung rechnet der Chip den Strom
> zurück. Man kann Strom nämlich nicht direkt messen, immer nur seine Wirkung.
>
> **Vorzeichen beachten:** Positiv heißt „Akku wird geladen", negativ heißt „Akku wird
> entladen". Wenn abends die Zahl negativ wird, weißt du: Jetzt lebt die Kamera vom Akku.

### Der Probebetrieb — nicht überspringen

**Zwei Wochen laufen lassen, bevor irgendetwas in den Nistkasten kommt.** Am besten auf
dem Balkon oder im Garten, Panel provisorisch aufgestellt.

Täglich 30 Sekunden auf die Website schauen:

| Prüfpunkt | Gut | Nicht gut |
|---|---|---|
| Akku morgens | über 12,8 V | unter 12,4 V |
| Ladestrom mittags | positiv, ≥ 0,3 A | 0 A → Panel prüfen |
| Akku über die Tage | pendelt, steigt tagsüber | sinkt jeden Tag ein Stück |
| Clips | entstehen | keine → Empfindlichkeit prüfen |

**Sinkt der Akku über mehrere Tage durchgehend**, reicht das Panel nicht — Standort
verbessern, größeres Panel, oder `NACHTRUHE_AN` und `BILDRATE` sparsamer einstellen.

Dieser Schritt ist der Unterschied zwischen „hat funktioniert" und „hat **im April**
funktioniert". Danach: [4. Bauplan](04-bauplan.md).

---

## Feinjustieren

Alles in `einstellungen.py`, danach `sudo systemctl restart birdycam`:

| Problem | Ändern |
|---|---|
| Zu viele Clips (Sonnenflecken, Blätter) | `BEWEGUNG_SCHWELLE` erhöhen (8 → 15 → 25) |
| Bewegungen werden nicht erkannt | `BEWEGUNG_SCHWELLE` senken (8 → 5) |
| Nur noch Lichtschranke soll auslösen | `BEWEGUNG_AN = False` |
| Clips zu kurz | `NACHLAUF_S` erhöhen |
| Anflug fehlt am Anfang | `VORLAUF_MS` erhöhen (3000 → 5000) |
| Nachtbild zu dunkel | `IR_HELLIGKEIT` erhöhen (0.30 → 0.60) |
| Nachtbild in der Mitte überstrahlt | `IR_HELLIGKEIT` senken, LEDs weiter von der Linse |
| Akku hält nicht durch | `BILDRATE = 10`, `NACHTRUHE_AN = True`, `BITRATE = 2_000_000` |
| Livestream soll Full HD sein | `STREAM_QUELLE = "haupt"` (kostet mehr Strom) |
| Speicher soll länger reichen | `BITRATE` senken oder `RING_CLIPS` erhöhen |

---

## Wenn etwas gar nicht geht

| Symptom | Ursache & Lösung |
|---|---|
| Dienst startet nicht | `sudo journalctl -u birdycam -b` zeigt den Grund |
| „Datenordner nicht beschreibbar" | SSD nicht eingebunden. `lsblk` und `df -h` prüfen |
| Pi startet ständig neu | Netzteil/Wandler zu schwach. **Häufigster Fehler.** 3-A-Wandler |
| Kamera geht nach Stunden aus | Meist Speichermangel. `BILDRATE` und `buffer_count` senken |
| Website erreichbar, Livebild nicht | Nachts ist die Kamera aus — Seite neu laden weckt sie |
| Uhrzeit falsch | Kein Internet beim Booten. Ohne NTP gibt es keine echten Zeitstempel |
| Alles läuft, aber keine Besuche | Lichtschranke justieren (Schritt 7) |

→ Weiter mit [6. Website & Daten](06-website-und-daten.md)
