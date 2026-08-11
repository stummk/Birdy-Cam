# 2. Stückliste & Kauflinks

> **Zu den Preisen:** Stand August 2026, Bruttopreise, deutsche Händler. Hardwarepreise
> schwanken — betrachte die Zahlen als Budgetrahmen, nicht als Kassenzettel.
>
> **Zu den Links:** Wo ich die exakte Produktseite kenne, ist sie direkt verlinkt. Wo
> mehrere gleichwertige Produkte existieren (Kabel, Gehäuse, Kleinteile), verlinke ich
> bewusst eine **Suchseite** — Direktlinks auf Zubehör veralten schneller als der Rest des
> Plans, und ein totes Link ist schlimmer als eine Suche.

> ⚠️ **Korrektur gegenüber meiner ersten Schätzung:** Ich hatte Variante A mit „≈ 269 €"
> angegeben. Nach Aufstellung jeder einzelnen Position sind es **≈ 295 €** — ich hatte
> Kleinteile zu optimistisch gerechnet (Sicherungen, OTG-Adapter, Strommesser, größeres
> Gehäuse, vier Kabelverschraubungen statt zwei). Eine Sparliste in
> [2.4](#24-wenn-es-billiger-sein-muss) bringt es auf **≈ 262 €**. Die Zahl im Kopf sollte
> **~300 €** sein, nicht 269 €.

---

## VARIANTE A — Raspberry Pi Zero 2 W ⭐

### 2.1 Gruppe A — Rechner, Kamera, Speicher

| # | Teil | Warum genau das | ~Preis | Link |
|---|---|---|---|---|
| A1 | **Raspberry Pi Zero 2 WH** | ⭐ Die Version **mit H** hat die Stiftleiste **schon angelötet** — genau das, was „möglichst lötfrei" bedeutet. Ohne H müsste man 40 Pins löten. | 22 € | [BerryBase](https://www.berrybase.de/en/raspberry-pi-zero-2-wh) · [Welectron](https://www.welectron.com/Raspberry-Pi-Zero-2-W_1) |
| A2 | **NoIR-Kamera 1080p** (OV5647, 5 MP) | Ohne IR-Filter → Nachtsicht. 1080p30. Weitwinkel nehmen, wenn wählbar — der Kasten ist eng. | 16 € | [Roboter-Bausatz](https://www.roboter-bausatz.de/p/kamera-mit-nachtsicht-1080p-5mp-ov5647-fuer-raspberry-pi) · [Geizhals: Pi NoIR](https://geizhals.de/raspberry-pi-noir-kameramodul-a1135965.html) |
| A3 | **CSI-Kabel für Zero** (22-pol 0,5 mm ↔ 15-pol 1 mm), 30 cm | ⚠️ **Zwingend nötig.** Der Zero hat eine schmalere Buchse als alle anderen Pis. Das Kabel in der Kamerapackung passt **nicht**. | 5 € | [BerryBase Suche](https://www.berrybase.de/search?sSearch=CSI+Kabel+Raspberry+Pi+Zero) |
| A4 | **USB-SSD 240 GB** (2,5-Zoll SATA im USB-Gehäuse) | Ersetzt die SD-Karte vollständig. **2,5-Zoll-SATA bewusst statt NVMe** — sparsamer, und der Zero kann nur USB 2.0. | 32 € | [Amazon Suche](https://www.amazon.de/s?k=externe+SSD+240GB+USB+2.5+Zoll) |
| A5 | Micro-USB-OTG-Adapter | Der Zero hat nur Micro-USB. Verbindet die SSD. | 5 € | [Amazon Suche](https://www.amazon.de/s?k=Micro+USB+OTG+Adapter) |
| A6 | microSD 16 GB | **Nur** zum Einrichten (OTP-Bit setzen). Wird danach herausgezogen und weggelegt. Billigste reicht. | 6 € | [Amazon Suche](https://www.amazon.de/s?k=microSD+16GB) |
| A7 | Ersatz-CSI-Kabel | Flachbandkabel brechen beim Basteln. 5 € Versicherung. | 5 € | s. A3 |
| | **Summe Gruppe A** | | **91 €** | |

### 2.2 Gruppe B — Solar & Akku

| # | Teil | Warum genau das | ~Preis | Link |
|---|---|---|---|---|
| B1 | **Solarpanel 30 W / 12 V** | 30 Wp gegen 42 Wh/Tag Bedarf. Klingt groß und ist an Sonnentagen überdimensioniert — genau das ist der Punkt: An **trüben** Tagen zählt jedes Watt. Rechnung in [1.8](01-machbarkeit.md#18-rechnet-die-stromversorgung--ja-für-märz-bis-juli). | 35 € | [Amazon Suche](https://www.amazon.de/s?k=Solarpanel+30W+12V+monokristallin) |
| B2 | **Laderegler mit Last-Ausgang & LiFePO4-Profil** | ⚠️ Drei Eigenschaften sind Pflicht: **LiFePO4-fähig**, **Last-Ausgang**, **Tiefentladeschutz mit Wiedereinschaltung**. Nur so bootet der Pi nach leerem Akku von selbst wieder — siehe [1.8](01-machbarkeit.md#wie-der-pi-wieder-aufwacht-wenn-der-akku-leer-war). MPPT ist besser, PWM reicht. | 28 € | [Amazon Suche](https://www.amazon.de/s?k=MPPT+Laderegler+LiFePO4+Lastausgang+12V) |
| B3 | **LiFePO4 12 V / 12 Ah** | 154 Wh, davon ~123 Wh nutzbar = **2,9 Tage ohne Sonne**. LiFePO4 statt Li-Ion wegen Hitzefestigkeit, Zyklenzahl und Brandverhalten — Begründung in [1.8](01-machbarkeit.md#warum-lifepo4-und-nicht-li-ion). | 68 € | [Amazon Suche](https://www.amazon.de/s?k=LiFePO4+12V+12Ah+Akku+BMS) |
| B4 | **DC-DC-Wandler 12 V → 5 V, 3 A**, mit USB-Buchse | ⚠️ **3 A nehmen, nicht 1 A.** Der Pi plus SSD zieht in Spitzen ~1,5 A; ein knapper Wandler ist die häufigste Ursache für unerklärliche Neustarts. | 10 € | [Amazon Suche](https://www.amazon.de/s?k=DC+DC+Wandler+12V+5V+3A+USB) |
| B5 | **INA219** Strom-/Spannungssensor (I2C) | Der Pi hat **keinen Analogeingang** — anders als ein ESP32. Der INA219 misst über I2C und liefert obendrein den **Ladestrom**: Man sieht auf der Website, wie viel die Sonne gerade bringt. Pädagogisch das beste Bauteil im Projekt. | 5 € | [Amazon Suche](https://www.amazon.de/s?k=INA219+Modul+I2C) |
| B6 | Sicherungshalter + Sicherungen (5 A, 2 A) | Zwischen Akku und Regler. Bei 12 V/12 Ah kann ein Kurzschluss Kabel zum Glühen bringen. **Nicht weglassen.** | 5 € | [Reichelt Suche](https://www.reichelt.de/index.html?ACTION=446&LA=446&nbc=1&q=KFZ+Sicherungshalter) |
| | **Summe Gruppe B** | | **151 €** | |

### 2.3 Gruppe C & D — Nachtsicht, Lichtschranke, Gehäuse

| # | Teil | Warum genau das | ~Preis | Link |
|---|---|---|---|---|
| C1 | **IR-LED-Module 940 nm, 4 Stück** | 940 nm ist für Menschen unsichtbar, 850 nm glimmt rot. Fertigmodule mit Vorwiderstand und Steckerleiste → lötfrei. | 6 € | [Amazon Suche](https://www.amazon.de/s?k=IR+LED+Modul+940nm+Arduino) |
| C2 | **MOSFET-Schaltmodul, Logic-Level** (D4184/AOD4184) | Schaltet und dimmt die LEDs per PWM. **„Logic Level" ist Pflicht** — muss mit 3,3 V durchschalten. Ein IRF520-Modul tut das *nicht* zuverlässig. | 4 € | [Amazon Suche](https://www.amazon.de/s?k=MOSFET+Modul+D4184+PWM+Schalter) |
| C3 | **IR-Lichtschranke**, Sender + Empfänger mit Digitalausgang | ⭐ Zählt Ein- und Ausflüge exakt. Der Grund, warum die Statistik echte Zahlen zeigt statt Schätzungen — siehe [1.3](01-machbarkeit.md#13-die-lichtschranke--die-beste-idee-im-ganzen-plan). | 6 € | [Amazon Suche](https://www.amazon.de/s?k=Infrarot+Lichtschranke+Modul+Sender+Empfaenger) |
| C4 | Dupont-Steckkabel-Set (F-F, F-M) | Alle GPIO-Verbindungen. | 5 € | [Amazon Suche](https://www.amazon.de/s?k=Dupont+Kabel+Set+female+female) |
| D1 | **IP65-Gehäuse ca. 200×150×75 mm** | Muss Pi, SSD, Laderegler **und den 12-Ah-Akku** fassen. Der Akku ist der Grund für die Größe — nicht kleiner kaufen. | 18 € | [Reichelt Suche](https://www.reichelt.de/index.html?ACTION=446&LA=446&nbc=1&q=Installationsgeh%C3%A4use+IP65) |
| D2 | Kabelverschraubungen M12/M16 (4×) | Panel, CSI, IR-Kabel, Lichtschranke. | 6 € | [Reichelt Suche](https://www.reichelt.de/index.html?ACTION=446&LA=446&nbc=1&q=Kabelverschraubung+M16) |
| D3 | Acrylglas 3 mm, ~10×10 cm | Schutzscheibe vor der Kameraöffnung. **Kein normales Glas** — Acryl lässt Infrarot besser durch. | 5 € | [Amazon Suche](https://www.amazon.de/s?k=Acrylglas+3mm+Platte+zuschnitt) |
| D4 | Silikagel-Beutel | Gegen Kondenswasser und beschlagene Linse. Jährlich tauschen. | 4 € | [Amazon Suche](https://www.amazon.de/s?k=Silikagel+Beutel+Trockenmittel) |
| D5 | Klettband, Kabelbinder, Schrauben, Kabel 0,75 mm² | Kleinteile. Klettband für den Akku — er soll tauschbar bleiben. | 8 € | — |
| | **Summe Gruppen C + D** | | **62 €** | |

### Gesamtsumme Variante A

| Gruppe | Summe |
|---|---|
| A — Rechner, Kamera, Speicher | 91 € |
| B — Solar & Akku | 151 € |
| C + D — Nachtsicht, Lichtschranke, Gehäuse | 62 € |
| **Gesamt** | **≈ 304 €** |
| *ohne Ersatzkabel A7* | *≈ 299 €* |

**Der Akku und das Panel machen zusammen 103 € — ein Drittel des Projekts.** Das ist der
Preis für „autonom über Solar".

---

## 2.4 Wenn es billiger sein muss

| Sparmaßnahme | Ersparnis | Was es kostet |
|---|---|---|
| **PWM- statt MPPT-Laderegler** | 10 € | ~15 % weniger Ertrag. Bei einem 30-W-Panel gegen 42 Wh Bedarf verkraftbar |
| **SSD 128 GB statt 240 GB** | 7 € | Nichts. 128 GB sind für den Ringspeicher mehr als genug |
| Ersatzkabel A7 weglassen | 5 € | Risiko, mitten im Bau drei Tage zu warten |
| Gehäuse: Vorratsdose + Silikon | 12 € | Für einen Sommer okay, über den Winter nicht |
| Akku 12 V / 7 Ah statt 12 Ah | 23 € | Nur ~1,7 Tage Reserve statt 2,9. Du hattest 3 Tage gewählt — davon rate ich ab |
| **Zusammen (die ersten vier)** | **34 €** | **→ ≈ 262 €** |

**An welchen Teilen man nicht sparen sollte:** DC-DC-Wandler (B4 — 3 A, sonst Neustarts),
Laderegler mit Lastabschaltung (B2 — sonst bootet der Pi nach leerem Akku nie wieder),
Sicherung (B6) und Panel (B1).

### Wenn mehr Budget da ist

| Aufpreis | Was es bringt |
|---|---|
| **+37 €** Akku 20 Ah statt 12 Ah | 5 Tage Reserve statt 3. Praktisch nie Handarbeit |
| +15 € 50-W-Panel statt 30 W | Deutlich entspannter im März und bei Teilschatten |
| +15 € Laderegler mit NTC-Temperaturfühler | Löst das Frost-Ladeproblem richtig |
| +5 € DS18B20 Temperaturfühler (wasserdicht) | Nesttemperatur auf der Website. Am Verlauf erkennt man, ob gebrütet wird — der Vogel *heizt*. Für 5 € das lehrreichste Extra |
| +6 € USB-Y-Kabel mit Stromeinspeisung | Rückfallebene, falls die SSD den USB-Port des Zero überlastet ([1.6](01-machbarkeit.md#️-ein-risiko-bei-der-ssd-das-man-kennen-muss)) |

---

## VARIANTE B — ESP32-S3 ⭐ (die gewählte Variante)

Vollständig beschrieben in [8. Variante ESP32](08-variante-esp32.md), Code liegt fertig
bereit.

> 🛒 **Zum Bestellen nicht diese Tabelle benutzen, sondern
> [9. Bestellliste](09-bestellliste.md)** — dort ist alles nach Shop gruppiert, mit
> Prüfpunkten, Bestellreihenfolge und realistischen Kassenpreisen inklusive Versand
> (**~190 €**, nicht die 149 € Bauteilkosten von hier).

| # | Teil | Warum | ~Preis | Link |
|---|---|---|---|---|
| E1 | **Seeed XIAO ESP32-S3 Sense** | Kamera-Anschluss + SD-Slot + Mikrofon auf einem Board, 8 MB PSRAM, 0,7 W. **Version mit vorgelöteten Pins nehmen.** | 25 € | [BerryBase](https://www.berrybase.de/en/seeed-xiao-esp32s3-sense-esp32-s3r8-wlan-ble-5.0-ov2640-camera-sensor-8mb-psram-8mb-flash) |
| E2 | **Kameramodul 24-pol DVP, ohne IR-Filter** | ⭐ **OV2640 „Night Vision"** (1600×1200 = 1,92 MP, 8–12 fps, sicher lieferbar) **oder** OV5640 (1920×1080 = 2,07 MP, ~6 fps, schwer zu finden). Nur 7 % Pixelunterschied — Entscheidungshilfe in [9.0](09-bestellliste.md#90-️-die-eine-entscheidung-die-du-vorher-treffen-musst) | 10–13 € | [OV2640 Night Vision](https://esp32s.com/product/24pin-ov2640-camera-module-for-esp32-cam-camera-module-2mp-180-66-120-160-222-200-degree-650nm-850nm-night-vision-dvp/) · [eBay OV5640](https://www.ebay.de/sch/i.html?_nkw=OV5640+24pin+no+IR+filter+DVP) |
| E3 | microSD 32 GB **High Endurance** | Dashcam-Klasse. **Max. 32 GB** — mehr kann das Board nicht. | 12 € | [Amazon Suche](https://www.amazon.de/s?k=microSD+32GB+High+Endurance) |
| E4 | FPC-Verlängerung 24-pol, 0,5 mm, ~10 cm | Kamera in den Kasten, Board nach außen. **Max. 15 cm.** | 3 € | [AliExpress Suche](https://de.aliexpress.com/w/wholesale-24pin-0.5mm-FPC-extension-cable.html) |
| E5 | **Solar Power Manager 5 V** (DFRobot DFR0559 o. ä.) | MPPT + Laderegler + 5-V-Ausgang in einem. Schraubklemme, JST, USB — komplett lötfrei. Grenzen: Eingang **4,5–6 V**, Ladestrom **900 mA**, Panel **≤ 10 W** | 10 € | [DFRobot](https://www.dfrobot.com/product-1712.html) · [Wiki](https://wiki.dfrobot.com/dfr0559/) |
| E6 | **5-V-Solarpanel für Akku-Kameras, 10 W, IP65** | ⚠️ Entscheidend ist die **Leerlaufspannung: unter 6,5 V**, sonst stirbt E5. Ein Panel mit Aufdruck „6 V" hat 7,2 V und ist **nicht** geeignet. Das USB-Kabel wird gekürzt und in die Schraubklemme geschraubt. Messanleitung in [9.3b](09-bestellliste.md#93b-️-welches-solarpanel-passt-zum-laderegler) | ~22 € | [Beispiel](https://www.amazon.de/Tragbares-Solarpanel-Ladeger%C3%A4t-Wasserdicht-%C3%9Cberwachungskamera/dp/B0BB79WH7K) · [5-W-Sparvariante](https://www.pearl.de/a-ZX5350-1322.shtml) |
| E7 | **LiPo 1S 5000 mAh** mit Schutzschaltung + JST-PH 2.0 | 18,5 Wh ≈ 1,2 Tage. Für 3 Tage: 10000 mAh (+12 €). | 14 € | [Pollin](https://www.pollin.de/stromversorgung/akkus/lipo-akkus/) |
| E8 | Spannungssensor-Modul 25 V | Akkustand. Der ESP32 hat — anders als der Pi — einen Analogeingang. | 2 € | [Amazon Suche](https://www.amazon.de/s?k=Spannungssensor+Modul+25V+Arduino) |
| E9 | IR-LEDs, MOSFET-Modul, Lichtschranke, Dupont | wie C1–C4 bei Variante A | 21 € | s. oben |
| E10 | IP65-Gehäuse (klein), Verschraubungen, Acryl, Silikagel, Kleinteile | Kleineres Gehäuse als bei A — kein 12-Ah-Akku darin | 30 € | s. oben |
| | **Gesamt Variante B** | | **≈ 149 €** | |

---

## 2.5 Bestell-Checkliste

### Beide Varianten

- [ ] Kameramodul ist **ohne IR-Filter** („NoIR" / „no IR filter" / „night vision")
- [ ] IR-LEDs sind **940 nm**, nicht 850 nm
- [ ] MOSFET-Modul ist **Logic-Level** (D4184/AOD4184), **nicht** IRF520
- [ ] Lichtschranke hat einen **digitalen** Ausgang (nicht nur analog)

### Nur Variante A (Pi)

- [ ] Pi ist die **„WH"**-Version (Stiftleiste bereits angelötet)
- [ ] CSI-Kabel ist das **Zero-Kabel** (22-pol 0,5 mm ↔ 15-pol 1 mm) — das aus der
      Kamerapackung passt nicht
- [ ] Laderegler ist **LiFePO4-fähig**, hat **Last-Ausgang** und **Tiefentladeschutz**
- [ ] Panel ist **12 V** (nicht 6 V)
- [ ] DC-DC-Wandler kann **3 A** (nicht 1 A)
- [ ] Sicherung ist dabei
- [ ] SSD ist **2,5-Zoll-SATA im USB-Gehäuse** (sparsamer als NVMe)

### Nur Variante B (ESP32)

- [ ] XIAO ist die **„Sense"**-Version, möglichst mit vorgelöteten Pins
- [ ] Kamera ist **OV5640** (nicht OV2640) — nur der schafft 1080p
- [ ] Panel-**Leerlaufspannung** in der Sonne gemessen: **unter 6,5 V** (kein „6 V"-Panel!)
- [ ] Akku hat **Schutzschaltung** und **JST-PH-2.0-Stecker**
- [ ] microSD ist **max. 32 GB** und **High Endurance**

> **Bestellreihenfolge:** Kameramodul und Laderegler haben die längsten Lieferzeiten —
> **die beiden zuerst**. Der Rest kommt in zwei Sammelbestellungen (BerryBase/Reichelt und
> Amazon).

→ Weiter mit [3. Schaltplan](03-schaltplan.md)
