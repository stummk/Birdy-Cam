# 2. Stückliste — was du kaufen musst

> 🛒 **Zum Bestellen nimm besser [8. Bestellliste](08-bestellliste.md).** Dort ist alles
> nach Shop sortiert, mit Prüfpunkten und der richtigen Bestellreihenfolge. Diese Seite
> hier erklärt, **warum** genau dieses Teil und kein anderes.

> **Zu den Preisen:** Stand September 2026, Bruttopreise, deutsche Händler. Hardwarepreise
> schwanken — sieh die Zahlen als Budgetrahmen, nicht als Kassenzettel.
>
> **Zu den Links:** Wo ich die genaue Produktseite kenne, ist sie verlinkt. Wo es viele
> gleichwertige Produkte gibt (Kabel, Gehäuse, Kleinteile), verlinke ich absichtlich eine
> **Suchseite** — Direktlinks auf Zubehör veralten schneller als der Rest der Anleitung.

---

## 2.1 Die Teileliste

| # | Teil | Warum genau das | ~Preis |
|---|---|---|---|
| **E1** | **Seeed XIAO ESP32-S3 Sense** | Das Gehirn. Kamera-Anschluss, SD-Slot und Mikrofon sind schon drauf. ⚠️ Es muss die **„Sense“**-Version sein — die normale hat keinen Kameraanschluss. Möglichst **mit angelöteten Pins** kaufen | 25 € |
| **E2** | **Kameramodul 24-polig DVP, ohne IR-Filter** | Das Auge. „Night Vision“, „no IR filter“ oder „NoIR“ muss dabeistehen, sonst ist es nachts blind. Empfehlung: **OV2640 Night Vision**, weitwinklig (120–160°) | 10 € |
| **E3** | **microSD 32 GB, High Endurance** | Der Speicher. ⚠️ **Höchstens 32 GB** — mehr kann das Board nicht. „High Endurance“ oder „Dashcam“ ist für Dauervideo gebaut | 12 € |
| **E4** | FPC-Verlängerung, 24-polig, 0,5 mm, ~10 cm | Damit die Kamera in den Kasten kann und das Board nach außen. ⚠️ **Nicht länger als 15 cm**, sonst rauscht das Bild | 3 € |
| **E5** | **Waveshare Solar Power Manager** | Laderegler, MPPT und 5-V-Netzteil in einem — komplett lötfrei. Nimmt Panels von **6 bis 24 V**, lädt einen 3,7-V-Akku, gibt **5 V / 1 A** über eine USB-Buchse aus. Hat obendrein eine **USB-Notladebuchse** | 9 € |
| **E6** | **Solarpanel HPLive 15 W / 12 V** | Der Tank-Nachfüller. 15 W sind an Sonnentagen großzügig — genau darum geht es: An **trüben** Tagen zählt jedes Watt. ⚠️ Auf dem Aufkleber muss `Voc` **unter 24 V** stehen (bei 12-V-Panels üblich: 18–22 V) | ~30 € |
| **E7** | **LiPo-Akku 1S, 5000 mAh**, mit Schutzschaltung + JST-PH-2.0-Stecker | Überbrückt Nacht und Regentage. ⚠️ **Mit Schutzschaltung** ist Pflicht. Für doppelte Reserve: 10 000 mAh, +12 € | 14 € |
| **E8** | Spannungssensor-Modul 0–25 V | Damit die Website den Akkustand zeigt. Kommt meist im 5er-Pack | 2 € |
| **E8b** | **JST-PH-2.0-Pigtails**, je 1× mit Stecker und 1× mit Buchse | Damit Laderegler **und** Spannungssensor gleichzeitig am Akku hängen, ohne zu löten — verteilt wird in der Schraubklemme des Sensors. ⚠️ **Kein Y-Kabel suchen:** JST-**PH 2.0**-Splitter gibt es praktisch nicht, die „JST-Y-Kabel“ aus dem Modellbau sind der dickere RCY-Stecker und passen nicht. Gibt es im Set. [Wie es geht](03-schaltplan.md#38-der-spannungssensor--damit-du-den-akkustand-siehst) | 3 € |
| **E9** | **4 × IR-LED-Modul 940 nm** | Das unsichtbare Nachtlicht. ⚠️ **940 nm**, nicht 850 nm — letzteres glimmt für Menschen sichtbar rot. ⚠️ **Fertige Module**, keine nackten LEDs — der Vorwiderstand sitzt auf der Platine ([2.2 ④](#22-die-teile-bei-denen-man-aufpassen-muss)) | 7 € |
| **E10** | **MOSFET-Modul, Logic Level** (D4184 / AOD4184) | Der elektronische Lichtschalter für die LEDs. ⚠️ **Kein IRF520!** Der schaltet bei 3,3 V nicht durch. Es gibt zwei Bauformen — [welche du hast und was das ändert](03-schaltplan.md#zuerst-welche-bauform-hast-du-zähl-die-anschlüsse) | 4 € |
| **E11** | **IR-Lichtschranke** mit Digitalausgang, 3,3 V | Zählt Ein- und Ausflüge exakt. Optional, in der Firmware **ab Werk abgeschaltet** — aber das schönste Bauteil im Projekt, und jederzeit nachrüstbar ([1.3](01-ueberblick.md#13-die-lichtschranke--die-beste-idee-im-ganzen-plan)) | 3 € |
| **E12** | Dupont-Steckkabel-Set (F-F und F-M) | Alle Steckverbindungen. 10–20 cm Länge reicht | 5 € |
| **E13** | IP65-Gehäuse, ca. 120 × 80 × 50 mm | Die Elektronikbox außen am Kasten. Muss Board, Laderegler und Akku fassen | 11 € |
| **E14** | Kabelverschraubungen M12, 3 Stück | Für Panelkabel, Kamerakabel, IR-/Lichtschrankenkabel | 5 € |
| **E15** | Acrylglas 3 mm, ca. 10 × 10 cm | Schutzscheibe vor der Kameraöffnung. ⚠️ **Kein Glas** — Acryl lässt Infrarot durch, Fensterglas dämpft es | 6 € |
| **E16** | Silikagel-Beutel | Gegen Kondenswasser und beschlagene Linse. Jährlich tauschen | 5 € |
| **E17** | USB-Kabel A → C, ca. 30 cm | Verbindet Laderegler und Board. Oft schon im Haus | 5 € |
| **E18** | Klettband, Kabelbinder, Schrauben | Kleinteile. Klettband für den Akku — er soll tauschbar bleiben | 5 € |
| | **Summe Bauteile** | | **≈ 159 €** |

> **Realistisch an der Kasse: 195–210 €.** Der Unterschied kommt von Versandkosten über
> mehrere Shops und davon, dass es viele Kleinteile nur im Mehrfachpack gibt. Die genaue
> Aufstellung steht in [8.8](08-bestellliste.md#88-summen).

---

## 2.1b Variante B — das Teile-Delta

[4.10 Variante B](04-bauplan.md#410-variante-b--alles-im-kasten-kamera-hinter-dem-klardeckel)
baut die ganze Elektronik **in** den Kasten, hinter einen durchsichtigen Gehäusedeckel — von
außen führt nur noch das Panelkabel hinein. Wer so baut, bestellt anders:

| # | Änderung gegenüber [2.1](#21-die-teileliste) | Δ |
|---|---|---|
| **E4** | **entfällt** — die Kamera bleibt in der Box, es gibt kein Flachbandkabel nach draußen | −3 € |
| **E13** | → **Hammond `1555FL2GY` + Klardeckel `1554FCL`**, 120 × 90 × 45 mm. Polycarbonat, UV-stabil, mit Dichtung bis IP68. **Der Deckel wird separat bestellt** | +14 € |
| **E15** | **entfällt** — der Klardeckel *ist* das Objektivfenster | −6 € |
| **E17** | → dasselbe Kabel, aber **mit 90°-Winkelstecker und nur 10–15 cm**. Ein gerader Stecker braucht 4 cm Biegeradius, die hast du in der Box nicht | 0 € |
| **E19** *neu* | **Steuerkabel LiYY 6 × 0,14 mm², ~1 m.** IR-LEDs und Lichtschranke gehen durch **eine** Verschraubung — fünf Einzellitzen dichten darin nicht | +3 € |
| **E20** *neu* | **Reedschalter** (Glasröhrchen, Schließer). Liegt an `RST`/`GND`: Magnet von außen an die Kastenwand = Neustart, ohne zu öffnen | +1 € |
| **E21** *neu* | **PVC-Platte 2 mm + 4 × M3-Distanzhülse 12 mm.** Zwischenboden, auf dem der Laderegler über dem Akku sitzt | +3 € |
| **E22** *neu* | Moosgummi schwarz (Blendring ums Objektiv), Brett 10 mm + Leisten für die Zwischendecke | Restholz |
| | **Summe Bauteile** | **≈ 171 €** |

> ⚠️ **Der Akku wird zum kritischen Maß.** In die 45-mm-Box passt nur bis
> **95 × 58 × 12 mm**. „5000 mAh 1S“ gibt es in einem Dutzend Bauformen — steht das Maß
> nicht in der Artikelbeschreibung, nimm einen anderen Anbieter. Und die Ausbaustufe
> **10 000 mAh aus [2.5](#25-wenn-mehr-budget-da-ist) fällt weg**, die passt nicht mehr.

> ⚠️ **Zwei Werkzeuge mehr** als in [2.6](#26-werkzeug): eine **Stichsäge oder Laubsäge**
> für den 122 × 92-mm-Ausschnitt in der Zwischendecke, und ein **Lötkolben** für die zwei
> Punkte am Reedschalter.

> ⚠️ **Und eine Sache, die kein Bauteil ist:** In Variante B kommst du fünf Monate lang
> nicht mehr an den USB-Anschluss. Die Firmware in diesem Repo hat heute **kein OTA und
> keinen Watchdog** — das gehört eingebaut, *bevor* die Box zugeschraubt wird
> ([4.10.6](04-bauplan.md#4106-vor-dem-zuschrauben-drei-versicherungen)).

**Wo es die Box gibt:** Hammond führen in Deutschland unter anderem
[Reichelt](https://www.reichelt.com/de/en/shop/manufacturer/hammond%20manufacturing),
[TME](https://www.tme.eu/de/katalog/?search=1555FL2GY),
[Distrelec](https://www.distrelec.de/de/search?q=1555FL2GY) und RS. Datenblätter:
[Gehäuse 1555](https://www.hammfg.com/electronics/small-case/plastic/1555) ·
[Klardeckel 1554/1555LID](https://www.hammfg.com/electronics/small-case/plastic/1554-1555lid).
Der Klardeckel `1554FCL` passt auf die Baugrößen F, FL und G — nicht vergessen, ihn
**mitzubestellen**, sonst liegt ein grauer Deckel im Karton.

---

## 2.2 Die Teile, bei denen man aufpassen muss

Wenn du dir nur vier Dinge aus diesem Kapitel merkst, dann diese.

### ① Das Kameramodul muss **ohne** IR-Filter sein

Im Angebot muss stehen: **„no IR filter“**, **„night vision“**, **„NoIR“** oder
**„850nm/940nm“**. Steht dort nichts davon, ist ab Werk ein winziges Filterglas über dem
Sensor — und die Kamera sieht nachts absolut nichts, egal wie stark die IR-LEDs sind.

> **Bestell zwei Stück.** Hier geht am ehesten etwas kaputt, und aus Asien wartet man sonst
> mitten im Bau zwei bis vier Wochen. Und teste **vor** dem Einbau in den Kasten
> ([8.9](08-bestellliste.md#89-wareneingangs-prüfung)).

### ② Das MOSFET-Modul muss **Logic Level** sein

Auf dem schwarzen Chip muss **D4184** oder **AOD4184** stehen. Die optisch fast gleichen
Module mit `IRF520` brauchen mehr als 3,3 Volt zum Durchschalten — die IR-LEDs bleiben dann
dunkel oder glimmen nur.

Und es gibt **zwei Bauformen**, die beide „D4184-Modul“ heißen. Zähl die Anschlüsse:

- **Variante A „HW-532“**, winzig (23 × 17 mm): **2** Steueranschlüsse (`PWM`, `GND`) und
  **3** Schraubklemmen (`+`, `LOAD`, `−`), mit Optokoppler `PC817`. Das ist die, die man
  fast überall bekommt. Sie funktioniert an unseren 5 V meistens, aber nicht garantiert —
  warum, steht in [3.6](03-schaltplan.md#36-das-unsichtbare-nachtlicht--mosfet-und-ir-leds).
- **Variante B „XY-MOS“**, groß (ca. 50 × 25 mm): **3** Steuerstifte (`SIG`, `VCC`, `GND`)
  und **4** Schraubklemmen (`VIN+`, `VIN−`, `OUT+`, `OUT−`), ohne Optokoppler. An 5 V die
  sichere Wahl.

> 💡 **Am entspanntesten:** Beide bestellen. Zusammen unter 10 €, und beide kommen ohnehin
> meist im Mehrfachpack. Dann bleibt Schritt 4 garantiert kein Abend voller Ratespiele.

### ③ Das Panel muss unter 24 Volt Leerlaufspannung haben

Auf dem Aufkleber hinten am Panel steht eine Zeile `Voc` oder „Open Circuit Voltage“ oder
„Leerlaufspannung“. Dieser Wert muss **unter 24 V** liegen.

Bei einem 12-V-Panel wie dem HPLive stehen dort typisch **18 bis 22 Volt** — das passt
bequem. Nur wenn dort mehr steht (etwa bei einem 24-V-Panel), gehört ein anderes her.

> 💡 **Das ist der Vorteil des Waveshare-Ladereglers:** Er nimmt alles zwischen 6 und
> 24 Volt und rechnet selbst herunter. Du musst bei der Panelauswahl also nicht auf
> Zehntelvolt achten — nur nach oben ist eine Grenze.

### ④ Die IR-LEDs müssen **Module** sein

`E9` heißt **IR-LED-Modul**, und das ist wörtlich gemeint: eine kleine Platine, auf der der
**Vorwiderstand schon sitzt**. Nur deshalb kommt das Wort „Vorwiderstand“ in dieser
Anleitung sonst nirgends vor.

Nimmst du stattdessen **nackte 5-mm-LEDs** aus der Bastelkiste — die es für ein Zehntel des
Geldes gibt —, fehlt genau dieses Bauteil. Und eine LED begrenzt ihren Strom nicht selbst:
Direkt an 5 V fließt statt der geplanten 20 mA schnell ein **Ampere**. Das MOSFET-Modul
wird dann heiß und riecht verschmort, und die LEDs nehmen Schaden.

> ⚠️ **Das Tückische daran: Die LEDs leuchten dabei munter weiter** — heller sogar als
> sonst. Am Licht merkst du den Fehler also nicht. Du merkst ihn am warmen MOSFET.

Das ist **kein** Grund, keine nackten LEDs zu nehmen. Es sind vier Widerstände mehr, sie
kosten zusammen ein paar Cent, und in den 5-mm-Bohrungen des Deckels sitzen nackte LEDs
sogar bequemer als die Platinen. Die Rechnung und die lötfreie Verdrahtung stehen in
[Schaltplan 3.6](03-schaltplan.md#nackte-leds-statt-module-dann-brauchst-du-vier-vorwiderstände).

**Und noch eine Zahl, über die man stolpert: zwei Pins oder drei?** Am einfachsten sind
Module mit **zwei** Anschlüssen (`+` und `−`) — die kommen genau so an den MOSFET, wie es
in [3.6](03-schaltplan.md#36-das-unsichtbare-nachtlicht--mosfet-und-ir-leds) steht. Module
mit **drei** Pins gehen aber genauso; einer der drei bleibt dann frei. Welcher, misst du in
zwei Minuten aus:
[3.6 „Dein LED-Modul hat drei Pins statt zwei“](03-schaltplan.md#dein-led-modul-hat-drei-pins-statt-zwei).

---

## 2.3 Warum diese Kombination aus Laderegler und Panel

Das ist die eine echte Entwurfsentscheidung in der Stromversorgung, und sie ist es wert,
kurz erklärt zu werden.

| | **Waveshare Solar Power Manager** |
|---|---|
| Panel-Eingang | **6 – 24 V** — Schraubklemme oder Hohlstecker |
| Zusätzlicher Eingang | **Micro-USB** — für Powerbank oder Netzteil |
| Akku | 3,7-V-Lithium, **JST-PH-2.0-Buchse** |
| Ausgang | **5 V / 1 A**, USB-A-Buchse und Steckstifte |
| MPPT | ✅ mit Schiebeschalter für 6/9/12/18/24 V |
| Schutz | Überladung, Tiefentladung, Verpolung, Überhitzung, Überstrom |
| Anzeige | Lämpchen für Laden, Voll, Verpolung und Akkustand |
| Eigenverbrauch | unter 2 mA |
| Preis | ~9 € |

**Drei Gründe, warum das gut zu diesem Projekt passt:**

1. **Lötfrei.** Schraubklemme fürs Panel, Steckbuchse für den Akku, USB-Buchse für den
   Bastelcomputer. Kein einziger Lötpunkt.
2. **Der breite Eingang macht die Panelwahl entspannt.** Jedes gängige 12-V-Gartenpanel
   passt. Man muss nicht nach exotischen 5-V-Panels suchen und deren Leerlaufspannung
   ausmessen.
3. **Die Lämpchen ersetzen ein Messgerät.** Verpolt angeschlossen? Rotes Lämpchen. Lädt
   gerade? Gelbes Lämpchen. Akku voll? Grünes. Für jemanden ohne Elektronikerfahrung ist
   das mehr wert als jedes Datenblatt.

> **Wie viel Ladestrom das Modul höchstens liefert, gibt Waveshare nicht an.** Der verbaute
> Ladechip (CN3791) ist für bis zu 2 A ausgelegt. Praktisch heißt das: Der Regler nimmt
> sich, was er braucht, und mehr Panel schadet nie — es wird dann einfach nicht ausgereizt.
> Deshalb ist ein 15-W-Panel eine gute Wahl und kein Overkill: Es erreicht die volle
> Ladeleistung **auch dann noch, wenn nur ein Drittel Sonne da ist.**

---

## 2.4 Wenn es billiger sein soll

| Weglassen oder ersetzen | Ersparnis | Was es kostet |
|---|---|---|
| Nur ein Kameramodul statt zwei | 10 € | Bei einem Defekt 2–4 Wochen Wartezeit mitten im Bau |
| Gehäuse: Vorratsdose + Silikon | 11 € | Für einen Sommer okay, über den Winter nicht |
| Acrylglas, Silikagel, Klettband, USB-Kabel aus dem Haus | 20 € | nichts, wenn vorhanden |
| Lichtschranke weglassen | 3 € | Die Statistik wird geschätzt statt gemessen |
| 10-W-Panel statt 15 W | ~8 € | Reicht ab April. Im März und bei Bewölkung wird es knapp |

**Wo man **nicht** sparen sollte:** Akku mit Schutzschaltung, High-Endurance-Speicherkarte
und das Kameramodul ohne IR-Filter. Diese drei erfüllen die eigentlichen Anforderungen.

## 2.5 Wenn mehr Budget da ist

| Aufpreis | Was es bringt |
|---|---|
| **+12 €** Akku 10 000 mAh statt 5 000 | Doppelte Reserve. Der lohnendste Aufpreis der Liste — **außer in [Variante B](#21b-variante-b--das-teile-delta)**, dort passt der dickere Pack nicht in die Box |
| +5 € DS18B20 Temperaturfühler (wasserdicht) | Nesttemperatur auf der Website. Am Verlauf erkennt man, ob gebrütet wird — der Vogel *heizt*. Für 5 € das lehrreichste Extra |
| +3 € zwei IR-LEDs mehr (6 statt 4) | Helleres Nachtbild, falls 4 nicht reichen. Für MOSFET und 5-V-Zweig kein Problem — [Rechnung](03-schaltplan.md#schafft-das-modul-überhaupt-vier-leds) |
| +20 € Multimeter | Macht das Kalibrieren exakt statt geschätzt — und bleibt für alle künftigen Projekte nützlich |

---

## 2.6 Werkzeug

| Werkzeug | Wofür | Muss es sein? |
|---|---|---|
| Kleiner Schlitzschraubendreher | Schraubklemmen am Laderegler und MOSFET | ✅ ja |
| Abisolierzange oder Seitenschneider | Panelkabel abisolieren | ✅ ja |
| Akkuschrauber + Holzbohrer 3, 5, 6, 12 mm | Löcher im Deckel und in der Wand | ✅ ja |
| Heißklebepistole | Kamera, LEDs, Lichtschranke fixieren und abdichten | ✅ ja |
| Laubsäge oder Cutter | Acrylglas zuschneiden | ✅ ja |
| Handy | IR-Test, Livebild beim Justieren | ✅ ja ⭐ |
| Multimeter | Pigtails auf Verpolung prüfen ([3.8](03-schaltplan.md#durchtesten-mit-dem-multimeter--vier-messungen)), Akku kalibrieren, Panel prüfen | ✅ ja, wenn du den Spannungssensor einbaust |
| Lötkolben | **nur** falls das Board ohne angelötete Pins kommt | meist nein |

---

→ Weiter mit [3. Schaltplan](03-schaltplan.md) · Zum Bestellen: [8. Bestellliste](08-bestellliste.md)
