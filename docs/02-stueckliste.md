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
| **E8b** | **JST-PH-2.0-Y-Kabel** (1 Buchse → 2 Stecker) | Damit Laderegler **und** Spannungssensor gleichzeitig am Akku hängen, ohne zu löten. Gibt es im 5er-Pack. [Warum](03-schaltplan.md#38-der-spannungssensor--damit-du-den-akkustand-siehst) | 3 € |
| **E9** | **4 × IR-LED-Modul 940 nm** | Das unsichtbare Nachtlicht. ⚠️ **940 nm**, nicht 850 nm — letzteres glimmt für Menschen sichtbar rot | 7 € |
| **E10** | **MOSFET-Modul, Logic Level** (D4184 / AOD4184) | Der elektronische Lichtschalter für die LEDs. ⚠️ **Kein IRF520!** Der schaltet bei 3,3 V nicht durch | 4 € |
| **E11** | **IR-Lichtschranke** mit Digitalausgang, 3,3 V | Zählt Ein- und Ausflüge exakt. Optional, aber das schönste Bauteil im Projekt ([1.3](01-ueberblick.md#13-die-lichtschranke--die-beste-idee-im-ganzen-plan)) | 3 € |
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

## 2.2 Die drei Teile, bei denen man aufpassen muss

Wenn du dir nur drei Dinge aus diesem Kapitel merkst, dann diese.

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

### ③ Das Panel muss unter 24 Volt Leerlaufspannung haben

Auf dem Aufkleber hinten am Panel steht eine Zeile `Voc` oder „Open Circuit Voltage“ oder
„Leerlaufspannung“. Dieser Wert muss **unter 24 V** liegen.

Bei einem 12-V-Panel wie dem HPLive stehen dort typisch **18 bis 22 Volt** — das passt
bequem. Nur wenn dort mehr steht (etwa bei einem 24-V-Panel), gehört ein anderes her.

> 💡 **Das ist der Vorteil des Waveshare-Ladereglers:** Er nimmt alles zwischen 6 und
> 24 Volt und rechnet selbst herunter. Du musst bei der Panelauswahl also nicht auf
> Zehntelvolt achten — nur nach oben ist eine Grenze.

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
| **+12 €** Akku 10 000 mAh statt 5 000 | Doppelte Reserve. Der lohnendste Aufpreis der Liste |
| +5 € DS18B20 Temperaturfühler (wasserdicht) | Nesttemperatur auf der Website. Am Verlauf erkennt man, ob gebrütet wird — der Vogel *heizt*. Für 5 € das lehrreichste Extra |
| +3 € zwei IR-LEDs mehr (6 statt 4) | Helleres Nachtbild, falls 4 nicht reichen |
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
| Multimeter | Akku kalibrieren, Panel prüfen | empfohlen |
| Lötkolben | **nur** falls das Board ohne angelötete Pins kommt | meist nein |

---

→ Weiter mit [3. Schaltplan](03-schaltplan.md) · Zum Bestellen: [8. Bestellliste](08-bestellliste.md)
