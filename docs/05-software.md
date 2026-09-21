# 5. Software — vom leeren Board zur fertigen Vogelkamera

Hier passiert das Programmieren. Keine Sorge: Du musst **keinen Code schreiben**. Du lädst
fertige Programme auf das Board und änderst höchstens ein paar Zeilen mit deinen eigenen
Werten.

**Der Plan:** sieben kleine Lernprogramme, die jedes für sich funktionieren — und danach die
fertige Firmware. Wenn eines nicht klappt, weißt du sofort, wo der Fehler steckt.

> 🎉 **Das erste Livebild kommt in Schritt 3, nicht am Ende.** Das ist Absicht.

---

## 5.1 Schritt 0 — Die Arduino IDE einrichten (einmalig, ~30 Minuten)

Die **Arduino IDE** ist das Programm auf deinem Computer, mit dem du Programme auf das Board
schiebst. Sie ist kostenlos und gibt es für Windows, Mac und Linux.

### Installieren

1. [Arduino IDE 2.x herunterladen](https://www.arduino.cc/en/software) und installieren.

2. **Datei → Einstellungen** öffnen. Ganz unten gibt es ein Feld
   **„Zusätzliche Boardverwalter-URLs“**. Dort diese Zeile eintragen:

```
https://espressif.github.io/arduino-esp32/package_esp32_index.json
```

   *(Das ist die Adresse, unter der die IDE erfährt, wie man ESP32-Boards anspricht. Sie
   kennt ab Werk nur echte Arduinos.)*

3. **Werkzeuge → Board → Boardverwalter** öffnen, nach `esp32` suchen und das Paket von
   **Espressif Systems** installieren.
   ⚠️ **Version 3.x oder neuer** — die Mikrofon-Bibliothek gibt es erst ab dort.
   Der Download ist groß, das dauert ein paar Minuten.

### Die vier Einstellungen unter „Werkzeuge“

| Einstellung | Wert |
|---|---|
| Board | **XIAO_ESP32S3** |
| Port | der COM-Port, der neu dazukommt, sobald du das Board ansteckst |
| **PSRAM** | **OPI PSRAM** ⚠️ |
| Upload Speed | 921600 |

> ⚠️ **PSRAM ist die Einstellung, an der die meisten scheitern.** Steht sie auf „Disabled“,
> startet die Kamera nie — und die Fehlermeldung sagt dir nicht, warum. Wenn später „Kamera
> startet nicht“ kommt: **zuerst hier nachsehen.**

### Das Board wird nicht gefunden?

1. Anderes USB-Kabel probieren. Viele billige Kabel sind **reine Ladekabel** ohne
   Datenadern — mit denen passiert gar nichts. Das ist der häufigste Grund.
2. Klappt es immer noch nicht: USB abziehen, die **BOOT**-Taste gedrückt halten, USB
   anstecken, BOOT loslassen. Danach erscheint der Port.
3. Nach dem Hochladen einmal **RESET** drücken.

### Der „Serielle Monitor“ — dein wichtigstes Werkzeug

Das ist ein Fenster in der Arduino IDE, in dem das Board dir Textmeldungen schickt. Du
öffnest es über **Werkzeuge → Serieller Monitor** und stellst unten rechts **115200 Baud**
ein.

Wenn du dieses Fenster offen hast, siehst du bei jedem Programm, was gerade passiert. **Ohne
den Seriellen Monitor arbeitest du blind.**

---

## 5.2 Die sieben Lern-Sketches

Ein „Sketch“ ist einfach ein Arduino-Programm. Alle sieben liegen in
[`software/firmware/steps/`](../software/firmware/steps/).

**So gehst du bei jedem vor:** Ordner in der Arduino IDE öffnen (Datei → Öffnen, die
`.ino`-Datei auswählen) → Pfeil-Symbol zum Hochladen anklicken → Seriellen Monitor öffnen.

| # | Sketch | Was du siehst | Vorher anstecken | Dauer |
|---|---|---|---|---|
| **1** | [`step1_hallo`](../software/firmware/steps/step1_hallo/step1_hallo.ino) | Die LED blinkt, das Board erzählt von sich | nur USB | 15 min |
| **2** | [`step2_sdkarte`](../software/firmware/steps/step2_sdkarte/step2_sdkarte.ino) | Karte erkannt, Schreibtest, Geschwindigkeit | SD-Karte einschieben | 15 min |
| **3** | [`step3_kamera`](../software/firmware/steps/step3_kamera/step3_kamera.ino) | 🎉 **das erste Livebild im Browser** | Kameramodul ans Flachband | 30 min |
| **4** | [`step4_irlicht`](../software/firmware/steps/step4_irlicht/step4_irlicht.ino) | Unsichtbares Licht — mit dem Handy-Trick | MOSFET + 4 IR-LEDs | 20 min |
| **5** | [`step5_akku`](../software/firmware/steps/step5_akku/step5_akku.ino) | Akku messen und **kalibrieren** | Spannungssensor | 25 min |
| **6** | [`step6_mikrofon`](../software/firmware/steps/step6_mikrofon/step6_mikrofon.ino) | Lautstärkebalken, Tonaufnahme | nichts (Mikro ist drauf) | 25 min |
| **7** | [`step7_lichtschranke`](../software/firmware/steps/step7_lichtschranke/step7_lichtschranke.ino) | Einflug und Ausflug mit Dauer | Lichtschranke | 30 min |

**Ganz oben in jeder Datei** steht, was vorher zu verkabeln ist. **Ganz unten** steht eine
Fehlertabelle für genau diesen Schritt. Die Verkabelung selbst erklärt
[Kapitel 3](03-schaltplan.md).

### Was du in den einzelnen Schritten lernst

**Schritt 1 — das Board lebt.** Du siehst zum ersten Mal, dass etwas passiert, wenn du auf
„Hochladen“ drückst. Und du lernst den Seriellen Monitor kennen.

**Schritt 2 — Speicher.** Das Programm misst, wie schnell deine Karte schreiben kann.
**Merk dir diesen Wert.** Er entscheidet später über die Bildqualität: Unter etwa 0,8 MB/s
wird es eng, ab 1,0 MB/s ist alles gut.

**Schritt 3 — das erste Bild.** Das Board macht ein eigenes kleines WLAN oder hängt sich an
deinen Router, und du rufst auf dem Handy eine Adresse auf. Der Moment, in dem das Projekt
plötzlich echt wird.

**Schritt 4 — unsichtbares Licht.** Die IR-LEDs leuchten, aber du siehst nichts. Dann hältst
du die **Frontkamera deines Handys** davor und siehst ein schwaches violett-weißes Glimmen.
Viele Handysensoren sehen Infrarot, unsere Augen nicht.

> Dieser Sketch hat noch eine zweite Aufgabe: Er sagt dir, ob dein MOSFET-Modul an unseren
> 5 Volt sauber durchschaltet. Bei der verbreiteten kleinen Bauform ist das nicht
> selbstverständlich — [Schaltplan 3.6](03-schaltplan.md#36-das-unsichtbare-nachtlicht--mosfet-und-ir-leds)
> erklärt, warum, und was du tust, wenn es nicht klappt.

**Schritt 5 — echte Messtechnik.** Hier kalibrierst du den Spannungssensor. Das geht so:

1. Sketch hochladen, Seriellen Monitor öffnen, die Zeile `Am Pin: x.xxx V` ablesen.
2. Die **echte** Akkuspannung feststellen — mit einem Multimeter direkt am Akku, oder du
   nimmst einen frisch geladenen LiPo, der hat fast genau **4,20 V**.
3. Rechnen: `FAKTOR = echte Spannung ÷ Spannung am Pin`.
   Beispiel: 4,20 V ÷ 0,84 V = **5,00**.
4. Diesen Faktor im Sketch bei `FAKTOR` eintragen, neu hochladen, prüfen.
5. Denselben Wert später in `config.h` bei `BATT_KALIBRIERUNG` eintragen.

> 💡 **Den Faktor hast du vielleicht schon.** Wenn du den Sensor nach
> [Schaltplan 3.8](03-schaltplan.md#durchtesten-mit-dem-multimeter--vier-messungen)
> mit dem Multimeter durchgetestet hast, ist er dort in Messung 4 schon herausgekommen.
> Dann dient dieser Sketch nur noch der Gegenprobe.

**Schritt 6 — Ton.** Ein Lautstärkebalken im Seriellen Monitor. Reden, pfeifen,
in die Hände klatschen. Wenn sich der Balken bewegt, funktioniert das Mikrofon.

**Schritt 7 — die Lichtschranke justieren** (siehe unten).

---

## Schritt 7 — die Lichtschranke justieren

Der einzige Schritt, bei dem du wirklich etwas mit den Händen einstellen musst. Nimm dir
Zeit dafür, es lohnt sich.

> **Überspringbar.** Die Lichtschranke ist optionales Zubehör und in `config.h` ab Werk
> abgeschaltet. Baust du sie (erstmal) nicht ein, lass diesen Schritt aus und
> `LICHTSCHRANKE_AN` auf `false` — der Rest funktioniert unverändert.

1. Sketch hochladen, Seriellen Monitor öffnen. Dort steht laufend **`frei`** oder
   **`UNTERBROCHEN`**.
2. Sender und Empfänger so ausrichten, dass **`frei` stabil stehen bleibt** — es darf nicht
   flackern. Falls das Modul ein kleines Drehrädchen (Potentiometer) hat, damit die
   Empfindlichkeit einstellen.
3. Mit dem Finger durch das Loch fahren. Die Anzeige muss **sofort** umschalten.
4. **Zweimal** durchfahren, mit einer Pause dazwischen. Jetzt siehst du im Monitor
   `EINFLUG` und `AUSFLUG` mit der Dauer dazwischen. Das ist genau das, was später auf der
   Website als „Aufenthaltsdauer“ steht.
5. **Erst wenn das sauber klappt**, wird geklebt ([Bauplan 4.4](04-bauplan.md#44-die-lichtschranke-einbauen)).
6. Zum Schluss in `config.h` **`LICHTSCHRANKE_AN true`** setzen — sonst bleibt sie trotz
   Einbau stumm.

> **Alles genau andersherum?** Zeigt der Monitor `UNTERBROCHEN`, wenn nichts im Weg ist, und
> `frei`, wenn du den Finger reinhältst — dann liefert dein Modul das Signal umgekehrt. Im
> Sketch `INVERTIERT` auf `true` stellen (und später auch `LICHTSCHRANKE_INVERTIERT` in
> `config.h`). Das ist kein Defekt, es gibt beide Bauarten.

| Problem | Lösung |
|---|---|
| Anzeige flackert dauernd | Sender und Empfänger schauen nicht genau geradeaus. Nachjustieren |
| Sonne blendet den Empfänger | 5 mm Strohhalm als Blende darüberschieben |
| Reagiert gar nicht | `OUT` steckt nicht auf `D2`, oder das GND-Kabel fehlt |
| Löst zu leicht aus | `MIN_UNTERBRECHUNG` von 30 auf 60 erhöhen |

---

## 5.3 Schritt 8 — die fertige Firmware

Öffne den Ordner [`software/firmware/birdycam/`](../software/firmware/birdycam/) in der
Arduino IDE (Datei → Öffnen → `birdycam.ino`). Alle Dateien erscheinen oben als Reiter.

> **Du musst nur eine einzige Datei anfassen:**
> [`config.h`](../software/firmware/birdycam/config.h).
> Alles andere kannst du in Ruhe lassen.

### Die vier Zeilen, die du eintragen musst

```cpp
#define NETZ_MODUS      NETZ_AUTO              // Router, eigenes WLAN, oder beides
#define WLAN_NAME       "HierDeinWLANName"     // <- dein WLAN-Name
#define WLAN_PASSWORT   "HierDeinPasswort"     // <- dein WLAN-Passwort
#define BATT_KALIBRIERUNG  5.00                // <- dein Wert aus Schritt 5
```

Mehr ist nicht nötig. Alles andere hat sinnvolle Voreinstellungen.

**Nur falls du die Lichtschranke eingebaut hast** — sie ist ab Werk abgeschaltet, weil
sie optionales Zubehör ist:

```cpp
#define LICHTSCHRANKE_AN          true         // <- schaltet sie überhaupt erst ein
#define LICHTSCHRANKE_INVERTIERT  false        // <- ggf. aus Schritt 7
```

Setz `LICHTSCHRANKE_AN` erst auf `true`, wenn das Modul angeschlossen **und** nach
Schritt 7 justiert ist. Ein offener Eingang zählt sonst Phantom-Besuche.

### Netzwerk

| Einstellung | Standard | Wirkung |
|---|---|---|
| `NETZ_MODUS` | `NETZ_AUTO` | Erst den Router probieren, sonst eigenes WLAN aufmachen |
| | `NETZ_ROUTER` | Nur das Heimnetz. Am sparsamsten, Uhrzeit kommt automatisch |
| | `NETZ_EIGENES` | Nur eigenes WLAN. Überall im Garten, aber **+40 % Strom** |
| `AP_NAME` | `BirdyCam` | So heißt das eigene WLAN |
| `AP_PASSWORT` | `vogelhaus` | mindestens 8 Zeichen! |
| `AP_NACHTS_AUS` | `false` | `true` spart ~2,5 Wh/Tag im eigenen WLAN |

### Bild und Speicher — die vier wichtigen Regler

| Einstellung | Standard | Wirkung |
|---|---|---|
| `BILD_GROESSE` | `FRAMESIZE_UXGA` | 1600×1200. `FRAMESIZE_SVGA` = kleiner, dafür ~15 Bilder/s |
| `BILD_QUALITAET` | `18` | Kleinere Zahl = schöner **und** größer. **Unter 16 wird die SD-Karte zum Engpass** |
| `STREAM_HAT_VORRANG` | `true` | Beim Zuschauen wird nicht aufgenommen |
| `RING_CLIPS` | `200` | So viele Clips werden im Kreis überschrieben (~1 GB) |

### Ton

| Einstellung | Standard | Wirkung |
|---|---|---|
| `AUDIO_AN` | `true` | Hauptschalter fürs Mikrofon |
| `TON_IN_CLIPS` | `true` | Clips bekommen eine Tonspur (+4 % Platz) |
| `TON_IM_STREAM` | `true` | Ton im Livestream, Knopf „🔊 Ton an“ auf der Website |
| `TON_VERSTAERKUNG` | `2` | Im Kasten ist es leise. Zu hoch = es knackt |

### Hochladen und der erste Start

Nach dem Hochladen siehst du im Seriellen Monitor (115200 Baud) so etwas:

```
=====  BirdyCam startet  =====
[System] PSRAM: 8189 KB frei
[Strom] Bereit.
[Kamera] Laeuft.
[SD] Karte da: 30500 MB
[Bewegung] Vergleichsbild: 240 x 135 Punkte
[SD] Takt: 40 MHz
[Licht] Lichtschranke bereit an GPIO3 (Strahl ist frei)
[WLAN] Verbunden. IP: 192.168.1.87 (-52 dBm)
[WLAN] Erreichbar als http://birdycam.local/
[Audio] Mikrofon laeuft: 16000 Hz, Verstaerkung 2x, Ton in Clips, Ton im Stream
[Web] Website:    http://192.168.1.87/
[Web] Livestream: http://192.168.1.87:81/
[Web] Tonstream:  http://192.168.1.87:82/  (auf der Website "Ton an")
=====  BirdyCam ist bereit  =====
```

**Dann im Browser `http://birdycam.local/` öffnen.** Fertig.

> **`birdycam.local` funktioniert nicht?** Manche Handys und Router können mit
> `.local`-Namen nichts anfangen. Nimm stattdessen die IP-Adresse, die im Seriellen Monitor
> steht (im Beispiel `192.168.1.87`). Im eigenen WLAN ist es immer **`192.168.4.1`**.

### So verbindest du dich mit dem eigenen WLAN

1. Am Handy in die WLAN-Einstellungen gehen.
2. Netz **`BirdyCam`** auswählen, Passwort **`vogelhaus`** eingeben.
3. Browser öffnen: **`http://192.168.4.1`**

Die Kamera hat ein **Captive Portal** eingebaut — sie leitet jede eingegebene Adresse auf
sich selbst um. Viele Handys öffnen die Seite dadurch von selbst, so wie im Hotel-WLAN.

> ⚠️ **Ein Handy-Ärgernis, das man kennen muss:** Android und iOS merken, dass dieses WLAN
> kein Internet hat, und wechseln gern zurück ins Mobilfunknetz. Wenn die Seite plötzlich
> nicht mehr lädt: In den WLAN-Einstellungen bei der Meldung „Kein Internetzugriff“ auf
> **„Verbindung beibehalten“** tippen.

---

## 5.4 Was in den einzelnen Dateien steckt

Nur für Neugierige — anfassen musst du keine davon.

| Datei | Aufgabe |
|---|---|
| [`birdycam.ino`](../software/firmware/birdycam/birdycam.ino) | Das Hauptprogramm |
| [`config.h`](../software/firmware/birdycam/config.h) | **Alle Einstellungen** — die einzige Datei für dich |
| `camera_pins.h` | Pinbelegung der Kamera (nicht ändern) |
| `bewegung.cpp` | Vergleicht Bilder miteinander, um Bewegung zu finden |
| `lichtschranke.cpp` | Zählt Vögel |
| `avi.cpp` | Baut aus Bildern und Ton eine Videodatei |
| `speicher.cpp` | Ringspeicher, Statistik, Tagesarchiv |
| `strom.cpp` | Akku überwachen, IR-Licht dimmen, Notaus |
| `systeminfo.cpp` | Systemzustand für die Website |
| `audio.cpp` | Mikrofon und Tonspeicher |
| `netzwerk.cpp` | Router oder eigenes WLAN, Uhrzeit |
| `web.cpp` | Die Website, der Livestream und der Tonstream |

### Ein Blick in `avi.cpp` — wie eine Videodatei entsteht

Der ESP32 kann Video nicht komprimieren. Er kann aber sehr schnell Einzelfotos machen. Ein
**MJPEG-AVI** ist genau das: viele JPEG-Bilder hintereinander, mit einem kleinen
„Deckblatt“ davor, auf dem steht „spiel das mit 10 Bildern pro Sekunde ab“.

Ein paar Zahlen im Deckblatt (etwa: wie viele Bilder es geworden sind) kennt man erst am
**Ende**. Deshalb schreibt das Programm zuerst Platzhalter hinein und springt zum Schluss
zurück, um die richtigen Werte einzutragen. Dieses Muster kommt in Dateiformaten sehr oft
vor — und es ist eine schöne Art zu verstehen, wie Dateien eigentlich aufgebaut sind.

Mit Ton wird das Deckblatt größer, weil eine **zweite Spur** beschrieben werden muss.
Danach wechseln sich die Häppchen ab: ein Stück Bild, ein Stück Ton, ein Stück Bild, ein
Stück Ton. Genau dafür steht das I in AVI: **A**udio **V**ideo **I**nterleave — Ton und Bild
ineinander verschachtelt.

---

## 5.5 Der Probebetrieb — bitte nicht überspringen

Bevor irgendetwas in den Nistkasten kommt: **zwei Wochen laufen lassen.** Auf dem Tisch,
auf dem Fensterbrett, im Schuppen — Hauptsache erreichbar.

Täglich einmal auf die Website schauen und vier Dinge prüfen:

| Prüfpunkt | Gut | Nicht gut |
|---|---|---|
| **Akku** | steigt tagsüber, sinkt nachts | sinkt über mehrere Tage durchgehend |
| **Laufzeit** | wächst kontinuierlich | springt immer wieder auf 0 → Neustarts |
| **Besuche** | passt zu dem, was du siehst | 0, oder unrealistisch hoch |
| **Systemzustand** | grüner Haken | gelbe oder rote Meldung |

**Warum das nicht optional ist:** Ab März darf der Kasten nicht mehr geöffnet werden. Ein
Fehler, den du jetzt in fünf Minuten behebst, kostet dich sonst ein ganzes Jahr.

---

## 5.6 Wenn etwas gar nicht geht

| Problem | Ursache | Lösung |
|---|---|---|
| Board erscheint nicht als Port | Ladekabel ohne Datenadern | anderes USB-Kabel |
| „Kamera startet nicht“ | PSRAM steht auf „Disabled“ | Werkzeuge → PSRAM → **OPI PSRAM** |
| „Mikrofon startet nicht“ | ESP32-Paket älter als Version 3.x | Boardverwalter → `esp32` aktualisieren |
| Hochladen bricht ab | Board hängt | BOOT gedrückt halten, USB anstecken, loslassen |
| SD-Karte wird nicht erkannt | Karte größer als 32 GB, oder nicht FAT32 | kleinere Karte, FAT32 formatieren |
| Bild rauscht oder hat Streifen | Flachbandkabel zu lang | `XCLK_MHZ` von 20 auf 10 |
| Clip hat weniger Bilder als erwartet | Karte zu langsam | `BILD_QUALITAET` erhöhen (18 → 22) |
| Website nicht erreichbar | `.local`-Name geht nicht | IP-Adresse aus dem Seriellen Monitor nehmen |
| Akkuanzeige zeigt Unsinn | nicht kalibriert | Schritt 5 wiederholen |

**Und der allgemeine Rat:** Wenn nach einer Änderung etwas nicht mehr geht, mach genau diese
eine Änderung rückgängig. Nicht drei auf einmal.

---

→ Weiter mit [6. Website & Daten](06-website-und-daten.md)
