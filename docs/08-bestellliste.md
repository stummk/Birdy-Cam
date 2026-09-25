# 8. Bestellliste — zum Ausdrucken und Abhaken

Nach **Shop sortiert**, damit du Versandkosten sparst. Am Ende steht eine
[Wareneingangs-Prüfung](#89-wareneingangs-prüfung) — die bitte wirklich machen.

> ⚠️ **Ehrliche Vorwarnung zur Summe.** In der [Stückliste](02-stueckliste.md) steht
> „≈ 159 €“. Das sind die reinen Bauteilkosten. An der Kasse wird es mehr, aus drei Gründen:
>
> 1. **Vieles gibt es nur im Mehrfachpack.** Ein einzelnes MOSFET-Modul findet man kaum —
>    es kommen drei oder fünf. Kein Nachteil (Ersatz liegt im Schrank), kostet aber mehr.
> 2. **Versand über vier bis fünf Shops** — realistisch 12–25 €.
> 3. **Kleinteile**, die in der Schätzung untergehen: USB-Kabel, Klettband, Stiftleiste.
>
> **Realistisch an der Kasse: 195–210 €.** Mit der [Sparliste](#86-sparliste) sind ~160 €
> erreichbar, wenn Werkzeug und Kleinteile schon im Haus sind.

> 🧪 **Baust du [Variante B](04-bauplan.md#410-variante-b--alles-im-kasten-kamera-hinter-dem-klardeckel)**
> — alles im Kasten, Kamera hinter dem Klardeckel? Dann **zuerst
> [8.4e](#84e-variante-b--was-sich-an-der-bestellung-ändert) lesen.** Zwei Positionen unten
> fallen weg, eine wird ausgetauscht, fünf kommen dazu, und der Akku bekommt ein Maximalmaß.

---

## 8.0 ⚠️ Die eine Entscheidung, die du vorher treffen musst

Es gibt zwei Kameramodule, und sie unterscheiden sich weniger, als der Name vermuten lässt:

| | **OV2640 „Night Vision“** ⭐ | OV5640 ohne IR-Filter |
|---|---|---|
| Max. Auflösung | 1600 × 1200 | 1920 × 1080 („Full HD“) |
| **Megapixel** | **1,92** | **2,07** |
| Bildformat | 4:3 | 16:9 |
| Bilder pro Sekunde | **8 bis 12** | ~6 |
| Lieferbarkeit „ohne IR-Filter“ | ✅ gut | 🟡 schwer zu finden |
| Preis | ~10 € | ~13 € |

**Der entscheidende Punkt:** 1,92 gegen 2,07 Megapixel — das sind **7 % Unterschied**. Auf
dem Bild sieht man das nicht. Was man sehr wohl sieht, ist die **doppelte Bildrate**.

> ⭐ **Empfehlung: OV2640 „Night Vision“.** Praktisch dieselbe Detailmenge wie Full HD,
> flüssigeres Video, besser lieferbar. Die Firmware ist ab Werk darauf eingestellt
> (`FRAMESIZE_UXGA`).
>
> Wenn dir das **16:9-Format** wichtig ist (weil es auf Handy und Fernseher formatfüllend
> ist), nimm den OV5640 und stelle `FRAMESIZE_FHD` ein. Beides ist vorbereitet.

> ⚠️ **Und ein Hinweis zur Ehrlichkeit:** Ein fertiges Nachtsicht-Modul, das ausdrücklich
> für die XIAO Sense verkauft wird, gibt es nicht. Die 24-poligen **DVP**-Module aus der
> ESP32-CAM-Welt passen mechanisch und elektrisch, und davon gibt es Nachtsicht-Varianten.
> **Deshalb: zwei Stück bestellen und vor dem Einbau testen.**

---

## 8.1 Sammelbestellung A — BerryBase

🔗 [berrybase.de](https://www.berrybase.de) · versandkostenfrei ab 29 €

| ✓ | Anz. | Teil | Achtung! | Preis |
|---|---|---|---|---|
| ☐ | 1 | **[Seeed XIAO ESP32-S3 Sense](https://www.berrybase.de/en/seeed-xiao-esp32s3-sense-esp32-s3r8-wlan-ble-5.0-ov2640-camera-sensor-8mb-psram-8mb-flash)** | Muss **„Sense“** heißen — nur die hat Kameraanschluss und SD-Slot | 25 € |
| ☐ | 1 | **[Waveshare Solar Power Manager](https://www.berrybase.de/en/solar-power-management-module-for-6v~24v-solar-panel)** (für 6–24 V Solarpanel) | Laderegler + MPPT + 5-V-Ausgang in einem. ⚠️ Nicht mit den Varianten B/C/D verwechseln — wir wollen das **Grundmodell mit PH-2.0-Akkubuchse** | 9 € |
| ☐ | 1 | *(optional)* [IR-Hindernissensor mit Digitalausgang](https://www.berrybase.de/bauelemente/sensoren-module/bewegung-distanz/ir-hindernissensor/lichtschranke-mit-digitalem-ausgang) | 3,3–5 V, mit Potentiometer. Das ist die Lichtschranke — siehe [8.4b](#84b-zur-lichtschranke) | 3 € |
| ☐ | 1 | *(nur falls nötig)* Stiftleiste 2×7, 2,54 mm | **Nur, wenn das Board ohne angelötete Pins kommt.** Vorher in der Artikelbeschreibung nachsehen | 1 € |
| | | | | **~38 €** *(versandfrei)* |

> Alternative für das Board: [Reichelt](https://www.reichelt.com/de/en/shop/product/xiao_esp32s3_sense_wifi_bt_camera_ov3660_without_header-358353)
> — dort steht **„without header“**, dann Stiftleiste zwingend mitbestellen.
>
> Alternative für den Laderegler: [Eckstein](https://eckstein-shop.de/WaveshareSolarPowerManagementModule2Cfor6V-24VSolarPanelEN) (10,70 €)
> oder direkt bei [Waveshare](https://www.waveshare.com/solar-power-manager.htm).

---

## 8.2 Sammelbestellung B — eBay / AliExpress ⏰ **zuerst bestellen!**

Diese Teile kommen oft aus Asien und brauchen **2–4 Wochen**. Deshalb als Erstes ordern.

| ✓ | Anz. | Teil | Achtung! | Preis |
|---|---|---|---|---|
| ☐ | **2** | **Kameramodul 24-polig DVP, ohne IR-Filter** | Im Angebot muss **„no IR filter“**, **„night vision“** oder „850nm/940nm“ stehen. **Zwei Stück** — eins ist Ersatz. Weitwinkel (120–160°) bevorzugen | 2 × 10 € |
| ☐ | 1 | **FPC-Verlängerung 24-polig, 0,5 mm, 10 cm** | **24-polig** und **0,5 mm Raster**. Nicht länger als 15 cm, sonst rauscht das Bild | 4 € |
| | | | | **~24 €** + Versand |

**Suchlinks:**
[OV2640 Night Vision (esp32s.com)](https://esp32s.com/product/24pin-ov2640-camera-module-for-esp32-cam-camera-module-2mp-180-66-120-160-222-200-degree-650nm-850nm-night-vision-dvp/) ·
[eBay: OV2640 24pin night vision](https://www.ebay.de/sch/i.html?_nkw=OV2640+24pin+night+vision+no+IR+filter) ·
[eBay: OV5640 24pin DVP](https://www.ebay.de/sch/i.html?_nkw=OV5640+24pin+DVP+no+IR+filter) ·
[AliExpress: 24pin 0.5mm FPC](https://de.aliexpress.com/w/wholesale-24pin-0.5mm-FPC-extension-cable.html)

> 💬 **Frage an den Verkäufer, wenn es unklar ist:** *„Is the IR filter removed?“*
> Ein Modul **mit** Filter ist nachts blind, egal wie stark die IR-LEDs sind.

---

## 8.3 Sammelbestellung C — Amazon

| ✓ | Anz. | Teil | Achtung! | Preis |
|---|---|---|---|---|
| ☐ | 1 | **[Solarpanel HPLive 15 W / 12 V](https://www.amazon.de/s?k=HPLive+Solarpanel+15W+12V)** | ⚠️ Auf dem Aufkleber muss `Voc` / „Leerlaufspannung“ **unter 24 V** stehen. Bei 12-V-Panels üblich: 18–22 V. Siehe [8.3b](#83b-zum-solarpanel) | ~30 € |
| ☐ | 1 | **[microSD 32 GB High Endurance](https://www.amazon.de/s?k=microSD+32GB+High+Endurance)** | **Höchstens 32 GB** (Grenze des Boards) und **„High Endurance“** / „Dashcam“ | 12 € |
| ☐ | 4 | **[IR-LED-Module 940 nm](https://www.amazon.de/s?k=IR+LED+Modul+940nm+Arduino)** | ⚠️ **940 nm**, nicht 850 nm (das glimmt sichtbar rot). ⚠️ **Module** mit Vorwiderstand auf der Platine — nackte LEDs brauchen vier Widerstände dazu, siehe [8.4d](#84d-zu-den-ir-leds) | 7 € |
| ☐ | 1 | **[MOSFET-Modul Logic Level](https://www.amazon.de/s?k=MOSFET+Modul+D4184+PWM)** (D4184 / AOD4184) | ⚠️ **Kein IRF520!** Der schaltet bei 3,3 V nicht durch. Kommt meist im Mehrfachpack. Zwei Bauformen im Umlauf — am besten **von beiden eine** mitbestellen, siehe [8.4c](#84c-zum-mosfet-modul) | 8 € |
| ☐ | 1 | [Spannungssensor-Modul 0–25 V](https://www.amazon.de/s?k=Spannungssensor+Modul+25V+Arduino) | Kommt meist im 5er-Pack | 6 € |
| ☐ | 1 | **[JST-PH-2.0-Pigtail-Set](https://www.amazon.de/s?k=JST+PH+2.0+Stecker+Buchse+Kabel+Set)** (Stecker **und** Buchse mit Kabel) | Du brauchst je eins von beiden — verteilt wird in der Schraubklemme des Sensors. ⚠️ **Nicht** nach einem Y-Kabel suchen, siehe [8.3c](#83c-warum-kein-y-kabel). Meist 10+ Paare im Set | 6 € |
| ☐ | 1 | [Dupont-Steckkabel-Set](https://www.amazon.de/s?k=Dupont+Kabel+Set+female+female) (F-F und F-M) | 10–20 cm Länge reicht | 6 € |
| ☐ | 1 | [Acrylglas 3 mm, ~10 × 10 cm](https://www.amazon.de/s?k=Acrylglas+3mm+Platte) | ⚠️ **Kein Glas** — Acryl lässt Infrarot durch | 6 € |
| ☐ | 1 | [Silikagel-Beutel](https://www.amazon.de/s?k=Silikagel+Beutel+Trockenmittel) | Gegen beschlagene Linse | 5 € |
| ☐ | 1 | USB-Kabel A → C, ca. 30 cm | Verbindet Laderegler und Board. ⚠️ Ein **richtiges Datenkabel**, kein dünnes Ladekabel | 5 € |
| ☐ | 1 | [Klettband (Rolle)](https://www.amazon.de/s?k=Klettband+Rolle) | Für den Akku — **nicht** Kabelbinder | 4 € |
| | | | | **~95 €** *(versandfrei ab 39 €)* |

### 8.3b Zum Solarpanel

**Warum ein 12-Volt-Panel?** Der Waveshare-Laderegler nimmt alles zwischen **6 und 24 Volt**
und rechnet selbst herunter. Damit hast du bei der Panelauswahl freie Hand — jedes gängige
12-V-Gartenpanel passt, und das sind die, die es überall gibt.

**Warum 15 Watt und nicht 10?** Der Laderegler nimmt sich, was er braucht — mehr Panel
schadet nie. Die Wattzahl entscheidet nicht über die Spitzenleistung, sondern darüber,
**wie viele Stunden am Tag** die volle Ladeleistung überhaupt erreicht wird:

```
   10-W-Panel  ->  volle Ladung nur um die Mittagszeit
   15-W-Panel  ->  volle Ladung schon ab etwa einem Drittel Sonne,
                   also fast den ganzen hellen Tag
```

Im Mai ist das egal. **Im März und bei Bewölkung ist es der Unterschied**, ob der Akku
über den Tag kommt.

**Das Kabel:** Aus dem Panel kommen zwei Adern (oft mit einem Hohlstecker dran). Der Stecker
wird abgeschnitten, die Adern kommen in die grüne Schraubklemme des Ladereglers —
[Schaltplan 3.4](03-schaltplan.md#34-das-solarpanel-anschließen).

**Verlängern:** Bei 12 Volt völlig unkritisch. Bis etwa 10 Meter mit 0,75 mm² verlierst du
nur ein paar Zehntelvolt.

> ⚠️ **Der eine Wert, den du prüfen musst:** `Voc` bzw. „Leerlaufspannung“ auf dem
> Aufkleber. Der muss **unter 24 V** liegen. Bei einem 12-V-Panel steht dort typisch 18–22 V
> — das passt bequem. Ein **24-V-Panel** wäre zu knapp, ein größeres Hausdachmodul erst
> recht.

### 8.3c Warum kein Y-Kabel

Laderegler und Spannungssensor wollen beide an denselben Akku. Naheliegend wäre ein
Y-Kabel — **es gibt aber keins.** JST-**PH 2.0**-Splitter werden praktisch nicht verkauft.
Was im Modellbau als „JST-Y-Kabel“ oder „Parallel-Adapter“ auftaucht, ist fast immer der
dickere **JST-RCY/BEC** mit 2,5 mm Raster. Der passt nicht in die `BAT`-Buchse des
Ladereglers. Lange danach zu suchen lohnt nicht.

Stattdessen nimmst du zwei **Pigtails** — kurze Kabel mit Stecker bzw. Buchse an einem
Ende, blanke Adern am anderen — und benutzt die **Schraubklemme des Sensors als
Verteiler**. In jede Klemme passen zwei Adern:

```
   🔋 Akku ──► [Buchsen-Pigtail] ──► Sensor-Schraubklemme ──► [Stecker-Pigtail] ──► Laderegler BAT
```

Volle Anleitung in [Schaltplan 3.8](03-schaltplan.md#38-der-spannungssensor--damit-du-den-akkustand-siehst).

**Zwei Wege, das zu kaufen:**

| | |
|---|---|
| **Pigtail-Set** (empfohlen) | Stecker- und Buchsenkabel gemischt, meist 10+ Paare. Nichts zu schneiden |
| **Verlängerungskabel** Stecker→Buchse | In der Mitte durchschneiden — das ergibt genau dieselben zwei Pigtails. Oft **AWG24** statt AWG26, und dickere Adern halten in der Schraubklemme besser. Gibt es z. B. bei [BerryBase](https://www.berrybase.de/en/extension-cable-2-pin-jst-ph-2.0mm-male-female-awg24-50cm) |

> ⚠️ **Beim Auspacken durchmessen:** Rot ist bei JST-PH 2.0 *meistens* Plus, garantiert
> ist es nicht. Prüfe, dass Stecker- und Buchsen-Pigtail **Rot auf demselben Pin** haben —
> sonst verpolst du den Akku, sobald du beide zusammenschraubst. Die Messung dauert zwei
> Minuten und steht Schritt für Schritt in
> [Schaltplan 3.8](03-schaltplan.md#durchtesten-mit-dem-multimeter--vier-messungen).

---

## 8.4 Sammelbestellung D — Akku, Gehäuse, Verschraubungen

| ✓ | Anz. | Teil | Achtung! | Preis | Shop |
|---|---|---|---|---|---|
| ☐ | 1 | **LiPo-Akku 1S, 5000 mAh**, mit Schutzschaltung + JST-PH 2.0 | ⚠️ **Mit Schutzschaltung** ist Pflicht. Steckertyp prüfen! 10 000 mAh = +12 €, doppelte Reserve | 14 € | [Pollin](https://www.pollin.de/stromversorgung/akkus/lipo-akkus/) · [lithium-polymer-akkus.de](https://www.lithium-polymer-akkus.de/lipo-akku-pack-25mah-to-10000mah/) |
| ☐ | 1 | **IP65-Gehäuse ca. 120 × 80 × 50 mm** | Muss Board, Laderegler und Akku fassen | 11 € | [Reichelt](https://www.reichelt.de/index.html?ACTION=446&LA=446&nbc=1&q=Installationsgeh%C3%A4use+IP65) |
| ☐ | 3 | Kabelverschraubung M12 | Für Panelkabel, Kamerakabel, IR-/Lichtschrankenkabel | 5 € | [Reichelt](https://www.reichelt.de/index.html?ACTION=446&LA=446&nbc=1&q=Kabelverschraubung+M12) |
| | | | | **~30 €** | + Versand |

### 8.4b Zur Lichtschranke

Sie zählt Ein- und Ausflüge exakt und liefert die Aufenthaltsdauer. **Die Firmware wird
ohne sie ausgeliefert** — `LICHTSCHRANKE_AN` steht ab Werk auf `false`, und das Auslösen
übernimmt die Bilderkennung. Eingebaut und justiert, stellst du den Wert auf `true`.

**Warum sie hier als optional geführt wird:** Ein gut lieferbares Sender/Empfänger-Paar
konnte ich nicht bestätigen. Der überall verfügbare Baustein ist ein **Reflex-Sensor**
(Bauform FC-51): IR-LED und Empfänger sitzen nebeneinander auf einem Board, gemessen wird
die Reflexion am Vogel. Das funktioniert, hat aber zwei Schwächen:

- **Dunkle Vögel reflektieren schwächer** als helle → Empfindlichkeit am Drehrädchen
  einstellen
- **Direkte Sonne kann blenden** → ein Stück Strohhalm als Blende über den Sensor schieben

Alternative, falls du sie findest:
[Lichtschranken-Paar Sender/Empfänger](https://www.ebay.de/sch/i.html?_nkw=Infrarot+Lichtschranke+Modul+Sender+Empf%C3%A4nger) (~6 €).
Die Firmware kommt mit beiden Bauarten zurecht.

---

### 8.4c Zum MOSFET-Modul

Unter dem Namen „D4184-Modul“ werden **zwei verschiedene Platinen** verkauft. Auf den
Angebotsbildern sehen sie ähnlich aus; angeschlossen werden sie unterschiedlich. Zähl
einfach die Anschlüsse:

| | **Variante A — „HW-532“** | **Variante B — „XY-MOS“** |
|---|---|---|
| Größe | 23 × 17 mm | ca. 50 × 25 mm |
| Steuerseite | **2**: `PWM`, `GND` | **3**: `SIG`, `VCC`, `GND` |
| Lastseite | **3**: `+`, `LOAD`, `−` | **4**: `VIN+`, `VIN−`, `OUT+`, `OUT−` |
| Optokoppler `PC817` | ja | nein |
| Lieferbarkeit | ✅ überall, meist 5er/6er-Pack | 🟡 seltener |
| An unseren 5 V | meistens ja, aber nicht garantiert | ✅ sicher |
| Maximale PWM-Frequenz | ~1 kHz (Optokoppler) | 20 kHz |

Variante A erzeugt die Gate-Spannung als **Hälfte der Lastspannung**. Bei 12 V ist das
bequem, bei unseren 5 V bleiben nur ~2,5 V — genau an der Einschaltschwelle des AOD4184.
Die meisten Exemplare schalten trotzdem sauber durch, manche nicht. Die ganze Erklärung
steht in [3.6](03-schaltplan.md#36-das-unsichtbare-nachtlicht--mosfet-und-ir-leds).

> 💡 **Empfehlung: von beiden eine mitbestellen.** Zusammen unter 10 €, und
> [Sketch 4](05-software.md#52-die-sieben-lern-sketches) sagt dir in zwei Minuten, welche
> bei dir funktioniert. Das ist billiger als eine Woche Wartezeit mitten im Bau.

Suchbegriffe:
[Variante A „HW-532 D4184“](https://www.amazon.de/s?k=HW-532+D4184+MOSFET+Modul) ·
[Variante B „MOSFET Modul 5-36V 15A“](https://www.amazon.de/s?k=MOSFET+Modul+D4184+5-36V+15A+PWM)

---

### 8.4d Zu den IR-LEDs

Hier gibt es drei Dinge zu prüfen, und nur eines davon steht zuverlässig in den
Artikeldaten. Der Reihe nach:

| Worauf achten | Warum | Was tun, wenn es nicht dasteht |
|---|---|---|
| ⭐ **940 nm** | 850-nm-LEDs glimmen für Menschen **sichtbar dunkelrot** — nachts vier rote Punkte über dem Nest | Beim Händler nachfragen. Viele Angebote für IR-Sender nennen die Wellenlänge gar nicht |
| **Vorwiderstand** | Ohne ihn zieht die LED an 5 V statt 20 mA schnell ein **Ampere** und das MOSFET-Modul schmort | Einfach vier Widerstände mitbestellen (unten) — dann ist es egal, was auf der Platine sitzt |
| **Nennstrom** | Manche Sendermodule sind mit **30–60 mA** je Stück angegeben, also 120–240 mA für vier statt der 80 mA, mit denen die Anleitung rechnet | Unkritisch für MOSFET und 5-V-Zweig, aber nachts am Akku spürbar. Erste Woche den Akkustand beobachten |

> ⚠️ **„Modul“ heißt nicht automatisch „Vorwiderstand drauf“.** Beim verbreiteten
> **KY-005** ist je nach Revision einer bestückt — oder es sind nur zwei leere Lötpads da.
> Ohne Widerstand ist so ein Modul elektrisch exakt eine nackte LED. Wie du es in zwei
> Minuten ausmisst, steht in
> [Schaltplan 3.6](03-schaltplan.md#dein-led-modul-hat-drei-pins-statt-zwei).

**Zwei Pins oder drei?** Zwei (`+` / `−`) ist am bequemsten — such nach „IR LED Platine
940nm 5V“ oder „Infrarot Illuminator Board“, das sind als Beleuchtung gebaute Platinen.
Dreipolige Sendermodule funktionieren genauso gut, einer der drei Pins bleibt dann frei.

**Und die günstigste Variante: nackte LEDs.** In den 5-mm-Bohrungen des Deckels sitzen sie
sogar bequemer als die Platinen, und du weißt genau, welcher Strom fließt. Dann gehören
**vier Widerstände** mit in den Warenkorb:

| | |
|---|---|
| **180 Ω, ¼ W**, 4 Stück | ergibt ~20 mA je LED — genau der Strom, mit dem die Anleitung rechnet. **200 Ω** aus einem Sortiment tun es genauso (18,5 mA) |
| **WAGO 221**, 2-fach, 4 Stück | je eine pro LED-Zweig. Die steckbaren 773/2273 halten ein Widerstandsbeinchen **nicht** |

Ohne Vorwiderstand zieht eine LED an 5 V statt 20 mA schnell ein **Ampere** — das
MOSFET-Modul wird heiß und riecht verschmort, während die LEDs munter weiterleuchten. Die
Rechnung steht in
[Schaltplan 3.6](03-schaltplan.md#nackte-leds-statt-module-dann-brauchst-du-vier-vorwiderstände).

Suchbegriffe:
[Widerstandssortiment ¼ W](https://www.amazon.de/s?k=Widerstand+Sortiment+1%2F4W+Metallschicht) ·
[WAGO 221 Set](https://www.amazon.de/s?k=WAGO+221+Set)

---

## 8.4e Variante B — was sich an der Bestellung ändert

Nur für [4.10 Variante B](04-bauplan.md#410-variante-b--alles-im-kasten-kamera-hinter-dem-klardeckel)
(alles im Kasten, Kamera hinter dem Klardeckel). Wer Variante A baut, überspringt das hier.

**Streichen** — diese zwei Zeilen brauchst du nicht:

| Wo | Zeile | Warum |
|---|---|---|
| [8.2](#82-sammelbestellung-b--ebay--aliexpress--zuerst-bestellen) | FPC-Verlängerung 24-polig | Die Kamera bleibt in der Box, es geht kein Flachbandkabel nach draußen |
| [8.3](#83-sammelbestellung-c--amazon) | Acrylglas 3 mm | Der Klardeckel ist das Objektivfenster |

**Ändern** — statt der Gehäusezeile in [8.4](#84-sammelbestellung-d--akku-gehäuse-verschraubungen):

| ✓ | Anz. | Teil | Achtung! | Preis | Shop |
|---|---|---|---|---|---|
| ☐ | 1 | **Hammond `1555FL2GY`** — Gehäuse 120 × 90 × 45 mm, Polycarbonat | Das **`2GY`** im Namen ist Polycarbonat (UV-stabil), nicht ABS. [Datenblatt](https://www.hammfg.com/electronics/small-case/plastic/1555) | ~18 € | [Reichelt](https://www.reichelt.com/de/en/shop/manufacturer/hammond%20manufacturing) · [TME](https://www.tme.eu/de/katalog/?search=1555FL2GY) · [Distrelec](https://www.distrelec.de/de/search?q=1555FL2GY) |
| ☐ | 1 | **Hammond `1554FCL`** — Klardeckel, 120 × 90 × 15 mm | ⚠️ **Separate Position.** Ohne ihn liegt ein grauer Deckel im Karton und die ganze Variante fällt in sich zusammen. Passt auf F, FL und G. [Datenblatt](https://www.hammfg.com/electronics/small-case/plastic/1554-1555lid) | ~8 € | dieselben |

**Dazu** — diese fünf Kleinteile kommen neu hinzu:

| ✓ | Anz. | Teil | Achtung! | Preis |
|---|---|---|---|---|
| ☐ | 1 | **USB-Kabel A → C mit 90°-Winkelstecker, 10–15 cm** | *Statt* des 30-cm-Kabels aus [8.3](#83-sammelbestellung-c--amazon). Auch hier ein **Datenkabel** | 6 € |
| ☐ | 1 | **Steuerkabel LiYY 6 × 0,14 mm², 1 m** | IR-LEDs **und** Lichtschranke gehen durch **eine** M12-Verschraubung. Einzellitzen dichten darin nicht | 3 € |
| ☐ | 1 | **Reedschalter**, Glasröhrchen, Schließer (+ kleiner Neodym-Magnet) | Kommt an `RST`/`GND`. Der einzige Weg, das Board im April neu zu starten | 1 € |
| ☐ | 1 | **PVC-Platte 2 mm** + 4 × **M3-Distanzhülse 12 mm** | Zwischenboden für den Laderegler. Darf **nicht** auf dem Akku aufliegen | 3 € |
| ☐ | 1 | **Moosgummi schwarz, 2–3 mm** | Blendring ums Objektiv, gegen Reflexionen im Fenster. Rest reicht auch fürs Mikrofonloch | 2 € |

> ⚠️ **Die Akkuzeile in [8.4](#84-sammelbestellung-d--akku-gehäuse-verschraubungen) bekommt
> eine harte Nebenbedingung:** höchstens **95 × 58 × 12 mm**. In der 45-mm-Box stapeln sich
> Akku (11) + Zwischenboden (2) + Laderegler (23) auf 36 von 37 verfügbaren Millimetern —
> da ist kein Spielraum. Steht kein Maß in der Artikelbeschreibung, woanders bestellen.
> Und **10 000 mAh geht nicht mehr**, der Pack ist zu dick.

> ⚠️ **Bevor du bestellst, lies
> [4.10.6](04-bauplan.md#4106-vor-dem-zuschrauben-drei-versicherungen).** In Variante B
> kommst du fünf Monate nicht an den USB-Anschluss, und die Firmware hat heute weder OTA
> noch Watchdog. Das ist kein Bestellposten, aber die eigentliche Voraussetzung.

**Delta unterm Strich: ≈ +12 €** (−3 FPC, −6 Acryl, +14 Gehäuse, +9 Kleinteile,
+1 teureres USB-Kabel).

---

## 8.5 Werkzeug — falls nicht im Haus

| ✓ | Werkzeug | Preis | Wofür |
|---|---|---|---|
| ☐ | Heißklebepistole + Sticks | 12 € | Kamera, LEDs, Abdichtung |
| ☐ | Akkuschrauber + Holzbohrer 3/5/6/12 mm | 40 € | Löcher im Deckel und in der Wand |
| ☐ | Kleiner Schlitzschraubendreher | 3 € | Schraubklemmen |
| ☐ | Seitenschneider, Abisolierzange | 15 € | Kabel |
| ☐ | Multimeter | 20 € | Pigtails auf Verpolung prüfen ([Schaltplan 3.8](03-schaltplan.md#durchtesten-mit-dem-multimeter--vier-messungen)), Akku kalibrieren (Sketch 5), Fehlersuche |
| ☐ | Lötkolben-Set | 25 € | **nur** falls das Board ohne Pins kommt |

Das **Multimeter** ist die sinnvollste Investition davon — es macht die Kalibrierung in
[Sketch 5](05-software.md#52-die-sieben-lern-sketches) exakt statt geschätzt, und es bleibt für
alle künftigen Projekte nützlich.

---

## 8.6 Sparliste

| Weglassen / ersetzen | Ersparnis | Was es kostet |
|---|---|---|
| Zweites Kameramodul | 10 € | Bei einem Defekt 2–4 Wochen Wartezeit mitten im Bau |
| Gehäuse: Vorratsdose + Silikon | 11 € | Für einen Sommer okay, über den Winter nicht |
| Acryl, Silikagel, Klettband, USB-Kabel aus dem Haus | 20 € | nichts, wenn vorhanden |
| Lichtschranke weglassen | 3 € | Die Statistik wird geschätzt statt gemessen |
| 10-W-Panel statt 15 W | ~8 € | Reicht ab April, im März wird es knapp |
| **Zusammen** | **~52 €** | **→ ~160 € an der Kasse** |

**Wo man nicht sparen sollte:** Akku mit Schutzschaltung, High-Endurance-Speicherkarte und
das Kameramodul ohne IR-Filter.

---

## 8.7 Bestellreihenfolge

Die Reihenfolge ist nicht beliebig — sie entscheidet, ob du wartest:

```
   Woche 1   ▶ eBay / AliExpress: 2× Kameramodul + FPC-Kabel
               (2-4 Wochen Lieferzeit — die Bremse im ganzen Projekt)

   Woche 1   ▶ BerryBase: Board + Laderegler + Lichtschranke
               (schnell da -> Sketch 1 und 2 gehen sofort los!)

   Woche 2   ▶ Amazon: Panel, Karte, Module, Kleinteile
             ▶ Reichelt: Gehäuse + Verschraubungen
             ▶ Akku
```

> 🧪 **In [Variante B](#84e-variante-b--was-sich-an-der-bestellung-ändert)** entfallen in
> Woche 1 das FPC-Kabel und in Woche 2 das Acrylglas. Das **Hammond-Gehäuse samt Klardeckel**
> ist dafür oft Lagerware beim Distributor statt Sofortversand — in Woche 1 mitbestellen,
> nicht erst in Woche 2.

> ⭐ **Der Trick:** Mit Board und Speicherkarte allein kannst du schon
> [Sketch 1 und 2](05-software.md#52-die-sieben-lern-sketches) machen. Du fängst also an,
> während die Kamera noch unterwegs ist — und die Vorfreude hat einen Zwischenschritt.

---

## 8.8 Summen

| Posten | Betrag |
|---|---|
| A — BerryBase (Board, Laderegler, Lichtschranke, Stiftleiste) | 38 € |
| B — eBay / AliExpress (2× Kamera, FPC-Kabel) | 24 € |
| C — Amazon (Panel, Karte, Module, Kleinteile) | 95 € |
| D — Akku, Gehäuse, Verschraubungen | 30 € |
| **Bauteile gesamt** | **≈ 187 €** |
| *Aufschlag für [Variante B](#84e-variante-b--was-sich-an-der-bestellung-ändert)* | *+ ≈ 12 €* |
| Versand (4–5 Shops) | 12–25 € |
| **Realistisch an der Kasse** | **≈ 200–210 €** |
| *mit Sparliste [8.6](#86-sparliste)* | *≈ 160 €* |
| Werkzeug, falls komplett fehlend | + bis 115 € |

---

## 8.9 Wareneingangs-Prüfung

Bitte **vor** dem Einbau prüfen, nicht danach.

- [ ] Board ist die **„Sense“**-Version (Kameraanschluss und SD-Slot vorhanden)
- [ ] Pins am Board angelötet? Wenn nein → ist die Stiftleiste da?
- [ ] Kameramodul: **24-polig**, Raster **0,5 mm**
- [ ] ⭐ **Kameramodul: kein IR-Filter.** Test: [Sketch 4](05-software.md#52-die-sieben-lern-sketches)
      laufen lassen (IR-LEDs an), dann in [Sketch 3](05-software.md#52-die-sieben-lern-sketches)
      im **abgedunkelten** Raum ins Bild schauen. Sieht man etwas → der Filter ist weg ✅
- [ ] IR-LEDs: mit der **Handy-Frontkamera** prüfen, ob sie leuchten
- [ ] IR-LEDs: wirklich **Module** (kleine Platine mit Bauteilen drauf)? Nackte 5-mm-LEDs brauchen **vier Vorwiderstände**, sonst schmort das MOSFET-Modul ([8.4d](#84d-zu-den-ir-leds))
- [ ] MOSFET-Modul: steht **D4184** oder **AOD4184** auf dem Chip? (Nicht IRF520)
- [ ] MOSFET-Modul: Anschlüsse zählen — 2 + 3 ist Variante A, 3 + 4 ist Variante B ([8.4c](#84c-zum-mosfet-modul))
- [ ] **Solarpanel: `Voc` auf dem Aufkleber unter 24 V?**
- [ ] Laderegler: **MPPT-SET-Schalter** auf der Rückseite gefunden? Auf **12V** stellen
- [ ] Laderegler: **Akkuschalter** gefunden? Muss später auf **ON**
- [ ] Akku: Schutzschaltung vorhanden, JST-PH-2.0-Stecker passt in die Buchse
- [ ] Pigtails: Buchse nimmt den **Akkustecker** auf, Stecker passt in die **`BAT`-Buchse** des Ladereglers
- [ ] Pigtails: liegt bei beiden **Rot auf demselben Pin**? Mit dem Durchgangsprüfer nachmessen, sonst verpolst du den Akku — Anleitung in [Schaltplan 3.8](03-schaltplan.md#durchtesten-mit-dem-multimeter--vier-messungen)
- [ ] microSD: höchstens 32 GB, als **FAT32** formatiert
- [ ] USB-Kabel A→C ist ein **Datenkabel**, kein reines Ladekabel

> ⭐ **Der vierte Punkt ist der wichtigste.** Ein Kameramodul mit IR-Filter ist nachts
> blind — und das merkst du sonst erst, wenn alles im Kasten verklebt ist und der Kasten
> bis September nicht mehr geöffnet werden darf.

**Nur für [Variante B](#84e-variante-b--was-sich-an-der-bestellung-ändert):**

- [ ] ⭐ **Klardeckel `1554FCL` wirklich dabei?** Liegt ein grauer Deckel im Karton, wurde er
      vergessen — nachbestellen, bevor du weiterbaust
- [ ] **Akku mit dem Lineal nachmessen:** höchstens **95 × 58 × 12 mm**. Nicht der
      Artikelbeschreibung glauben
- [ ] **Probestapeln ohne Schrauben:** Akku + Zwischenboden + Laderegler in die Box legen,
      Deckel auflegen. Schließt er spannungsfrei? (36 von 37 mm — es ist knapp)
- [ ] USB-Kabel A→C: **90°-Winkelstecker** und trotzdem ein **Datenkabel**?
- [ ] **Klardeckel auf IR-Durchlass prüfen:** IR-LED dahinter halten, mit der
      Handy-Frontkamera durchschauen. Leuchtet sie klar durch?
- [ ] Reedschalter: Magnet nähern — piept der Durchgangsprüfer?

---

← Zurück zur [Übersicht](../README.md) · [3. Schaltplan](03-schaltplan.md) · [5. Software](05-software.md)
