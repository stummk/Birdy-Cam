# 9. Bestellliste — Variante B (ESP32-S3)

Zum Ausdrucken und Abhaken. Nach **Shop gruppiert**, damit man Versandkosten spart.

> ⚠️ **Ehrliche Vorwarnung zur Summe.** In [2. Stückliste](02-stueckliste.md) steht
> „≈ 149 €". Das war eine **Bauteilkosten-Schätzung** mit Einzelpreisen. An der Kasse wird
> es mehr, aus drei Gründen:
>
> 1. **Amazon verkauft Multipacks.** Ein einzelnes MOSFET-Modul gibt es praktisch nicht —
>    es kommen 3 oder 5. Das ist kein Nachteil (Ersatz liegt im Schrank), kostet aber mehr.
> 2. **Versand über fünf Shops** — realistisch 15–25 €.
> 3. **Kleinteile**, die in der Schätzung untergingen: USB-Kabel, Klettband, Stiftleiste.
>
> **Realistisch an der Kasse: 175–195 €.** Mit der Sparliste in
> [9.7](#97-sparliste--wenn-es-knapper-sein-soll) sind ~150 € erreichbar, wenn Werkzeug und
> Kleinteile schon im Haus sind.

---

## 9.0 ⚠️ Die eine Entscheidung, die du vorher treffen musst

Es gibt zwei Kameramodule, und sie unterscheiden sich weniger, als der Name vermuten lässt:

| | **OV2640 „Night Vision"** ⭐ | OV5640 ohne IR-Filter |
|---|---|---|
| Max. Auflösung | 1600×1200 (UXGA) | 1920×1080 (Full HD) |
| **Megapixel** | **1,92** | **2,07** |
| Bildformat | 4:3 | 16:9 |
| Bilder pro Sekunde | **8–12** | ~6 |
| Lieferbarkeit „ohne IR-Filter" | ✅ **bestätigt lieferbar** | 🟡 schwer zu finden |
| Vom XIAO Sense nativ unterstützt | ✅ ja | ja, Treiber umstellen |
| Preis | ~10 € | ~13 € |

**Der entscheidende Punkt:** 1,92 gegen 2,07 Megapixel — das sind **7 % Unterschied**. Auf
dem Bild sieht man das nicht. Was man sieht, ist die doppelte Bildrate.

> **Meine Empfehlung: OV2640 „Night Vision" + `FRAMESIZE_UXGA`.** Praktisch dieselbe
> Detailmenge wie Full HD, flüssigeres Video, sicher lieferbar. „Full HD" im Sinne von
> 1920×1080 bekommst du damit nicht — an Bildqualität verlierst du aber nichts.
>
> Wenn dir das 16:9-Format wichtig ist (weil es am Handy und Fernseher formatfüllend ist),
> nimm den OV5640 und stelle `FRAMESIZE_FHD` ein. Beides ist in der Firmware vorbereitet.

Die Firmware ist auf **UXGA** voreingestellt. Eine Zeile in
[`config.h`](../software/firmware/birdycam/config.h) ändert das.

---

## 9.0b ⚠️ Noch eine Entscheidung vor dem Panel-Kauf

Die Kamera kann auf zwei Wegen erreichbar sein — und das bestimmt die Panelgröße:

| | **Router-Betrieb** | **Eigenes WLAN (autark)** |
|---|---|---|
| Wie du zugreifst | Vom Sofa über dein Heimnetz | Handy verbindet sich direkt mit der Kamera |
| Braucht WLAN am Kasten | ✅ ja | ❌ nein |
| Verbrauch | 15 Wh/Tag | **21 Wh/Tag (+40 %)** |
| **Panel** | **10 W** (20 €) | **15–20 W** (28 €) |
| Uhrzeit | automatisch aus dem Internet | erster Website-Besuch stellt sie |

**Warum das eigene WLAN mehr Strom braucht:** Ein WLAN-Sender muss ständig Funkbaken
aussenden, damit Handys ihn finden — er darf nicht schlafen. Im Routerbetrieb darf der
ESP32 zwischen zwei Funkkontakten dösen.

> **Mein Rat: `NETZ_MODUS = NETZ_AUTO` und ein 15-W-Panel.** Dann läuft sie normalerweise
> sparsam am Router, macht aber automatisch ihr eigenes WLAN auf, wenn der Router nicht
> erreichbar ist — und das Panel trägt beide Fälle. Die 8 € Aufpreis sind gut angelegt.
>
> Wenn am Nistkasten sicher kein WLAN-Empfang ist: `NETZ_EIGENES` und **20 W**.

Details in [8.2c](08-variante-esp32.md#82c-router-oder-eigenes-wlan--beides-geht).

---

## 9.1 Sammelbestellung A — BerryBase

🔗 [berrybase.de](https://www.berrybase.de) · Versandkostenfrei ab 29 €

| ✓ | Anz. | Teil | Achtung! | Preis | Link |
|---|---|---|---|---|---|
| ✓ | 1 | **Seeed XIAO ESP32-S3 Sense** | Muss **„Sense"** heißen — nur die hat Kamera-Anschluss und SD-Slot | 25 € | [Produktseite](https://www.berrybase.de/en/seeed-xiao-esp32s3-sense-esp32-s3r8-wlan-ble-5.0-ov2640-camera-sensor-8mb-psram-8mb-flash) |
| X | 1 | Stiftleiste 2×7, 2,54 mm | **Nur nötig, wenn das Board ohne angelötete Pins kommt.** Vorher in der Artikelbeschreibung prüfen | 1 € | [Suche](https://www.berrybase.de/search?sSearch=Stiftleiste+2%2C54) |
|  ✓ | 1 | *(optional)* IR-Hindernissensor mit Digitalausgang | 3,3–5 V, Reichweite 2–40 cm, mit Potentiometer. Siehe [9.5](#95-optional--die-lichtschranke) | 3 € | [Produktseite](https://www.berrybase.de/bauelemente/sensoren-module/bewegung-distanz/ir-hindernissensor/lichtschranke-mit-digitalem-ausgang) |
| | | | | **~29 €** | *versandfrei* |

> Alternative zu BerryBase für das Board: [Reichelt](https://www.reichelt.com/de/en/shop/product/xiao_esp32s3_sense_wifi_bt_camera_ov3660_without_header-358353)
> (dort **„without header"** — dann Stiftleiste zwingend mitbestellen).

---

## 9.2 Sammelbestellung B — eBay / AliExpress ⏰ **zuerst bestellen!**

Diese Teile kommen oft aus Asien und brauchen **2–4 Wochen**. Deshalb als Erstes ordern.

| ✓ | Anz. | Teil | Achtung! | Preis | Link |
|---|---|---|---|---|---|
|  ✓ | **2** | **Kameramodul 24-pin DVP, ohne IR-Filter** | Im Angebot muss **„no IR filter"**, „850nm" oder **„night vision"** stehen. **Zwei Stück** — eins ist Ersatz, hier geht am ehesten etwas kaputt. Weitwinkel (120–160°) bevorzugen | 2× 10 € | [OV2640 Night Vision ⭐](https://esp32s.com/product/24pin-ov2640-camera-module-for-esp32-cam-camera-module-2mp-180-66-120-160-222-200-degree-650nm-850nm-night-vision-dvp/) · [eBay-Suche OV2640](https://www.ebay.de/sch/i.html?_nkw=OV2640+24pin+night+vision+no+IR+filter) · [eBay-Suche OV5640](https://www.ebay.de/sch/i.html?_nkw=OV5640+24pin+DVP+no+IR+filter) |
| X | 1 | **FPC-Verlängerung 24-pol, 0,5 mm, 10 cm** | **24-polig** und **0,5 mm Raster**. Nicht länger als 15 cm — sonst Bildrauschen | 4 € | [AliExpress-Suche](https://de.aliexpress.com/w/wholesale-24pin-0.5mm-FPC-extension-cable.html) |
| | | | | **~24 €** | + Versand |

> **Frage an den Verkäufer, wenn unklar:** „Is the IR filter removed?" Ein Modul *mit*
> Filter ist nachts blind, egal wie stark die IR-LEDs sind.

---

## 9.3 Sammelbestellung C — Amazon

| ✓ | Anz. | Teil | Achtung! | Preis | Link |
|---|---|---|---|---|---|
|  ✓ | 1 | **microSD 32 GB High Endurance** | **Max. 32 GB** (Board-Grenze) und **„High Endurance"** / „Dashcam" | 12 € | [Suche](https://www.amazon.de/s?k=microSD+32GB+High+Endurance) |
| ☐ | 1 | **Solarpanel 6 V**, wetterfest — **10 W** *oder* **15–20 W** | ⚠️ **6 Volt!** Ein 12-V-Panel zerstört den Laderegler. **Größe hängt von der Betriebsart ab — siehe [9.0b](#90b-️-noch-eine-entscheidung-vor-dem-panel-kauf)** | 20–28 € | [Suche](https://www.amazon.de/s?k=Solarpanel+6V+10W+wetterfest) · [eBay](https://www.ebay.de/sch/i.html?_nkw=solarpanel+6v+10w) |
| ☐ | 1 | Spannungssensor-Modul 0–25 V | Kommt meist im 5er-Pack | 6 € | [Suche](https://www.amazon.de/s?k=Spannungssensor+Modul+25V+Arduino) |
| ☐ | 4 | **IR-LED-Module 940 nm** | ⚠️ **940 nm**, nicht 850 nm (das glimmt rot) | 7 € | [Suche](https://www.amazon.de/s?k=IR+LED+Modul+940nm+Arduino) |
| ☐ | 1 | **MOSFET-Modul, Logic-Level** (D4184/AOD4184) | ⚠️ **Kein IRF520!** Der schaltet bei 3,3 V nicht durch. Meist 3er-Pack | 8 € | [Suche](https://www.amazon.de/s?k=MOSFET+Modul+D4184+PWM) |
| ☐ | 1 | Dupont-Steckkabel-Set (F-F und F-M) | 10–20 cm Länge reicht | 6 € | [Suche](https://www.amazon.de/s?k=Dupont+Kabel+Set+female+female) |
| ☐ | 1 | Acrylglas 3 mm, ~10×10 cm | **Kein Glas** — Acryl lässt Infrarot durch | 6 € | [Suche](https://www.amazon.de/s?k=Acrylglas+3mm+Platte) |
| ☐ | 1 | Silikagel-Beutel | Gegen beschlagene Linse | 5 € | [Suche](https://www.amazon.de/s?k=Silikagel+Beutel+Trockenmittel) |
| ☐ | 1 | USB-A → USB-C Kabel, 30 cm | Verbindet Laderegler und Board | 5 € | oft im Haus |
| ☐ | 1 | Klettband (Rolle) | Für den Akku — **nicht** Kabelbinder | 4 € | [Suche](https://www.amazon.de/s?k=Klettband+Rolle) |
| | | | | **~79 €** | *versandfrei ab 39 €* |

---

## 9.4 Einzelbestellungen

| ✓ | Anz. | Teil | Achtung! | Preis | Shop |
|---|---|---|---|---|---|
|  ✓ | 1 | **Solar Power Manager 5 V** (DFRobot DFR0559 o. gleichwertig) | MPPT + Laderegler + 5-V-Ausgang in einem, komplett lötfrei. Eingang **5–7 V** | 10 € | [DFRobot](https://www.dfrobot.com/product-1712.html) · [Datenblatt](https://wiki.dfrobot.com/dfr0559/) · [eBay-Suche](https://www.ebay.de/sch/i.html?_nkw=solar+power+manager+5V+MPPT+lipo) |
|  ✓ | 1 | **LiPo-Akku 1S, 5000 mAh**, mit Schutzschaltung + JST-PH 2.0 | ⚠️ **Mit Schutzschaltung** ist Pflicht. Steckertyp prüfen! 10000 mAh = +12 €, doppelte Reserve | 14 € | [Pollin](https://www.pollin.de/stromversorgung/akkus/lipo-akkus/) · [lithium-polymer-akkus.de](https://www.lithium-polymer-akkus.de/lipo-akku-pack-25mah-to-10000mah/) |
| ✓ | 1 | **IP65-Gehäuse ca. 120×80×50 mm** | Muss Board, Laderegler und Akku fassen | 11 € | [Reichelt](https://www.reichelt.de/index.html?ACTION=446&LA=446&nbc=1&q=Installationsgeh%C3%A4use+IP65) |
| ✓ | 3 | Kabelverschraubung M12 | Für Panelkabel, Kamerakabel, IR-Kabel | 5 € | [Reichelt](https://www.reichelt.de/index.html?ACTION=446&LA=446&nbc=1&q=Kabelverschraubung+M12) |
| | | | | **~40 €** | + Versand |

---

## 9.5 Optional — die Lichtschranke

Sie zählt Ein- und Ausflüge exakt und liefert die Aufenthaltsdauer. **Die Firmware läuft
auch ohne sie** — dann übernimmt die Bilderkennung das Auslösen (`LICHTSCHRANKE_AN false`).

**Warum optional und nicht Pflicht:** Ich konnte kein gut lieferbares
Sender/Empfänger-Paar bestätigen. Der verfügbare Baustein ist ein **Reflex-Sensor**
(FC-51-Bauform): IR-LED und Empfänger sitzen nebeneinander auf einem Board, es wird die
Reflexion am Vogel gemessen. Das funktioniert, hat aber zwei Schwächen:

- **Dunkle Vögel reflektieren schwächer** als helle → Empfindlichkeit am Potentiometer
  einstellen
- **Direkte Sonne kann blenden** → kleines Röhrchen (Strohhalm) als Blende über den Sensor

| ✓ | Teil | Preis | Link |
|---|---|---|---|
| ✓ | IR-Hindernissensor mit Digitalausgang, 3,3 V | 3 € | [BerryBase](https://www.berrybase.de/bauelemente/sensoren-module/bewegung-distanz/ir-hindernissensor/lichtschranke-mit-digitalem-ausgang) |
| X | *Alternative:* Lichtschranken-Paar Sender/Empfänger | 6 € | [eBay-Suche](https://www.ebay.de/sch/i.html?_nkw=Infrarot+Lichtschranke+Modul+Sender+Empf%C3%A4nger) |

Beide liefern ein digitales Signal — die Firmware kommt mit beiden zurecht. Justage in
[Sketch 7](../software/firmware/steps/step7_lichtschranke/step7_lichtschranke.ino).

---

## 9.6 Werkzeug — falls nicht im Haus

| ✓ | Werkzeug | Preis | Wofür |
|---|---|---|---|
| ☐ | Heißklebepistole + Sticks | 12 € | Kamera, LEDs, Abdichtung |
| ☐ | Akkuschrauber + Holzbohrer 3/5/12 mm | 40 € | Löcher im Deckel |
| ☐ | Multimeter | 20 € | Akku kalibrieren (Sketch 5), Fehlersuche |
| ☐ | Seitenschneider, Abisolierzange | 15 € | Kabel |
| ☐ | Lötkolben-Set | 25 € | **nur** falls das Board ohne Pins kommt |

Das Multimeter ist die sinnvollste Investition davon — es macht die Kalibrierung in
[Sketch 5](../software/firmware/steps/step5_akku/step5_akku.ino) exakt statt geschätzt, und
es bleibt für alle künftigen Projekte nützlich.

---

## 9.7 Sparliste — wenn es knapper sein soll

| Weglassen / ersetzen | Ersparnis | Was es kostet |
|---|---|---|
| Zweites Kameramodul | 10 € | Bei Defekt 2–4 Wochen Wartezeit mitten im Bau |
| Gehäuse: Vorratsdose + Silikon | 11 € | Für einen Sommer okay, über den Winter nicht |
| Acrylglas, Silikagel, Klettband, USB-Kabel aus dem Haus | 20 € | nichts, wenn vorhanden |
| Lichtschranke weglassen | 3 € | Statistik wird geschätzt statt gemessen |
| 10-W-Panel statt 15 W | 8 € | Nur wenn du **sicher** Routerbetrieb fährst ([9.0b](#90b-️-noch-eine-entscheidung-vor-dem-panel-kauf)) |
| **Zusammen** | **~52 €** | **→ ~140 € Bauteile** |

**Wo man nicht sparen sollte:** Panel (B1) und Akku — sie erfüllen „läuft autonom". Und die
High-Endurance-Karte, die „muss stabil sein" erfüllt.

---

## 9.8 Summen

| Posten | Betrag |
|---|---|
| A — BerryBase (Board, Stiftleiste) | 26 € |
| B — eBay/AliExpress (2× Kamera, FPC) | 24 € |
| C — Amazon (Karte, Panel, Module, Kleinteile) | 79 € |
| D — Einzelbestellungen (Laderegler, Akku, Gehäuse, Verschraubungen) | 40 € |
| Lichtschranke (optional) | 3 € |
| **Bauteile gesamt** | **≈ 172 €** |
| Versand (4–5 Shops) | 15–25 € |
| **Realistisch an der Kasse** | **≈ 190 €** |
| *mit Sparliste 9.7* | *≈ 150 €* |
| Werkzeug, falls komplett fehlend | + bis 110 € |

---

## 9.9 Bestellreihenfolge

Die Reihenfolge ist nicht beliebig — sie bestimmt, ob man wartet:

```
   Woche 1   ▶ eBay/AliExpress: Kameramodule + FPC-Kabel
               (2-4 Wochen Lieferzeit — die Bremse im Projekt)
             ▶ DFRobot: Solar Power Manager (auch länger unterwegs)

   Woche 1   ▶ BerryBase: Board + Stiftleiste + Lichtschranke
               (schnell da -> Sketches 1 und 2 gehen sofort los!)

   Woche 2   ▶ Amazon: alles andere
             ▶ Reichelt: Gehäuse + Verschraubungen
             ▶ Akku
```

**Der Trick:** Mit Board und SD-Karte allein kann man schon
[Sketch 1](../software/firmware/steps/step1_hallo/step1_hallo.ino) und
[Sketch 2](../software/firmware/steps/step2_sdkarte/step2_sdkarte.ino) machen. Das Kind
fängt also an, während die Kamera noch unterwegs ist — und die Vorfreude hat einen
Zwischenschritt.

---

## 9.10 Wareneingangs-Prüfung

Bitte **vor** dem Einbau prüfen, nicht danach:

- [ ] Board ist die **„Sense"**-Version (Kamera-Anschluss und SD-Slot vorhanden)
- [ ] Pins am Board angelötet? Wenn nein → Stiftleiste da?
- [ ] Kameramodul: **24-polig**, Raster **0,5 mm**
- [ ] Kameramodul: **kein IR-Filter** — Test: [Sketch 4](../software/firmware/steps/step4_irlicht/step4_irlicht.ino)
      laufen lassen, dann in [Sketch 3](../software/firmware/steps/step3_kamera/step3_kamera.ino)
      im **dunklen** Raum schauen. Sieht man etwas → Filter ist weg. ✅
- [ ] IR-LEDs: mit der **Handy-Frontkamera** prüfen, ob sie leuchten
- [ ] MOSFET-Modul: steht **D4184** oder **AOD4184** auf dem Chip? (Nicht IRF520)
- [ ] Solarpanel: Aufkleber sagt **6 V** und die Wattzahl passt zur Betriebsart
      ([9.0b](#90b-️-noch-eine-entscheidung-vor-dem-panel-kauf))
- [ ] Akku: Schutzschaltung vorhanden, Stecker passt in den Laderegler
- [ ] microSD: max. 32 GB, als FAT32 formatiert
- [ ] Laderegler: Eingang **5–7 V**, JST-Buchse für Akku, USB-Ausgang

Der vierte Punkt ist der wichtigste. **Ein Kameramodul mit IR-Filter ist nachts blind** —
und das merkt man sonst erst, wenn alles im Kasten verklebt ist.

---

← Zurück: [8. Variante ESP32](08-variante-esp32.md) · [Übersicht](../README.md)
