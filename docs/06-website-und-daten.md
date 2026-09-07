# 6. Die Website & die Daten

Die Website läuft **auf dem Bastelcomputer selbst**. Kein Server, keine Cloud, kein Abo,
keine App. Du tippst `http://birdycam.local/` in den Browser, und da ist sie.

---

## 6.1 Was die Website zeigt

```
┌──────────────────────────────────────────────────────────┐
│ 🐦 BirdyCam  ☀️Tag  ⏺aufnahmebereit  🔋78%⚡  Kasten leer │
├──────────────────────────────────────────────────────────┤
│ LIVEBILD                                                 │
│ ┌──────────────────────────────────────────────────────┐ │
│ │             [ Livestream aus dem Kasten ]            │ │
│ └──────────────────────────────────────────────────────┘ │
│ (🔊 Ton an)                                              │
├──────────────────────────────────────────────────────────┤
│ HEUTE                                                    │
│ ┌────┐┌────┐┌─────┐┌─────┐                               │
│ │ 47 ││1382││ 5:41││20:12│                               │
│ └────┘└────┘└─────┘└─────┘                               │
│ Besuche  Sek. drin  erster  letzter                      │
├──────────────────────────────────────────────────────────┤
│ WANN IST RUSHHOUR?   (Heute)( Ø letzte Tage )            │
│     █ █       █                                          │
│   █ █ █ █   █ █ █     █                                  │
│ ░ █ █ █ █ █ █ █ █ █ █ █ ░ ░                              │
│ 0     6     12    18                                     │
├──────────────────────────────────────────────────────────┤
│ VERLAUF — LETZTE 30 TAGE                                 │
│         █     █ █   █                                    │
│   █ █ ░ █ █ ░ █ █ █ █ █ ░ █ █ █ ░ █ █ █ █ █ █ ░ █ █ █    │
│ 18.03.    23.03.    28.03.    02.04.    07.04.           │
│ Ø 41,3 Besuche am Tag · bester Tag 16.04. mit 78         │
│ tage.csv herunterladen                                   │
├──────────────────────────────────────────────────────────┤
│ AKKU                                                     │
│ [████████████████░░░░]  78 % (3.92 V)                    │
│ ⬆ steigt — die Sonne lädt                                │
├──────────────────────────────────────────────────────────┤
│ SYSTEMZUSTAND                                            │
│ ┌──────────────────────────────────────────────────────┐ │
│ │ ✓ Alles in Ordnung                                   │ │
│ └──────────────────────────────────────────────────────┘ │
│ ┌────────┐┌────────┐┌────────┐┌────────┐                 │
│ │gut     ││  42 °C ││ 121 KB ││ 3.4 MB │                 │
│ │(-61dBm)││  Chip  ││  Heap  ││ PSRAM  │                 │
│ └────────┘└────────┘└────────┘└────────┘                 │
│ SD-Karte schreibt 1,05 MB/s · reicht aus ✓               │
│ ▸ Technische Details                                     │
├──────────────────────────────────────────────────────────┤
│ AUFNAHMEN   (Videoclips)( Fotos )( Vogelgesang )         │
│  ▶ clip_047.avi  (Bild+Ton)  5.1 MB · 16.04. 06:12       │
└──────────────────────────────────────────────────────────┘
```

### Die drei Zahlen, an denen man am meisten sieht

| Anzeige | Warum sie interessant ist |
|---|---|
| **Besuche heute** | Steigt sprunghaft, sobald die Küken geschlüpft sind — die Eltern fliegen dann alle paar Minuten. **An dieser Kurve sieht man den Schlupftag.** |
| **Sekunden drin** | Beim Brüten sitzt der Vogel lange drin, beim Füttern nur Sekunden. Ein sehr direkter Blick auf die Brutphase |
| **Erster / letzter Anflug** | Wandert über die Wochen mit dem Sonnenaufgang mit. Eine echte kleine Naturbeobachtung |

### Der Ton im Livestream

Unter dem Livebild sitzt ein Knopf **🔊 Ton an**. Den musst du **einmal antippen** — Browser
verbieten, dass Webseiten von selbst losdudeln, und das ist auch gut so.

Zwei Dinge dazu, die keine Fehler sind, sondern Eigenschaften des Webs:

- **Der Ton läuft dem Bild etwa eine Sekunde nach**, weil der Browser ihn puffert. Für
  „was ist da los?“ reicht das völlig. **Im Clip ist er synchron.**
- **Auf iPhone und iPad kann der Tonstrom streiken.** Safari stellt an Audiodateien
  Anforderungen, die ein endloser Strom nicht erfüllen kann. Ausweg: den **Clip
  herunterladen** (da ist der Ton drin, und er läuft überall) oder
  `http://birdycam.local:82/` in **VLC für iOS** öffnen. Chrome, Firefox und Edge auf
  Android, Windows und Mac spielen ihn problemlos.

---

## 6.2 Die Systemzustand-Karte

Ab März hängt der Kasten unerreichbar im Garten. Du kannst dann **nichts** mehr nachschauen
— außer über die Website. Also zeigt sie alles, was man zur Beurteilung braucht.

### Oben eine Ampel mit Klartext

Sie wird nach diesen Regeln gebildet:

| Prüfung | ⚠️ Achtung | ✕ Problem |
|---|---|---|
| Akkuspannung | unter 3,60 V | nahe der Notaus-Schwelle |
| SD-Karte | Bilder verworfen (zu langsam) | keine Karte erkannt |
| Arbeitsspeicher | war schon unter 35 KB | jetzt unter 25 KB |
| WLAN | Signal unter −80 dBm | — |
| Chip-Temperatur | über 65 °C | über 80 °C |
| Kamerafehler | mehr als 50 | — |
| Letzter Neustart | Absturz oder Spannungseinbruch | — |

### Vier Kacheln mit den Werten, die am meisten erklären

| Kachel | Warum sie wichtig ist |
|---|---|
| **WLAN-Signal** | In Worten statt in dBm („gut“, „schwach“). Erklärt einen ruckelnden Stream |
| **Chip-Temperatur** | Verrät, ob die Box doch in der Sonne hängt |
| **Arbeitsspeicher frei** | Sinkt er über Tage, gibt es ein Speicherleck — dann kommen Neustarts |
| **PSRAM frei** | Zeigt, ob Vorlaufspeicher und Kamera noch Luft haben |

### „Letzter Start“ ist das nützlichste Feld der ganzen Seite

Unter **Technische Details** steht, warum das Board zuletzt gestartet ist:

| Dort steht… | Das bedeutet |
|---|---|
| **Einschalten** | Der Akku war leer, und der Laderegler hat wieder eingeschaltet |
| **Spannungseinbruch** | Die Stromversorgung war zu schwach — meist ein schlechtes USB-Kabel |
| **Watchdog** | Das Programm hat sich aufgehängt und wurde automatisch neu gestartet |

Drei völlig verschiedene Ursachen — und du erkennst sie, ohne hingehen zu müssen.

### Wie die Kamera weiß, ob die Sonne lädt

Sie misst nur die Spannung, keinen Strom. Um trotzdem „lädt gerade“ anzuzeigen, vergleicht
sie die aktuelle Spannung mit der von vor 10 Minuten. Mehr als 10 mV Anstieg → **⬆ steigt**.

Das ist weniger genau als eine echte Strommessung, reicht aber für die Frage, die man
tatsächlich hat: *Kommt der Akku über den Tag oder nicht?*

### Die Selbstdiagnose der Speicherkarte

> SD-Karte schreibt 1,05 MB/s · reicht aus ✓

**Wichtig zum Verstehen:** Eine zu langsame Karte erzeugt **keine Fehlermeldung**. Sie
*bremst* — das Programm wartet auf sie, und im Clip landen weniger Bilder pro Sekunde. Man
erkennt es an **zwei Zahlen zusammen**:

| Anzeige | Bedeutung |
|---|---|
| Schreibrate nahe 1,2 MB/s **und** Bilder/s deutlich unter 10 | Karte am Limit → `BILD_QUALITAET` erhöhen |
| Schreibrate um 0,8 MB/s **und** Bilder/s bei ~10 | alles gut |
| „Bilder verworfen“ größer 0 | echter Schreibfehler — Karte voll oder defekt |

---

## 6.3 Der Ringspeicher — warum die Karte nie volläuft

Die Kamera schreibt in einen **Ring** mit fester Größe. Ist er voll, wird die **älteste**
Datei überschrieben.

```
   clip_001  clip_002  clip_003  ...  clip_200
      ▲                                   │
      └───────────────────────────────────┘
        Nach 200 fängt es wieder bei 1 an.
```

| Ring | Standard | Belegt etwa |
|---|---|---|
| Videoclips | 200 | ~1 GB |
| Fotos | 1000 | ~80 MB |
| Tonaufnahmen | 100 | ~140 MB |

Das ist Absicht, und es hat zwei Vorteile: **Die Karte kann nicht volllaufen** — und weil
die Dateinamen immer dieselben bleiben, muss die Karte keine Verzeichniseinträge ständig
anlegen und löschen. Das ist genau die Sorte Belastung, an der SD-Karten sonst sterben.

**Willst du mehr Rückblick?** `RING_CLIPS` in `config.h` auf 1000 stellen — dann sind es
etwa 6,4 GB, und auf einer 32-GB-Karte immer noch reichlich Luft.

**Und wenn du eine Aufnahme behalten willst:** einfach herunterladen. Was auf deinem
Computer liegt, kann der Ring nicht überschreiben. **Die Karte ist Puffer, nicht Archiv.**

---

## 6.4 Der Verlauf über Wochen — `tage.csv`

„War letzte Woche mehr los als diese?“ „Wie tief ist der Akku in der Regenwoche gefallen?“
„Wann fing die Fütterung an, sich zu häufen?“ — dafür reicht eine Tagesstatistik nicht.

Deshalb schreibt die Kamera **jede Stunde eine Zeile** über den laufenden Tag in die Datei
`/tage.csv`, und um Mitternacht die endgültige.

```
datum;besuche;erster;letzter;drin_s;clips;fotos;audio;akku_min;akku_max;h00;…;h23
2026-04-14;0039;05:47;20:03;001504;0026;0092;0000;3.71;4.11;000;000;…
2026-04-15;0044;05:44;20:08;001702;0029;0104;0000;3.68;4.12;000;000;…
2026-04-16;0047;05:41;20:12;001820;0031;0112;0000;3.61;4.09;000;000;…
```

| Spalte | Bedeutung |
|---|---|
| `datum` | Tag als `JJJJ-MM-TT` |
| `besuche` | gezählte Einflüge (von der Lichtschranke) |
| `erster` / `letzter` | erster und letzter Anflug als `HH:MM` |
| `drin_s` | Aufenthaltsdauer aller Vögel zusammen, in Sekunden |
| `clips` / `fotos` / `audio` | an diesem Tag entstandene Dateien |
| `akku_min` / `akku_max` | tiefster und höchster Akkustand des Tages |
| `h00`…`h23` | Anflüge je Stunde — daraus wird die Ø-Rushhour |

**Diese Datei kannst du herunterladen und in Excel öffnen.** Doppelklick, Spalten markieren,
Diagramm einfügen. Für ein Schul- oder Naturprojekt ist das Gold wert.

### Warum eine Textdatei und keine Datenbank?

Drei Gründe, alle drei praktisch:

1. **Eine Zeile anhängen ist ein einziger Schreibvorgang.** 156 Bytes, fertig. Eine
   Datenbank müsste die Datei einlesen, ergänzen und neu schreiben — und ein Stromausfall
   mittendrin würde **alles** zerstören statt nur die letzte Zeile.
2. **Alle Zeilen sind genau gleich lang.** Deshalb kann die Kamera direkt an die Stelle
   springen, wo die letzten 30 Tage anfangen, statt die ganze Datei durchzulesen. Bei einem
   Jahr Verlauf ist das der Unterschied zwischen „sofort“ und „56 KB durchkauen“ — und der
   Bastelcomputer hat nur ein paar Kilobyte frei.
3. **Man kann sie in Excel öffnen.** Ohne Umwandlung.

Und derselbe Trick mit der festen Zeilenlänge erlaubt noch etwas: Die Kamera liest die
**letzte** Zeile, und wenn sie vom selben Tag ist, überschreibt sie sie. Deshalb kann sie
stündlich mitschreiben, ohne dass 24 Zeilen pro Tag entstehen. Ein Stromausfall kostet damit
höchstens die letzte Stunde.

### Platzbedarf

| | |
|---|---|
| Eine Zeile | 156 Bytes |
| Ein ganzes Jahr | 56 KB |
| Die Website zeigt | `TAGE_ANZEIGEN` Tage (Standard 30, höchstens 60) |
| Die Datei behält | **alles**, unbegrenzt |

### Zwei Dinge, die man wissen sollte

**Der Tageswechsel braucht eine gestellte Uhr.** Im Routerbetrieb kommt sie vom Zeitserver
aus dem Internet. Im eigenen WLAN schenkt sie der erste Website-Besuch. Solange die Uhr
fehlt, zählt die Kamera weiter, schreibt aber keine Tageszeile — sonst stünde dort das Datum
1970.

**War die Kamera ein paar Tage aus** (leerer Akku), entsteht **keine** Zeile für die
fehlenden Tage. Im Diagramm ist dann eine Lücke zu sehen, und das ist richtig so: Es gab
keine Messung, also gibt es auch keine Null.

### Wenn der Verlauf leer bleibt

| Symptom | Ursache | Lösung |
|---|---|---|
| „Noch keine Tageszeile“ bleibt stehen | Uhr nicht gestellt | Details → „Uhrzeit gestellt: NEIN“ → im eigenen WLAN einmal die Website aufrufen |
| Verlaufskarte fehlt ganz | `TAGE_CSV_AN false` | in `config.h` einschalten |
| Alle Akkuwerte 0.00 | Spannungssensor nicht angeschlossen | [Schaltplan 3.8](03-schaltplan.md#38-der-spannungssensor--damit-du-den-akkustand-siehst) |
| `/tage.csv` bringt einen Fehler | Noch keine Stunde vergangen | eine Stunde warten |
| Besuche immer 0, obwohl Clips entstehen | Lichtschranke zählt nicht | `LICHTSCHRANKE_INVERTIERT` prüfen |

---

## 6.5 Die Schnittstellen — für Neugierige

Alles, was die Website anzeigt, kannst du auch direkt abrufen. Praktisch, wenn du eigene
Auswertungen basteln willst.

| Adresse | Was zurückkommt |
|---|---|
| `http://birdycam.local/` **oder** `http://192.168.4.1/` | die Website |
| `…:81/` | nur der Livestream (Bild, ohne Ton) |
| `…:82/` | nur der Tonstrom |
| `/api/status` | alles als JSON — Statistik, Akku, System, Netzwerk, Ton |
| `/api/system` | nur die Systemdaten |
| `/api/liste?typ=clips` | Liste der Aufnahmen (`clips`, `fotos`, `audio`) |
| `/api/tage?n=30` | die letzten 30 Tage als JSON |
| `/tage.csv` | das ganze Tagesarchiv zum Herunterladen |
| `/datei?p=/clips/clip_047.avi` | eine bestimmte Datei |

> 💡 **Bastelidee:** `/api/system` alle 5 Minuten von einem Rechner im Haus abfragen und
> mitschreiben. Dann siehst du über Wochen, wie sich Akku, Temperatur und freier Speicher
> entwickeln — und erkennst Probleme, bevor sie zum Ausfall werden.

---

## 6.6 Grenzen der Website — damit niemand enttäuscht ist

| Was nicht geht | Warum |
|---|---|
| **Clips im Browser abspielen** | Browser können MJPEG-AVI nicht. Herunterladen und mit [VLC](https://www.videolan.org/) öffnen |
| **Ton lippensynchron im Stream** | Der Browser puffert etwa eine Sekunde. Im **Clip** ist er synchron |
| **Ton im Stream auf iPhone/iPad** | Safari verlangt etwas, das ein endloser Strom nicht liefern kann → VLC oder Clip herunterladen |
| **Mehrere Zuschauer gleichzeitig** | Ein Livestream und ein Tonstrom, der zweite muss warten |
| **Von unterwegs zuschauen** | Nur im eigenen Netz. ⚠️ Es gibt **keinen Passwortschutz** — stell die Kamera **nicht** ins Internet |

> ⚠️ **Zum letzten Punkt, weil es wichtig ist:** Die Website hat absichtlich keine
> Anmeldung. Solange sie nur im Heimnetz erreichbar ist, ist das in Ordnung. Wenn du im
> Router eine Portfreigabe einrichtest, kann jeder im Internet in deinen Garten schauen —
> und die Kamera auch abschalten. **Also nicht tun.**

---

## 6.7 Ausbauideen — in der Reihenfolge, in der sie Spaß machen

| Idee | Aufwand | Was es bringt |
|---|---|---|
| `tage.csv` in Excel auswerten | 10 min | Diagramme über die ganze Brutzeit |
| Lieblingsclips herunterladen und sammeln | 5 min | Das Jahresalbum des Nistkastens |
| DS18B20-Temperaturfühler nachrüsten | 1 h | **Man sieht am Temperaturverlauf, ob gebrütet wird** — der Vogel heizt |
| `/api/status` von einem Rechner im Haus mitschreiben | 1 h | Langzeitverlauf über Monate |
| Zweite Kamera am Futterhaus | ein Winter | Farbe, Tageslicht, viele Arten — der natürliche nächste Schritt |

---

## 6.8 Bonus: Vogelgesang als eigene Datei

Ton in Clip und Stream ist ab Werk an. Darüber hinaus kann die Kamera **einzelne
Gesangsstücke** als eigene Tondatei mitschneiden — unabhängig davon, ob gerade ein Clip
läuft. Ab Werk ist das **aus**; einschalten in `config.h`:

```cpp
#define GESANG_AUFNEHMEN  true
```

Dann horcht das Programm zusätzlich mit: Wird es im Vogelgesang-Bereich laut, speichert es
eine Tondatei — **inklusive der 2 Sekunden davor**, weil sonst der Anfang jedes Rufs fehlen
würde. Die Website zeigt sie unter dem Reiter „Vogelgesang“.

**Was es kann:** „Heute war um 5:40 Uhr das erste Vogelkonzert, 40 Sekunden lang.“
**Was es nicht kann:** sagen, *welcher* Vogel singt ([1.7](01-ueberblick.md#17-was-bewusst-fehlt--und-warum)).

> **Wie es Vögel von Verkehrslärm unterscheidet — ein hübscher Trick:** Das Programm schaut
> nicht auf die Lautstärke, sondern auf den **Unterschied zwischen benachbarten Messwerten**.
> Tiefes Brummen (Autos, Wind) ändert sich langsam → kleine Unterschiede. Vogelgesang ist
> hoch und ändert sich schnell → große Unterschiede. Eine einzige Rechenzeile, und das
> Programm hört Vögel besser als Autos.

**Die Schwelle einstellen** mit [Sketch 6](05-software.md#52-die-sieben-lern-sketches):
Wert bei Stille notieren, Wert beim Pfeifen notieren, `GESANG_SCHWELLE` in die Mitte legen.

**Kosten:** etwa 1,4 MB je Minute Gesang auf der Karte, und ein halbes Megabyte
Arbeitsspeicher — aber nur, wenn es eingeschaltet ist.

→ Weiter mit [7. Wartung & Fehlersuche](07-wartung-und-fehlersuche.md)
