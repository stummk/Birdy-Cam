# 1. Machbarkeit — was geht, was nicht, und warum

Kurzfassung: **Das Projekt ist machbar, in zwei Ausführungen.** Mit einem Raspberry Pi
Zero 2 W für ~269 € gibt es echtes Full-HD-Video und USB-SSD statt SD-Karte. Mit einem
ESP32-S3 für ~155 € gibt es dieselben Funktionen bei schlechterer Videoqualität.

Vogelgesang-Erkennung und Artenbestimmung sind gestrichen — auf Wunsch, und aus guten
Gründen ([1.7](#17-was-bewusst-fehlt-und-warum)).

---

## 1.1 Die Ausgangslage

Ursprünglich standen drei Vorgaben im Raum, die sich widersprachen: Solar, 150 €, alle
Funktionen. Nach zwei Runden Präzisierung sieht die Aufgabe so aus:

| Vorgabe | Status |
|---|---|
| Autonom über Solar und Akku | ✅ fest |
| Nachtsicht, Livestream, Bewegungsclips, Website, stabiler Speicher | ✅ fest |
| **Full HD** | ✅ neu dazugekommen |
| 3 Tage Reserve bei Dauerregen | ✅ festgelegt |
| Vogelgesang, Artenerkennung | ⬜ gestrichen |
| Budget | 150 € **oder** 269 €, je nach Variante |

---

## 1.2 Die Lichtschranke — die beste Idee im ganzen Plan

Statt Bewegung nur im Bild zu suchen, kommt ein Infrarot-Strahl quer durch das
Einflugloch. Fliegt ein Vogel durch, bricht er ihn.

```
       Einflugloch von oben gesehen:

         ┌─────────────────┐
         │                 │
    IR ──┼───────●─────────┼── Empfänger
   Sender│    Strahl       │
         │                 │
         └─────────────────┘
              Ø 28-32 mm
```

| | Bilderkennung | Lichtschranke |
|---|---|---|
| Sonnenfleck wandert durch den Kasten | ❌ Fehlalarm | ✅ ignoriert |
| Blattschatten flackert | ❌ Fehlalarm | ✅ ignoriert |
| Vogel fliegt ein | ✅ erkannt | ✅ erkannt, **exakt** |
| Ein- oder Ausflug? | ❌ nicht unterscheidbar | ✅ zählbar |
| Aufenthaltsdauer | ❌ Schätzung | ✅ echte Messung |
| Stromverbrauch | 0,4 W | ~0,0005 W |
| Preis | 0 € | 6 € |

**Beide werden benutzt**, weil sie verschiedene Dinge sehen: Die Lichtschranke merkt das
Kommen und Gehen, die Bilderkennung merkt Bewegung *innerhalb* des Kastens (Küken füttern,
Nest umbauen). Die Statistik zählt aber nur die Lichtschranke — deshalb sind die Zahlen auf
der Website belastbar.

---

## 1.4 Full HD — hier trennen sich die Varianten

Das ist die Anforderung, die die Variantenwahl entscheidet. Es geht nicht um die Zahl der
Pixel, sondern um die **Kompression**.

| | Pi Zero 2 W | ESP32-S3 |
|---|---|---|
| Verfahren | **H.264** (Hardware-Encoder im Chip) | **MJPEG** (Einzelbilder) |
| 1080p Bildrate | **15–30 Bilder/s** | ~6 Bilder/s |
| Datenrate | ~0,5 MB/s | ~0,7 MB/s |
| 10-Sekunden-Clip | ~5 MB **bei 15 fps** | ~7 MB **bei nur 6 fps** |
| Im Browser abspielbar | ✅ **MP4, ein Klick** | ❌ Download, dann VLC |
| Aufnahmen pro Tag (60 Clips) | ~300 MB | ~430 MB |

**Warum H.264 besser ist:** MJPEG speichert jedes Bild einzeln und vollständig — jedes Mal
ein komplettes Foto. H.264 speichert nur, *was sich geändert hat*. In einem Nistkasten
ändert sich fast nichts außer dem Vogel, also ist H.264 hier besonders effizient. Der
Zero 2 W hat den Encoder als **Hardware** im Chip; das kostet kaum Rechenzeit.

**Der Unterschied zeigt sich nicht in der Dateigröße, sondern in der Bildrate.** Beide
Verfahren landen bei etwa 0,5–0,7 MB/s — nur liefert H.264 dafür 15 bis 30 Bilder pro
Sekunde und MJPEG nur 6. Anders gesagt: **Für dieselbe Flüssigkeit bräuchte MJPEG etwa das
Dreifache an Platz** — und genau das kann die SD-Karte am ESP32 nicht schreiben. Deshalb
bleibt es bei 6 Bildern/s.

Zwei Engpässe begrenzen Variante B:

| Engpass | Grenze | Folge |
|---|---|---|
| ESP32-S3 liest den Sensor aus | ~6 Bilder/s bei 1080p | ruckeliges Video |
| SD-Karte über SPI | ~1,2 MB/s | bei zu guter JPEG-Qualität fallen Bilder aus dem Clip |

Praktisch heißt das: Beim Pi klickt man auf der Website auf einen Clip und er läuft im
Browser. Beim ESP32 lädt man ~7 MB herunter und öffnet sie im VLC Player.

> **Fairness gegenüber Variante B:** 1080p bei 6 Bildern/s ist nicht wertlos — man sieht
> alles, was passiert, und jedes Einzelbild ist scharfes Full HD. Es ruckelt nur. Wer
> Flüssigkeit höher gewichtet als die Pixelzahl, stellt eine Zeile auf `FRAMESIZE_SVGA`
> (800×600) → ~15 Bilder/s. Wer 150 € nicht überschreiten will, verliert Komfort, keine
> Funktion.

---

## 1.5 Nachtsicht

Eine normale Kamera sieht nachts nichts. Zwei Dinge sind nötig:

1. **Kamera ohne IR-Sperrfilter** („NoIR"). Jede normale Kamera hat ein winziges Filterglas
   über dem Sensor, das Infrarot wegfiltert. Genau das macht sie nachts blind, selbst mit
   IR-Lampe.
2. **Eigene IR-Beleuchtung.** Das ist **keine Wärmebildkamera** — ohne IR-Lampe sieht sie
   in absoluter Dunkelheit exakt nichts.

**Beim Pi ist das einfacher als beim ESP32:** Fertige NoIR-Module gibt es ab ~15 € von der
Stange (OV5647, 1080p), das Raspberry-Pi-NoIR-Modul ab ~14,90 €. Kein Filterausbau, kein
Basteln. Beim ESP32 musste dafür ein Modul aus der ESP32-CAM-Welt zweckentfremdet werden.

### 940 nm oder 850 nm — der Kompromiss

Vögel sehen bis etwa 700 nm. **Beide IR-Varianten liegen darüber und sind für den Vogel
unsichtbar** — die Wahl ist also keine Tierschutzfrage, sondern eine technische.

| | 940 nm | 850 nm |
|---|---|---|
| Für Menschen sichtbar? | nein | schwaches rotes Glimmen, wenn man direkt hineinsieht |
| Für Vögel sichtbar? | **nein** | **nein** |
| Empfindlichkeit des Kamerasensors | ~50–60 % | **100 %** (Bezugswert) |
| Nachtbild bei gleicher LED-Leistung | dunkler | **heller** |

**Silizium-Sensoren sehen 940 nm deutlich schlechter als 850 nm.** Dieselbe LED-Leistung
ergibt also bei 940 nm ein merklich dunkleres Bild. Das ist der Preis für die
Unsichtbarkeit.

**Der Plan nimmt 940 nm**, weil genug Reserve da ist: Die LEDs laufen per PWM auf nur 30 %
(`IR_HELLIGKEIT = 0.30`). Ist das Nachtbild zu dunkel, dreht man einfach hoch — bis 100 %
gibt es Luft, also mehr als dreifache Helligkeit.

**Reicht das nicht, gibt es drei Wege** (in dieser Reihenfolge probieren):

1. `IR_HELLIGKEIT` erhöhen — kostet nur Strom, kein Bauteil
2. Mehr LEDs (6 statt 4) oder stärkere LEDs
3. **Auf 850 nm wechseln** — der Vogel merkt es nicht, nur ein Mensch, der nachts direkt
   ins Einflugloch schaut, sieht ein schwaches rotes Glimmen. Technisch die beste
   Bildqualität.

IR-Lampen können trotzdem störend sein — nicht durch Licht, sondern durch **Wärme**.
Deshalb: nur 4 kleine LEDs, per PWM auf ~30 % gedimmt, nur bei Dunkelheit an, im Deckel
ca. 15 cm über dem Nestboden.

### Was man nachts realistisch sieht

Graustufen, etwas körnig, Vögel und Bewegung klar erkennbar. Farbige Federn nachts zu
bestimmen ist **physikalisch unmöglich** — im Infrarot gibt es keine Farbe.

---

## 1.6 Speicher — das SD-Problem ist bei Variante A vollständig gelöst

Die Sorge im Auftrag war berechtigt, und bei Variante A gibt es dafür eine saubere Lösung.

### Der Pi Zero 2 W kann von USB-SSD booten

Das war die entscheidende Recherche-Frage, und die Antwort ist **ja** — mit einem
Zwischenschritt:

1. Einmal von SD-Karte booten, in `/boot/firmware/config.txt` die Zeile
   `program_usb_boot_mode=1` eintragen, neu starten.
2. Damit wird ein **OTP-Bit** gesetzt (One Time Programmable — dauerhaft, nicht
   umkehrbar).
3. Ab jetzt bootet der Pi von USB. **Die SD-Karte wird herausgezogen und weggelegt.**

Ergebnis: **Im Dauerbetrieb steckt keine SD-Karte im Gerät.** Das Ausfallrisiko, um das es
in der Anforderung ging, existiert nicht mehr.

### Die Rechnung

| Posten | Menge/Tag |
|---|---|
| Bewegungsclips (~60 × 10 s H.264 1080p ≈ 5 MB) | ~300 MB |
| Vogelfotos (~200 × 250 KB) | ~50 MB |
| Datenbank, Logs, System | ~50 MB |
| **Summe** | **≈ 0,4 GB/Tag** |

Das sind ~145 GB pro Jahr geschriebene Daten.

| Medium | Aushalten (TBW) | Lebensdauer |
|---|---|---|
| Normale microSD | ~5 TB (optimistisch) | ~2 Jahre, Ausfall ohne Vorwarnung |
| High-Endurance microSD | ~20 TB | ~10 Jahre |
| **USB-SSD 240 GB** | **~80–150 TBW** | **> 50 Jahre** |

Dazu drei Maßnahmen in der Software:

1. **`log2ram`** — Systemlogs sammeln sich im RAM und werden nur einmal täglich
   geschrieben.
2. **Ringspeicher mit harten Obergrenzen** — die Platte kann nicht volllaufen.
3. **SQLite im WAL-Modus** — übersteht einen Stromausfall ohne kaputte Datenbank.

### ⚠️ Ein Risiko bei der SSD, das man kennen muss

Der Pi Zero 2 W hat **einen einzigen** Micro-USB-Datenanschluss, und der kann nur begrenzt
Strom liefern. Eine SSD zieht im Schreibbetrieb bis zu 2,5 W (~500 mA). Das kann zu
Instabilität führen — der Pi startet neu, wenn die Spannung einbricht.

Gegenmaßnahmen, in dieser Reihenfolge:

1. **Sparsame SSD** wählen (2,5-Zoll-SATA im USB-Gehäuse ist sparsamer als NVMe).
2. **Kräftige 5-V-Versorgung** — der DC-DC-Wandler muss 2,5 A können, nicht 1 A.
3. Falls es weiterhin klemmt: **Y-Kabel** mit separater Stromeinspeisung für die SSD (~6 €).

Das ist der Punkt, an dem Variante A am ehesten Nacharbeit braucht.

### Bei Variante B (ESP32)

Dort gibt es keine SSD-Option — ein ESP32 kann keine USB-Massenspeicher betreiben. Dafür
ist das Problem kleiner, als es klingt: Ein ESP32 hat **kein Betriebssystem auf der Karte**
— keine Logs, kein Swap, kein Journal. Die Karte sieht nur große, zusammenhängende
Schreibvorgänge. Mit einer High-Endurance-Karte reicht das für viele Jahre. Details in
[8. Variante ESP32](08-variante-esp32.md#speicher).

---

## 1.7 Was bewusst fehlt — und warum

### Vogelgesang mit Artnamen

Gestrichen. Auch technisch die richtige Entscheidung:

BirdNET vom Cornell Lab ist ein neuronales Netz mit über 6000 Arten; das Modell ist
mehrere Dutzend Megabyte groß. **BirdNET-Go** nennt in seinen Hardware-Empfehlungen einen
**Raspberry Pi 4B mit 2 GB als Minimum** und führt Pi 3 und Pi Zero als abgekündigt
(„the codebase has outgrown these platforms"). Ein Zero 2 W ist Pi-3-Klasse — also
ausdrücklich außen vor.

> **Nebenbei:** Das früher populäre *BirdNET-Pi* wurde vom Hauptentwickler im August 2025
> eingestellt. Wer später Gesangsbestimmung will, nimmt BirdNET-Go auf einem Pi 4/5 **am
> Stromkabel**, aufgestellt am Hausfenster. Ein Mikrofon hört ohnehin den ganzen Garten und
> nicht den Kasten — es muss also gar nicht am Nistkasten hängen. Das ist ein eigenes,
> gutes zweites Projekt.

*Die Audio-Funktion für Variante B ist übrigens fertig programmiert und liegt im Code —
sie ist nur abgeschaltet. Wer will, stellt `AUDIO_AN` auf `true`. Sie erkennt und speichert
Gesang, sagt aber nicht, welcher Vogel singt.*

### Artenerkennung im Bild

Gestrichen — und selbst mit Aufwand wäre sie im Nistkasten wenig wert:

1. **In einem Nistkasten brütet genau ein Paar.** Sechs Wochen dieselbe Blaumeise. Die
   Statistik „welche Arten besuchen den Kasten" hätte einen Eintrag.
2. **Nachts ist alles grau.** Klassifikationsmodelle sind auf Farbfotos trainiert.
3. **Der Vogel ist zu nah und halb verdeckt** — man sieht einen Rücken, einen Flügel, aus
   15 cm Entfernung.

**Was der Plan stattdessen liefert:** Anflüge pro Stunde, Aufenthaltsdauer, erster und
letzter Anflug, Verlauf über die Brutzeit. Die Fütterfrequenz steigt dramatisch, wenn die
Küken schlüpfen — **das ist die Kurve, an der man den Schlupftag sieht.** Kein Artenmodell
liefert etwas Vergleichbares.

Wer echte Artenerkennung will, braucht eine **zweite Kamera am Futterhaus**: Farbe,
Tageslicht, Seitenansicht, 20+ Arten. Auch das ein natürlicher Ausbau — siehe
[7.7](07-wartung-und-fehlersuche.md#77-wenn-das-projekt-größer-werden-soll).

---

## 1.8 Rechnet die Stromversorgung? — Ja, für März bis Juli

Die Brutsaison liegt in der **guten Sonnenhälfte des Jahres**. Das ist der Grund, warum
Solar hier überhaupt funktioniert.

### Verbrauch Variante A

| Betriebsart | Leistung | Dauer/Tag | Energie |
|---|---|---|---|
| Tag: Linux + Kamera + Encoder + SSD | 2,2 W | 14 h | 30,8 Wh |
| Nacht: Linux + Lichtschranke, Kamera aus | 1,0 W | 10 h | 10,0 Wh |
| IR-LEDs (nur bei Dunkelheit & Auslösung) | 0,4 W | ~2 h | 0,8 Wh |
| **Summe** | | | **≈ 42 Wh/Tag** |

### Ertrag

Als Faustregel für Mitteleuropa liefert ein unverschattetes Modul pro Tag zwischen dem
**0,2-fachen** (trüber, kurzer Wintertag) und dem **7-fachen** (klarer, langer Sommertag)
seiner Nennleistung in Wattstunden.

Für ein **30-Wp-Panel**:

| Tag im Frühling | Faktor | Ertrag | Bilanz gegen 42 Wh |
|---|---|---|---|
| trüb, Dauerregen | ~0,5× | 15 Wh | **−27 Wh** → Akku |
| bedeckt | ~1,5× | 45 Wh | +3 Wh |
| durchwachsen | ~2,5× | 75 Wh | +33 Wh |
| sonnig im Mai | ~4,5× | 135 Wh | +93 Wh |

### Der Akku überbrückt die Regentage

LiFePO4 12 V / 12 Ah = 154 Wh nominal, davon nutzbar ~123 Wh.
**123 Wh ÷ 42 Wh/Tag = 2,9 Tage ohne jede Sonne** — genau die gewünschten 3 Tage.

Nach einer Regenwoche muss man nachhelfen. Dafür ist im Bauplan eine **USB-Notlade-Buchse**
vorgesehen: Powerbank anstecken, fertig.

### Warum LiFePO4 und nicht Li-Ion

| | Li-Ion / LiPo | **LiFePO4** |
|---|---|---|
| Ladezyklen | 500–1000 | **2000–5000** |
| Hitzeverträglichkeit | mäßig | **gut** |
| Brandverhalten | kritisch | **deutlich harmloser** |
| Preis pro Wh | günstiger | teurer |

In einem Gartengehäuse, das im Sommer heiß wird und viele Jahre halten soll, ist LiFePO4
die richtige Wahl.

### Drei Dinge, die das kaputt machen können

1. **Panel im Schatten.** Ein Nistkasten hängt gern schattig — das Panel gehört deshalb
   **nicht an den Kasten**, sondern per Kabel an die sonnigste erreichbare Stelle. Siehe
   [Bauplan 4.6](04-bauplan.md#46-solarpanel-montieren).
2. **Laden bei Frost.** Auch LiFePO4 darf **unter 0 °C nicht geladen werden**. Im März gibt
   es Nachtfrost. Auswege: Saison erst im April starten, oder Laderegler mit
   Temperaturfühler (NTC).
3. **Hitze.** Ein Akku über 45 °C altert schnell. Die Elektronikbox sitzt auf der
   **Nordseite** im Schatten.

### Wie der Pi wieder aufwacht, wenn der Akku leer war

Das ist ein Detail, das leicht übersehen wird: Ein Pi, der sich selbst herunterfährt,
startet **nicht** von allein wieder. Die Lösung steckt im Laderegler:

```
   Akku sinkt auf 11,0 V
        → Software fährt den Pi sauber herunter (Dateisystem bleibt intakt)
   Akku sinkt weiter auf ~10,5 V
        → Laderegler trennt den Last-Ausgang (Tiefentladeschutz)
   Sonne kommt, Akku lädt auf ~12,5 V
        → Laderegler schaltet die Last wieder ein
        → Pi bekommt Strom und bootet von selbst
```

Deshalb steht in der Stückliste ausdrücklich ein Laderegler **mit Last-Ausgang und
Tiefentladeschutz** — nicht der billigste ohne.

---

## 1.9 Rechtliches und Tierschutz

Nistkästen mit Kamera sind in Deutschland erlaubt, aber **§ 44 BNatSchG** schützt
europäische Vogelarten vor *erheblicher Störung* während der Brut- und Aufzuchtzeit; Brut-
und Ruhestätten dürfen nicht beschädigt werden.

Drei praktische Konsequenzen:

1. **Einbau nur zwischen September und Februar.** Die meisten heimischen Singvögel
   beginnen im März. Ist der Kasten belegt, wird er **nicht** geöffnet — auch nicht „kurz
   zum Kabel nachziehen". Wer im April merkt, dass die Kamera schief hängt, muss bis zum
   Herbst warten. Deshalb: **vor dem Einbau alles auf dem Tisch fertig testen.**
2. **Kein sichtbares Licht im Kasten.** Nur 940-nm-IR und der IR-Strahl der Lichtschranke.
   Der Pi hat eine grüne Aktivitäts-LED — die wird per `config.txt` abgeschaltet, sitzt
   aber ohnehin in der Box außerhalb.
3. **Kein Lärm, keine Wärme im Kasten.** Im Kasten liegen nur Kamera, IR-LEDs,
   Lichtschranke und Kabel. Pi, Akku und Laderegler sitzen in der Box **außen**.

---

## 1.10 Wo es realistisch klemmen wird

| Risiko | Wahrscheinlich | Gegenmaßnahme im Plan |
|---|---|---|
| **SSD überlastet den USB-Port des Zero** | **hoch** | Sparsame SSD, 2,5-A-Wandler, notfalls Y-Kabel ([1.6](#️-ein-risiko-bei-der-ssd-das-man-kennen-muss)) |
| USB-Boot funktioniert nicht beim ersten Versuch | mittel | Tutorial-Schritt 2 macht nur das, mit Rückfallebene „Daten auf SSD, Boot von SD" |
| Software läuft beim ersten Mal nicht durch | **hoch** | Tutorial in 8 einzeln testbare Schritte zerlegt |
| WLAN reicht nicht bis zum Kasten | **hoch** | Vor dem Bau mit dem Handy am Kastenplatz messen. Nachrüstlösung: Repeater |
| Akku leer nach Regenwoche | mittel | USB-Notlade-Buchse, Powerbank anstecken |
| Lichtschranke löst nicht aus / immer aus | mittel | Justierbar, Tutorial-Schritt 7 mit Live-Anzeige |
| Beschlagene Linse | mittel | Silikagel in der Box, Acrylscheibe, Kamera nicht an der Außenwand |
| Spinnennetz vor der Linse 🕷️ | **hoch** | Klassiker. Im Herbst putzen (Wartungsplan) |
| Falsche Bewegungsalarme (Sonnenflecken) | mittel | **Lichtschranke zählt, Bild löst nur aus** — Statistik bleibt sauber |
| CSI-Flachbandkabel bricht | mittel | Zugentlastung, nicht knicken, Ersatzkabel auf Lager |

---

## 1.11 Fazit

**Machbar, und mit Full HD sinnvoll auf dem Raspberry Pi.** Der Hardware-H.264-Encoder ist
das Argument: Er macht aus einer Minute Video 5 MB statt 40 MB und liefert Clips, die man
im Browser anklicken kann. Zusammen mit USB-SSD-Boot ist Variante A technisch klar die
bessere Lösung — sie kostet 114 € mehr.

Die beiden gestrichenen Funktionen fehlen dem Projekt nicht. Was der Nistkasten wirklich
erzählen kann — wann geflogen wird, wie oft, wie lange, und wann die Küken geschlüpft sind
— liefert der Plan vollständig, und die Lichtschranke macht diese Zahlen sogar
belastbarer, als es Bilderkennung könnte.

---

### Quellen

- [Raspberry Pi Zero 2 W: USB-Massenspeicher-Boot (Raspberry Pi Forums)](https://forums.raspberrypi.com/viewtopic.php?t=353127)
- [Raspberry Pi Hardware-Dokumentation](https://www.raspberrypi.com/documentation/computers/raspberry-pi.html)
- [H.264-Hardware-Encoding auf dem Pi Zero 2 W (Forums)](https://forums.raspberrypi.com/viewtopic.php?t=373660)
- [Video-Monitoring mit den Hardware-Encodern des Pi Zero 2 W (ScienceDirect)](https://www.sciencedirect.com/science/article/pii/S2352711025002419)
- [picamera2 — CircularOutput2 (Quellcode)](https://github.com/raspberrypi/picamera2/blob/main/picamera2/outputs/circularoutput2.py)
- [picamera2 — Bewegungserkennung mit lores-Stream (Beispiel)](https://github.com/raspberrypi/picamera2/blob/main/examples/capture_motion.py)
- [Raspberry Pi Zero 2 W Stromverbrauch (CNX Software)](https://www.cnx-software.com/2021/12/09/raspberry-pi-zero-2-w-power-consumption/)
- [Raspberry Pi Stromverbrauch, alle Modelle 2026 (raspberry.tips)](https://raspberry.tips/en/faq/raspberry-pi-power-consumption-update-2026-all-models-compared-2)
- [USB-SSD Stromverbrauch (mpr projects)](https://mpr-projects.com/index.php/2023/12/19/rpi-nas-extras-power-consumption/)
- [Tagesertrag von Solarmodulen, Faustregel (Öko-Energie)](https://www.oeko-energie.de/shop1/de/Solarstrom/Solarmodule/Solarmodul-Wissen/Leistung--bzw--Ertrag-eines-Solarmodules/)
- [BirdNET-Go — Hardware-Empfehlungen](https://github.com/tphakala/birdnet-go/wiki/hardware)
- [XIAO ESP32S3 Sense — Spezifikation (Seeed Wiki)](https://wiki.seeedstudio.com/xiao_esp32s3_getting_started/)

→ Weiter mit [2. Stückliste](02-stueckliste.md)
