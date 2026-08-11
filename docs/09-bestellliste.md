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
| Verbrauch (mit Ton) | 16 Wh/Tag | **22 Wh/Tag (+40 %)** |
| **Panel** | 5-V-Kamerapanel **10 W** (~22 €) | dasselbe, **10 W** ist hier Pflicht |
| Uhrzeit | automatisch aus dem Internet | erster Website-Besuch stellt sie |

**Warum das eigene WLAN mehr Strom braucht:** Ein WLAN-Sender muss ständig Funkbaken
aussenden, damit Handys ihn finden — er darf nicht schlafen. Im Routerbetrieb darf der
ESP32 zwischen zwei Funkkontakten dösen.

> **Mein Rat: `NETZ_MODUS = NETZ_ROUTER` und ein 5-V-Panel mit 10 W.** Der Laderegler
> lädt mit höchstens 900 mA — mehr Panel kauft nur Stunden am Rand des Tages, keine
> Spitzenleistung. Und der Router-Betrieb spart 6 Wh/Tag, also mehr, als jedes
> größere Panel im Budget wieder hereinholen könnte. Warum das so ist und welches
> Panel konkret: [9.3b](#93b-️-welches-solarpanel-passt-zum-laderegler).
>
> `NETZ_AUTO` (die Werkseinstellung) verbraucht **genauso wenig**, solange der
> Router da ist — die 22 Wh gelten nur, wenn sie auf eigenes WLAN umgeschaltet
> hat. Genau das ist der unangenehme Fall: Der Router fällt aus, niemand merkt
> es, und die Kamera funkt tagelang im teuren Modus. Wer knapp kalkuliert, nimmt
> `NETZ_ROUTER` **oder** setzt zusätzlich `AP_NACHTS_AUS = true`.

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
| ☐ | 1 | **5-V-Solarpanel für Akku-Kameras, 10 W**, IP65, starr | ⚠️ **Leerlaufspannung muss unter 6,5 V bleiben** — ein „6 V"-Gartenpanel hat 7,2 V und zerstört den Laderegler. Messen! Der USB-Stecker wird abgeschnitten, das Kabel kommt in die Schraubklemme — **[9.3b](#93b-️-welches-solarpanel-passt-zum-laderegler)** | ~22 € | [Beispiel](https://www.amazon.de/Tragbares-Solarpanel-Ladeger%C3%A4t-Wasserdicht-%C3%9Cberwachungskamera/dp/B0BB79WH7K) · [Suche](https://www.amazon.de/s?k=Solarpanel+10W+5V+Micro-USB+IP65+%C3%9Cberwachungskamera) · [5-W-Sparvariante, 12,99 €](https://www.pearl.de/a-ZX5350-1322.shtml) |
| ☐ | 1 | Spannungssensor-Modul 0–25 V | Kommt meist im 5er-Pack | 6 € | [Suche](https://www.amazon.de/s?k=Spannungssensor+Modul+25V+Arduino) |
| ☐ | 4 | **IR-LED-Module 940 nm** | ⚠️ **940 nm**, nicht 850 nm (das glimmt rot) | 7 € | [Suche](https://www.amazon.de/s?k=IR+LED+Modul+940nm+Arduino) |
| ☐ | 1 | **MOSFET-Modul, Logic-Level** (D4184/AOD4184) | ⚠️ **Kein IRF520!** Der schaltet bei 3,3 V nicht durch. Meist 3er-Pack | 8 € | [Suche](https://www.amazon.de/s?k=MOSFET+Modul+D4184+PWM) |
| ☐ | 1 | Dupont-Steckkabel-Set (F-F und F-M) | 10–20 cm Länge reicht | 6 € | [Suche](https://www.amazon.de/s?k=Dupont+Kabel+Set+female+female) |
| ☐ | 1 | Acrylglas 3 mm, ~10×10 cm | **Kein Glas** — Acryl lässt Infrarot durch | 6 € | [Suche](https://www.amazon.de/s?k=Acrylglas+3mm+Platte) |
| ☐ | 1 | Silikagel-Beutel | Gegen beschlagene Linse | 5 € | [Suche](https://www.amazon.de/s?k=Silikagel+Beutel+Trockenmittel) |
| ☐ | 1 | USB-A → USB-C Kabel, 30 cm | Verbindet Laderegler und Board | 5 € | oft im Haus |
| ☐ | 1 | Klettband (Rolle) | Für den Akku — **nicht** Kabelbinder | 4 € | [Suche](https://www.amazon.de/s?k=Klettband+Rolle) |
| | | | | **~81 €** | *versandfrei ab 39 €* |

---

## 9.3b ⚠️ Welches Solarpanel passt zum Laderegler?

**In einem Satz: Nicht die Wattzahl entscheidet, sondern die Leerlaufspannung — und
die muss unter 6,5 V bleiben.**

### Der Satz, der den Laderegler rettet

DFRobot schreibt im [Wiki zum DFR0559](https://wiki.dfrobot.com/Solar_Power_Manager_5V_SKU__DFR0559),
dass jedes Panel geht, **solange die Leerlaufspannung unter 6,5 V liegt** — und
dazu: *„The input voltage of SOLAR IN should not exceed 6.5V, or the module may be
permanently damaged."* Das Datenblatt des verbauten Chips CN3165 nennt als
absolute Obergrenze für **alle** Anschlüsse −0,3 V bis 6,5 V.

Und jetzt der Haken: Auf einem Panel steht „6 V" — das ist die **Arbeitsspannung
unter Last**. Die **Leerlaufspannung** solcher Panels liegt bei **7,2–7,9 V**. Das
ist kein Ausreißer, das ist die Regel: Selbst das Waveshare-Panel mit dem Aufdruck
„5,5 V / 6 W" hat laut Datenblatt **7,2 V Leerlaufspannung**.

Leerlauf heißt: Es fließt kein Strom. Genau das passiert jeden Tag — sobald der
Akku voll ist, hört der Regler auf zu laden, und am Eingang steht die volle
Leerlaufspannung an. Kalte, klare Tage machen es schlimmer, denn kalte Zellen
liefern **mehr** Spannung.

> ⚠️ **Deshalb ist meine frühere Panelempfehlung hinfällig.** Die dort genannten
> 6-V-Panels (Villageboom 10 W, zwei davon parallel) liegen mit ~7,2 V darüber,
> und selbst der Voltaic P120 mit **6,61 V** liegt über der Grenze.
> **Nicht kaufen, nicht anschließen.**

| | Leerlaufspannung | Am DFR0559 |
|---|---|---|
| Panel mit Aufdruck „6 V" | 7,2–7,9 V | ❌ zerstört das Modul |
| Panel mit Aufdruck „5,5 V" (z. B. Waveshare 6 W) | 7,2 V | ❌ zerstört das Modul |
| Voltaic P120 „6 V / 20 W" | 6,61 V | ❌ knapp darüber |
| **5-V-Panel für Akku-Kameras (USB)** | meist **~5,0–5,5 V** | ✅ passt — trotzdem messen |
| 12-V-Panel | 20–24 V | ❌❌ sofortiger Totalschaden |

### Was stattdessen passt: ein 5-V-Kamerapanel

Es gibt eine Panelklasse, die genau diese Bedingung von Haus aus erfüllt:
**wetterfeste 5-V-Panels für Akku-Überwachungskameras.** Sie sind gebaut, um über
USB in eine Kamera zu laden — und ein USB-Anschluss würde 7 V nicht überleben.
Ihre Ausgangsspannung bleibt deshalb bei ~5 V. Nebenbei sind sie IP65, haben ein
3-m-Kabel und eine verstellbare Halterung, sind also für draußen gedacht statt
nur dafür geeignet.

| | **Standard: 5-V-Kamerapanel mit 10 W** |
|---|---|
| Beschreibung | Starres Panel für Akku-Überwachungskameras, **10 W / 5 V**, Micro-USB-Kabel (~3 m), **IP65**, 360°-Halterung dabei |
| Preis | **~20–25 €** |
| Wo | [Beispiel bei Amazon.de](https://www.amazon.de/Tragbares-Solarpanel-Ladeger%C3%A4t-Wasserdicht-%C3%9Cberwachungskamera/dp/B0BB79WH7K) · [Suche nach gleichwertigen](https://www.amazon.de/s?k=Solarpanel+10W+5V+Micro-USB+IP65+%C3%9Cberwachungskamera) |

⚠️ Den Preis bitte selbst nachsehen: Amazon lässt sich nicht automatisch abfragen,
die 20–25 € sind der Marktpreis dieser Klasse und keine geprüfte Angabe.

**Sparvariante, wenn 10 € zählen:** dasselbe mit **5 W** — der
[revolt ZX5350](https://www.pearl.de/a-ZX5350-1322.shtml) für **12,99 €**
(monokristallin, Micro-USB, IP65, 3 m Kabel, 17 × 16 cm, 288 g, Halterung dabei;
Preis geprüft). Reicht ab April; im März und bei Bewölkung fehlt Reserve.

> **Nicht nehmen: faltbare Panels.** Pearl hat ein 10-W-Panel für 20,99 €
> ([ZX7384](https://www.pearl.de/a-ZX7384-3034.shtml)), das aber aus vier Modulen
> in einer Stofftasche besteht — gedacht für den Rucksack, nicht für ein Jahr am
> Pfosten. Für draußen ein starres Panel nehmen.

**Mehr als 10 W ist rausgeworfenes Geld** — warum, steht gleich darunter.

### Das Panel hat ein USB-Kabel — passt das an den Regler?

**Ja. Der Weg führt aber nicht über den Stecker, sondern über die Schraubklemme.**

Der DFR0559 hat zwei Eingänge, und beide laden mit denselben 900 mA:

| Eingang | Wofür | Für unser Panel |
|---|---|---|
| **SOLAR IN** (Schraubklemme + / −) | Solarpanel | ✅ **der Weg** |
| **USB IN** (Buchse) | Netzteil oder Powerbank | nur, wenn der Steckertyp zufällig passt |

Drei Gründe für die Klemme:

1. **Der Stecker passt nicht durch die Kabelverschraubung.** Eine M12-Verschraubung
   klemmt Kabel von 3–6,5 mm; ein Micro-USB-Stecker ist mit Umspritzung rund
   10 × 6 mm. Das Kabel muss also ohnehin ohne Stecker durch die Box-Wand.
2. **Der Steckertyp des USB-Eingangs ist nirgends dokumentiert.** Ich habe Wiki,
   Produktseite und drei Distributoren-Datenblätter durchgesehen — überall steht
   nur „USB IN 5 V". Beim Auspacken sieht man es, verlassen würde ich mich nicht
   darauf.
3. **5 V sind für SOLAR IN genau richtig.** Das erlaubte Fenster ist 4,5–6 V, und
   die MPPT-Regelung des Reglers zieht Strom, bis die Spannung einbricht — bei
   einem geregelten Panel passiert das erst an dessen Strombegrenzung. Genau das
   soll es tun.

**So wird es gemacht:**

```
   Panel  ──── 3 m Kabel ────  ✂  ──── M12-Verschraubung ────  SOLAR IN
                            Stecker ab                         + / −
```

1. **Vorher messen** (volle Sonne, nichts angeschlossen): am Stecker müssen
   ~5,0–5,5 V stehen. Über 6,5 V → Finger weg.
2. Stecker abschneiden, die zwei Adern abisolieren.
3. **Nochmal messen — das ist der Punkt, den man leicht übersieht.** Sitzt die
   5-V-Elektronik ausnahmsweise *im Stecker* statt im Panel, springt die Spannung
   jetzt auf 6–7 V hoch. Dann das Kabel nicht verwenden, sondern den USB-Weg
   nehmen (falls der Stecker passt) oder das Panel zurückschicken.
4. Polarität notieren (meist rot = Plus, aber messen), Kabel durch die
   Verschraubung führen, in die Klemme schrauben, **Tropfschlaufe** legen
   ([4.5](04-bauplan.md#45-die-elektronikbox)).
5. Website öffnen: Bei Sonne muss die Akkukarte „⬆ steigt" zeigen.

> **Die USB-Buchse bleibt trotzdem nützlich:** Dort passt in einer Regenwoche
> eine Powerbank hinein und lädt den Akku mit denselben 900 mA — ohne die Box zu
> öffnen, wenn ein kurzes Kabel gleich beim Bauen nach außen gelegt wird.

### Warum mehr Watt fast nichts bringt

Der DFR0559 lädt mit **höchstens 900 mA**. Das sind ~3,5 W in den Akku, egal wie
groß das Panel ist. Die Panelgröße entscheidet deshalb nicht über die
Spitzenleistung, sondern nur darüber, **wie viele Stunden am Tag diese 900 mA
überhaupt erreicht werden**:

```
   5-W-Panel  (1,0 A bei voller Sonne)  -> 900 mA nur um die Mittagszeit
  10-W-Panel  (2,0 A bei voller Sonne)  -> 900 mA schon ab ~45 % Sonne,
                                            also fast den ganzen hellen Tag
  20-W-Panel                            -> derselbe Deckel, doppelter Preis
```

Gebraucht werden bei 16 Wh/Tag (Routerbetrieb, mit Ton) rund **4,7 Ah**, also
**gut 5 Stunden bei vollen 900 mA**. Im Mai schafft das auch das 5-W-Panel; im
März und bei Bewölkung ist das 10-W-Panel im Vorteil. Beides bleibt unter 30 €.

### Die Hebel, die mehr bringen als jedes Panel — und 0 € kosten

| Maßnahme | Ersparnis | Preis |
|---|---|---|
| `NETZ_MODUS NETZ_ROUTER` statt `NETZ_AUTO` | 22 → **16 Wh/Tag** | 0 € |
| `AP_NACHTS_AUS true` (falls doch eigenes WLAN) | −2,5 Wh/Tag | 0 € |
| `NACHT_PAUSE_MS 1800` statt 900 | ~−1 Wh/Tag | 0 € |
| `IR_HELLIGKEIT 50` statt 75 | ~−0,5 Wh/Tag | 0 € |
| `AUDIO_AN false` (letzte Reserve) | −1 Wh/Tag | kostet den Ton |
| **Powerbank an den Laderegler**, wenn eine Regenwoche kommt | füllt den Akku ganz | 0 €, falls im Haus |

Der letzte Punkt ist die eigentliche Antwort auf „was, wenn der März grau ist":
Der DFR0559 hat neben dem Solareingang einen **USB-Eingang**. Eine Powerbank dort
angesteckt lädt den Akku mit denselben 900 mA — ohne die Box zu öffnen, wenn man
das Kabel gleich beim Bauen nach außen führt.

> **Und wenn das Projekt später wächst:** Der 900-mA-Deckel und die 6,5-V-Grenze
> gehören zum DFR0559, nicht zum Aufbau. Ein Laderegler mit 7–30 V Eingang und
> 2 A Ladestrom (z. B. [DFRobot DFR0535](https://www.dfrobot.com/product-1714.html))
> nimmt normale 12-V-Panels und lädt doppelt so schnell. Das ist ein Umbau für
> einen späteren Winter, kein Muss für diese Saison.

---

## 9.4 Einzelbestellungen

| ✓ | Anz. | Teil | Achtung! | Preis | Shop |
|---|---|---|---|---|---|
|  ✓ | 1 | **Solar Power Manager 5 V** (DFRobot DFR0559 o. gleichwertig) | MPPT + Laderegler + 5-V-Ausgang in einem, komplett lötfrei. Eingang **4,5–6 V**, Ladestrom **max. 900 mA**, Panel **≤ 10 W** — die Grenzen erklärt [9.3b](#93b-️-welches-solarpanel-passt-zum-laderegler) | 10 € | [DFRobot](https://www.dfrobot.com/product-1712.html) · [Datenblatt](https://wiki.dfrobot.com/dfr0559/) · [eBay-Suche](https://www.ebay.de/sch/i.html?_nkw=solar+power+manager+5V+MPPT+lipo) |
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
| 5-W-Panel statt 10 W | 9 € | Reicht ab April; im März und bei Bewölkung wird es knapp ([9.3b](#93b-️-welches-solarpanel-passt-zum-laderegler)) |
| **Zusammen** | **~52 €** | **→ ~140 € Bauteile** |

**Wo man nicht sparen sollte:** Panel (B1) und Akku — sie erfüllen „läuft autonom". Und die
High-Endurance-Karte, die „muss stabil sein" erfüllt.

---

## 9.8 Summen

| Posten | Betrag |
|---|---|
| A — BerryBase (Board, Stiftleiste) | 26 € |
| B — eBay/AliExpress (2× Kamera, FPC) | 24 € |
| C — Amazon (Karte, Panel, Module, Kleinteile) | 81 € |
| D — Einzelbestellungen (Laderegler, Akku, Gehäuse, Verschraubungen) | 40 € |
| Lichtschranke (optional) | 3 € |
| **Bauteile gesamt** | **≈ 174 €** |
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
- [ ] **Solarpanel in der Sonne mit dem Multimeter gemessen: unter 6,5 V?**
      Das ist die wichtigste Messung der ganzen Liste — darüber stirbt der
      Laderegler ([9.3b](#93b-️-welches-solarpanel-passt-zum-laderegler))
- [ ] Polarität des Panelkabels notiert (welche Ader ist Plus)
- [ ] **Nach dem Abschneiden des USB-Steckers nochmal gemessen** — springt die
      Spannung jetzt über 6,5 V, saß die Regelung im Stecker ([9.3b](#93b-️-welches-solarpanel-passt-zum-laderegler))
- [ ] Akku: Schutzschaltung vorhanden, Stecker passt in den Laderegler
- [ ] microSD: max. 32 GB, als FAT32 formatiert
- [ ] Laderegler: Eingang **4,5–6 V**, JST-Buchse für Akku, USB-Ausgang

Der vierte Punkt ist der wichtigste. **Ein Kameramodul mit IR-Filter ist nachts blind** —
und das merkt man sonst erst, wenn alles im Kasten verklebt ist.

---

← Zurück: [8. Variante ESP32](08-variante-esp32.md) · [Übersicht](../README.md)
