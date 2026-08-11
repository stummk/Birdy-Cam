# 8. Variante B — ESP32-S3 ⭐ **die gewählte Variante**

**≈ 149 € statt ≈ 304 €.** Dieselben Funktionen, ruckeligeres Video.

👉 **Das ist der Hauptweg dieses Projekts.** Dieses Kapitel enthält alles, was du für
Variante B brauchst: Stückliste, Schaltplan, Tutorial und Firmware. Für Mechanik und
Nistkasten-Einbau gelten [3. Schaltplan](03-schaltplan.md) und
[4. Bauplan](04-bauplan.md) unverändert.

**Warum diese Variante:** Als kleines Projekt reichen Livestream, Clips, Statistik und
Dashboard. Gesangsbestimmung und Artenerkennung würden ohnehin eine Basisstation im Haus
brauchen — und damit ein anderes Projekt sein. Variante A bleibt als Alternative
dokumentiert, falls das Projekt später wächst.

---

## 8.1 Wann diese Variante die richtige ist

| Nimm Variante B, wenn… | Nimm Variante A, wenn… |
|---|---|
| 150 € sind die Obergrenze | Full HD wirklich zählt |
| Elektronik-Basteln ist das Ziel | Clips sollen im Browser anklickbar sein |
| Der Kasten steht sehr schattig (10-W-Panel ist unkritischer) | Speicherstabilität ist die Hauptsorge |
| Kein Linux-Wissen im Haus | Später BirdNET oder Artenerkennung dazukommen soll |


> **Kein Fehlkauf-Risiko:** Wer mit B anfängt, kann später auf A umsteigen. Kamera-Modul,
> Panel und Akku sind dann zwar andere, aber IR-LEDs, MOSFET, Lichtschranke, Gehäuse und
> der ganze Bauplan bleiben gleich. Das sind ~50 € Übertrag.

---

## 8.2 Die Unterschiede in einer Tabelle

| | **A — Pi Zero 2 W** | **B — ESP32-S3** |
|---|---|---|
| Preis | ≈ 304 € | **≈ 149 €** |
| Videoformat | **H.264 in MP4** | MJPEG in AVI |
| 1080p Bildrate | **15–30** | ~6 |
| 10-s-Clip | ~5 MB **bei 15 fps** | ~7 MB **bei 6 fps** |
| Clip im Browser | **✅ ein Klick** | ❌ Download + [VLC](https://www.videolan.org/) |
| Speicher | **USB-SSD, keine SD im Betrieb** | High-Endurance-microSD |
| Datenbank | SQLite (Jahre an Verlauf) | CSV-Datei, eine Zeile je Tag ([8.7c](#87c-verlauf-über-tage--tagecsv)) |
| Verbrauch | 42 Wh/Tag | **16 Wh/Tag** |
| Panel | 30 W / 12 V (35 × 45 cm, 2 kg) | **5 V / 10 W (handtellergroß, ~300 g)** |
| Akku | LiFePO4 12 V/12 Ah, 3 Tage | LiPo 5000 mAh, ~1,2 Tage |
| Nachtsicht | ✅ | ✅ |
| Lichtschranke | ✅ | ✅ |
| Livestream nur bei Zuschauern | ✅ | ✅ |
| Vorlauf im Clip | ✅ 3 s | ✅ 3 s |
| **Ton in den Clips** | ❌ | ✅ **Bild und Ton in einer Datei** |
| **Ton im Livestream** | ❌ | ✅ **auf Knopfdruck** ([8.2d](#82d-ton--bild-und-ton-zusammen)) |
| Vogelgesang als eigene WAV | ❌ | 🟡 Code liegt bereit, abgeschaltet |
| Programmiersprache | Python | C++ (Arduino) |
| Kann es kaputtgehen durch Stromausfall? | Dateisystem-Risiko, durch WAL abgesichert | **praktisch nein** — kein Betriebssystem |

**Der ehrliche Kern:** 1080p bei 6 Bildern/s ist nicht wertlos — man sieht alles, was
passiert, und jedes Einzelbild ist scharfes Full HD. Es ruckelt nur. Wer Flüssigkeit höher
gewichtet, stellt in `config.h` auf `FRAMESIZE_SVGA` (800×600) um: ~15 Bilder/s und ~3 MB
pro Clip.

---

## 8.2b Stream ODER Aufnahme — die wichtigste Designentscheidung

Bei Full HD kann der ESP32 **nicht gleichzeitig** streamen und aufnehmen. Das ist keine
Software-Bequemlichkeit, sondern Arithmetik:

```
   Ein Bild bei UXGA, JPEG-Qualität 18    ≈ 80 KB
   Bei 10 Bildern pro Sekunde             ≈ 0,80 MB/s
   Ton (16 kHz, 16 Bit, Mono)             ≈ 0,03 MB/s   <- fällt kaum auf

   Die SD-Karte am XIAO hängt an SPI und schafft praktisch ~1,2 MB/s.
   -> Aufnehmen allein:  0,83 von 1,2 MB/s   = 69 %  ✅ passt
   -> Aufnehmen + streamen: die Kamera müsste jedes Bild zweimal
      ausliefern, und die SD-Karte konkurriert mit dem WLAN um Rechenzeit.
      -> Die Karte bremst das Programm, die Bildrate bricht ein.
```

Deshalb wechseln sich zwei Betriebsarten ab:

```
   Niemand schaut zu               Jemand ruft die Website auf
   ─────────────────────────       ────────────────────────────
   ⏺ AUFNAHMEBEREIT                📹 STREAM
   Kamera → Vorlaufpuffer (3 s)    Kamera → WLAN in Full HD
   Auslöser → Clip auf SD-Karte    laufender Clip wird beendet
   Lichtschranke zählt             Lichtschranke zählt WEITER
   Mikrofon → Tonspur im Clip      Mikrofon → Tonstrom ins WLAN
                                   kein Vorlaufpuffer
```

**Der Ton macht diese Trennung nicht enger.** Er kostet 0,03 MB/s auf der
Karte und läuft auf dem zweiten Prozessorkern — das Bild merkt davon nichts.
Umgeschaltet wird also nur die Bildrichtung, nicht die Tonrichtung.

**Zwei Details, die bewusst so sind:**

1. **Gezählt wird auch beim Streamen.** Die Lichtschranke kostet nichts, und die Statistik
   soll keine Löcher haben, nur weil jemand zuschaut.
2. **Der erste Clip nach dem Zuschauen hat keinen Vorlauf.** Während des Streams wird der
   Vorlaufpuffer nicht gefüllt — er ist danach leer und braucht ~3 Sekunden, um sich wieder
   zu füllen.

Umschaltbar in `config.h` mit `STREAM_HAT_VORRANG`. Auf `false` gesetzt wird beides parallel
versucht — sinnvoll nur bei kleiner Auflösung.

---

## 8.2c Router oder eigenes WLAN — beides geht

Die Kamera kann auf zwei Wegen erreichbar sein. Eingestellt wird das in
[`config.h`](../software/firmware/birdycam/config.h) mit `NETZ_MODUS`.

```
   NETZ_ROUTER                          NETZ_EIGENES
   ─────────────────────────            ──────────────────────────
   📱 ──── 🏠 Router ──── 📷            📱 ──────────────── 📷
      Handy   Heimnetz   Kamera            Handy      Kamera macht
                                                      eigenes WLAN auf
   ✅ sparsam (WLAN darf dösen)         ✅ funktioniert überall
   ✅ Uhrzeit aus dem Internet          ✅ kein Router nötig
   ✅ vom Sofa erreichbar               ⚠️ +40 % Stromverbrauch
   ⚠️ braucht Empfang am Kasten         ⚠️ keine Internet-Uhrzeit
                                        ⚠️ nur im Garten in Funkreichweite
```

**`NETZ_AUTO` (Standard) macht beides:** Erst wird der Router versucht. Kommt er innerhalb
von 20 Sekunden nicht, macht die Kamera ihr eigenes WLAN auf. Fällt der Router später aus,
schaltet sie nach etwa 2,5 Minuten ebenfalls um. Man steht also nie ohne Zugang da.

### ⚡ Die Stromrechnung — bitte vor dem Panel-Kauf lesen

Das ist der Punkt, den man vorher wissen muss:

| | Routerbetrieb | Eigenes WLAN |
|---|---|---|
| WLAN-Anteil am Verbrauch | ~0,25 W | **~0,50 W** |
| Gesamtverbrauch (mit Ton) | 16 Wh/Tag | **22 Wh/Tag** |
| Empfohlenes Panel | 5-V-Panel mit **10 W** | dasselbe, 10 W ist hier Pflicht |
| Aufpreis Panel | — | keiner — 10 W ist schon der Standard |

*(Der Ton kostet davon ~1 Wh/Tag, siehe [8.2d](#82d-ton--bild-und-ton-zusammen).
⚠️ **Das Panel darf keine 6 V haben, sondern 5 V** — der Laderegler verträgt
höchstens 6,5 V Leerlaufspannung. Welches konkret und warum:
[9.3b](09-bestellliste.md#93b-️-welches-solarpanel-passt-zum-laderegler).)*

**Warum:** Ein WLAN-Sender muss ständig „Funkbaken" aussenden, damit Handys ihn finden. Er
darf deshalb nicht schlafen. Im Routerbetrieb hingegen darf der ESP32 zwischen zwei
Funkkontakten dösen („Modem-Sleep") — das halbiert den WLAN-Verbrauch.

**Sparoption:** `AP_NACHTS_AUS = true` schaltet das eigene WLAN nachts ab, wenn niemand
verbunden ist (spart ~2,5 Wh/Tag). Der Preis: Nachts kommt man nicht sofort dran, sondern
erst nach dem nächsten Helligkeitstest.

### 🕐 Und das Uhrzeit-Problem

Im eigenen WLAN gibt es **kein Internet** — also auch keinen Zeitserver. Ohne Uhrzeit
hätten alle Dateien das Datum 1970, und das Stunden-Diagramm wäre sinnlos.

**Die Lösung:** Der erste Besucher der Website schenkt der Kamera die Uhrzeit seines Handys.
Eine Zeile JavaScript sendet sie beim Öffnen der Seite mit:

```javascript
fetch('/api/zeit?ts='+Math.floor(Date.now()/1000));
```

Die Kamera nimmt den Wert nur an, wenn sie noch **keine** Zeit hat — sonst würde jeder
Seitenaufruf die Uhr neu stellen und Zeitstempel könnten springen. Und sie prüft auf
Plausibilität (irgendwann zwischen 2024 und 2050), damit ein kaputter Browser die Statistik
nicht ruiniert.

> **Praktische Folge:** Nach jedem Neustart im eigenen WLAN einmal die Website aufrufen.
> Danach stimmen die Zeitstempel. Bis dahin steht auf der Website als Warnung
> „Uhrzeit fehlt".

### So verbindet man sich mit dem eigenen WLAN

1. Am Handy in die WLAN-Einstellungen gehen
2. Netz **`BirdyCam`** auswählen, Passwort **`vogelhaus`** eingeben (beides in `config.h`
   änderbar)
3. Browser öffnen: **`http://192.168.4.1`**

Die Kamera hat ein **Captive Portal** eingebaut: Sie leitet jede eingegebene Adresse auf
sich selbst um. Viele Handys öffnen die Seite dadurch von selbst — so wie im Hotel-WLAN.

> ⚠️ **Ein Handy-Ärgernis, das man kennen muss:** Android und iOS merken, dass dieses WLAN
> kein Internet hat, und wechseln gerne wieder ins Mobilfunknetz. Wenn die Seite plötzlich
> nicht mehr lädt: In den WLAN-Einstellungen bei der Meldung „Kein Internetzugriff"
> auf **„Verbindung beibehalten"** tippen. Bei Android hilft zusätzlich, in den
> Netzwerkdetails „Automatisch wechseln" abzuschalten.

### Die Selbstdiagnose auf der Website

Weil die SD-Geschwindigkeit der Engpass ist, zeigt das Dashboard sie an:

> SD-Karte schreibt 1,05 MB/s · reicht aus ✓

**Wichtig zum Verstehen:** Eine zu langsame Karte erzeugt keine Fehlermeldung. Sie *bremst*
— das Programm wartet auf sie, und im Clip landen weniger Bilder pro Sekunde. Man erkennt es
also an **zwei Zahlen zusammen**:

| Anzeige | Bedeutung |
|---|---|
| Schreibrate nahe 1,2 MB/s **und** Bilder/s deutlich unter 10 | Karte ist am Limit → `BILD_QUALITAET` erhöhen |
| Schreibrate um 0,8 MB/s **und** Bilder/s bei ~10 | alles gut |
| „Bilder verworfen" > 0 | echter Schreibfehler — Karte voll oder defekt |

Die Firmware probiert beim Start außerdem **40, dann 20, dann 10 MHz** SPI-Takt und nimmt
den schnellsten, der funktioniert. Der gewählte Takt steht im Seriellen Monitor.

---

## 8.2d Ton — Bild und Ton zusammen

Die XIAO Sense hat ein digitales Mikrofon an Bord. Es läuft dauerhaft mit, und
zwar auf dem **zweiten Prozessorkern**: Einer macht Bild, einer macht Ton. Der
Ton landet in einem Ringpuffer, aus dem sich **zwei Abnehmer unabhängig
voneinander** bedienen:

```
                        ┌──> Videoclip: zweite Spur in der AVI-Datei
   🎤 ──> [ Ring, 4 s ] ─┤
                        └──> Livestream: Tonstrom auf Tür 82
```

Der Ring ist der Grund, warum der **Clip-Vorlauf auch beim Ton funktioniert**:
Wenn der Vogel den Auslöser erreicht, liegen die letzten Sekunden noch im
Speicher — Bild *und* Ton. Ohne diesen Umweg würde jeder Clip mit dem Moment
anfangen, in dem schon alles passiert ist.

### Ton in den Clips

Ein AVI darf mehrere Spuren haben. Die Firmware schreibt zwei, abwechselnd:

```
   00dc Bild ─ 01wb Ton ─ 00dc Bild ─ 01wb Ton ─ ...
```

Genau dieses Abwechseln ist das I in AVI: **A**udio **V**ideo **I**nterleave.
Der Ton steckt unkomprimiert drin (PCM, wie in einer WAV-Datei) — Tonkompression
kann der ESP32 nicht, und er braucht sie auch nicht:

| | Wert |
|---|---|
| Abtastrate | 16 000 Messungen/s (deckt Vogelstimmen bis 8 kHz ab) |
| Platzbedarf | **32 KB/s**, also ~4 % zusätzlich zum Bild |
| 10-Sekunden-Clip | ~7 MB Bild **+ 0,3 MB Ton** |
| Abspielen | [VLC](https://www.videolan.org/) spielt Bild und Ton zusammen |

Die Serielle Ausgabe nennt am Clipende **beide Längen**, und daran erkennt man
sofort, ob etwas nicht stimmt:

```
[AVI] Clip fertig: 96 Bilder / 9.6 s Bild, 9.7 s Ton, 7810 KB (SD schreibt 0.86 MB/s)
```

Stehen dort zwei sehr verschiedene Zahlen, laufen Bild und Ton auseinander —
dann ist die Karte zu langsam (siehe `BILD_QUALITAET`).

### Ton im Livestream

Das Bild kommt als MJPEG in einem `<img>`-Element an, und ein `<img>` kann
keinen Ton. Deshalb gibt es den Ton als **zweiten Strom auf Tür 82**: technisch
eine WAV-Datei, die einfach nie zu Ende geht. Ein `<audio>`-Element auf der
Website spielt sie.

Zwei Dinge muss man dabei wissen — beide sind keine Schwächen des Aufbaus,
sondern Eigenschaften des Webs:

1. **Ton startet nicht von selbst.** Browser verbieten das (sonst würde jede
   Website losdudeln). Auf der Website gibt es deshalb den Knopf
   **🔊 Ton an** — einmal antippen ist die Erlaubnis.
2. **Der Ton läuft dem Bild etwa eine Sekunde nach**, weil der Browser ihn
   puffert. Für „was ist da los?" reicht das. Lippensynchron bekommt man das
   nur mit WebRTC, und das passt nicht in einen ESP32.

> 📱 **Auf iPhone und iPad kann der Tonstrom streiken.** Safari verlangt bei
> Audiodateien, dass der Server „Sprünge in der Datei" unterstützt (HTTP-Range)
> — bei einem Strom, der nie endet, geht das nicht. Chrome, Firefox und Edge
> (Android, Windows, Mac) spielen ihn. Klappt es auf dem iPhone nicht, steht
> die Meldung unter dem Knopf, und man hat zwei Auswege: den **Clip
> herunterladen** (der hat den Ton drin und läuft überall) oder
> `http://birdycam.local:82/` in **VLC für iOS** öffnen.

### Was der Ton kostet

| Posten | Aufschlag |
|---|---|
| Strom (Mikrofon + zweiter Kern) | **+1 Wh/Tag** (~7 %) |
| SD-Karte | +32 KB/s im Clip, ~+16 MB am Tag |
| PSRAM | 128 KB Ring + 80 KB Vorlauf-Eimer |
| WLAN beim Zuhören | +256 kbit/s |

Der Verbrauch steigt damit von 15 auf **16 Wh/Tag** (Router) bzw. von 21 auf
**22 Wh/Tag** (eigenes WLAN). An der Panelempfehlung ändert das nichts.

### Die Einstellungen dazu

Alles in [`config.h`](../software/firmware/birdycam/config.h), Abschnitt 6:

| Einstellung | Standard | Wirkung |
|---|---|---|
| `AUDIO_AN` | `true` | Hauptschalter. `false` = kein Mikrofon, gar kein Ton |
| `TON_IN_CLIPS` | `true` | Tonspur in der AVI-Datei |
| `TON_IM_STREAM` | `true` | Tonstrom auf Tür 82 |
| `TON_ABTASTRATE` | `16000` | `8000` halbiert den Platzbedarf, klingt dumpf |
| `TON_VERSTAERKUNG` | `2` | Im Kasten ist es leise. Zu hoch = es knackt |
| `TON_VORLAUF_MS` | `2500` | Ton vor dem Auslöser, passend zum Bild-Vorlauf |
| `GESANG_AUFNEHMEN` | `false` | zusätzlich einzelne WAV-Dateien, siehe [8.10](#810-der-bonus-vogelgesang-als-eigene-datei) |

> ⚠️ **Ein Mikrofon am Nistkasten hört den ganzen Garten**, nicht nur den
> Kasten. Es kann „Vogel im Kasten" und „Vogel im Nachbarbaum" nicht trennen —
> das ist keine Einschränkung des Aufbaus, sondern der Physik. Was man dagegen
> sehr gut hört: bettelnde Junge, und die sind laut.

### 🔨 Und ein Bauschritt, den man nicht vergessen darf

Das Mikrofon sitzt **fest auf dem XIAO-Board**, und das liegt in der
geschlossenen IP65-Box an der Außenwand des Kastens. Ohne einen Schallweg hört
es gedämpften Garten und kaum das Nest.

Die Lösung ist ein **6-mm-Loch von der Box in den Kasten** — beide Öffnungen
zeigen aufeinander, dazwischen eine Moosgummi-Dichtung, boxseitig eine
atmungsaktive Membran. Damit hört das Mikrofon ins Nest, und dicht bleibt es
trotzdem, weil das Loch nicht ins Freie geht.

👉 Anleitung mit Skizze: [Bauplan 4.5b](04-bauplan.md#45b-das-mikrofon-hören-lassen-nur-variante-b).
**Beim Bauen im Winter machen** — ab März darf der Kasten nicht mehr geöffnet
werden.

### Wenn der Ton nicht kommt — Fehlertabelle

| Symptom | Ursache | Abhilfe |
|---|---|---|
| Serieller Monitor sagt „Mikrofon startet nicht" | ESP32-Boardpaket älter als 3.x (`ESP_I2S.h` fehlt) | Boardverwalter → `esp32` aktualisieren |
| „Zu wenig PSRAM fuer den Tonring" | PSRAM steht auf „Disabled" oder der Vorlauf ist zu groß | PSRAM = **OPI PSRAM**, ggf. `VORLAUF_MAX_BILDER` senken |
| Clip hat keine Tonspur | `TON_IN_CLIPS false`, oder Mikrofon nicht gestartet | Startmeldung `[Audio] Mikrofon laeuft` prüfen |
| Ton im Clip **kürzer** als das Bild | Karte zu langsam, Ton wurde verworfen | Website → Details → „Ton verworfen"; `BILD_QUALITAET` erhöhen |
| Alles sehr leise | Kein Schallloch zur Box (siehe unten), oder `TON_VERSTAERKUNG 1` | Loch bohren, dann Verstärkung auf 3–4 |
| Es knackt und verzerrt | `TON_VERSTAERKUNG` zu hoch | auf 1–2 zurück |
| Knopf „Ton an" fehlt auf der Website | `TON_IM_STREAM false` oder Mikrofon aus | config.h prüfen |
| Nachts liegt ein Pfeifen über der Aufnahme | PWM der IR-LEDs im Hörbereich | siehe direkt darunter — ab Werk erledigt |

### Ein Nebeneffekt, der schon eingebaut ist

Die IR-LEDs werden per PWM gedimmt. Stünde dieser Takt bei 1 kHz, würde der
Stromstoß über die 3,3-V-Leitung ins Mikrofon koppeln — und nachts, wenn das
IR-Licht an ist, läge ein Pfeifen genau im Vogelgesang-Bereich über der
Aufnahme. Deshalb läuft die PWM in
[`strom.cpp`](../software/firmware/birdycam/strom.cpp) mit **20 kHz**: oberhalb
des Hörbaren und oberhalb dessen, was das Mikrofon überhaupt durchlässt.

---

## 8.3 Stückliste

👉 **Zum Bestellen: [9. Bestellliste](09-bestellliste.md)** — nach Shop gruppiert, mit
Preisen, Prüfpunkten und Bestellreihenfolge. Überblick in
[2. Stückliste, Variante B](02-stueckliste.md#variante-b--esp32-s3-die-günstige-alternative).

Die drei wichtigsten Punkte:

- **XIAO ESP32-S3 „Sense"** — nur die Sense-Version hat Kameraanschluss und SD-Slot.
  Möglichst **mit vorgelöteten Pins** kaufen.
- **Kameramodul 24-pol DVP ohne IR-Filter.** Hier gibt es eine Entscheidung, die
  [9.0](09-bestellliste.md#90-️-die-eine-entscheidung-die-du-vorher-treffen-musst)
  ausführlich erklärt — kurz:

  | | OV2640 „Night Vision" ⭐ | OV5640 ohne IR-Filter |
  |---|---|---|
  | Auflösung | 1600×1200 = **1,92 MP** | 1920×1080 = **2,07 MP** |
  | Bilder/s | **8–12** | ~6 |
  | Lieferbar | ✅ bestätigt | 🟡 schwer zu finden |

  **7 % weniger Pixel, doppelte Bildrate, sicher lieferbar** — deshalb ist die Firmware auf
  `FRAMESIZE_UXGA` mit dem OV2640 voreingestellt. Wer 16:9 will, nimmt den OV5640 und
  stellt `FRAMESIZE_FHD` ein.
- **Solarpanel mit 5 V** — entscheidend ist die **Leerlaufspannung unter 6,5 V**.
  Ein Panel mit Aufdruck „6 V" hat 7,2 V und zerstört den Laderegler, ein 12-V-Panel
  erst recht. Messanleitung und konkrete Modelle in
  [9.3b](09-bestellliste.md#93b-️-welches-solarpanel-passt-zum-laderegler).

> ⚠️ **Ein Kaufteil ist unbestätigt:** Ein fertiges NoIR-Modul speziell für die XIAO Sense
> konnte ich nicht verifizieren. Die 24-Pin-**DVP**-Module aus der ESP32-CAM-Welt passen
> mechanisch und elektrisch (ESP32 hat *kein* MIPI-CSI, entgegen manchen
> Shop-Beschreibungen), und davon gibt es Nachtsicht-Varianten. Rückfallebene: den
> IR-Filter aus dem mitgelieferten Modul selbst ausbauen — 20 Minuten Feinmotorik, und die
> Kamera kann dabei kaputtgehen. **Deshalb ein Ersatzmodul mitbestellen und vor dem Einbau
> in den Kasten testen.**

---

## 8.4 Schaltplan

Alles gesteckt und geschraubt. Der einzige Lötpunkt ist die Stiftleiste am Board — und die
kann man vorgelötet kaufen.

```mermaid
flowchart TB
  PV["☀️ Solarpanel 5 V / 10 W<br/>Leerlauf &lt; 6,5 V"]
  subgraph SPM["Solar Power Manager (E5)"]
    IN["Schraubklemme IN + / −"]
    JST["JST-Buchse BAT"]
    OUT["USB-A · 5 V"]
  end
  BAT["🔋 LiPo 1S 3,7 V<br/>5000 mAh"]
  subgraph XIAO["XIAO ESP32-S3 Sense"]
    USBC["USB-C"]
    P5V["5V"]
    P33["3V3"]
    PGND["GND"]
    D0["D0 = GPIO1"]
    D1["D1 = GPIO2"]
    D2["D2 = GPIO3"]
    FPC["24-pol FPC"]
    SD[("microSD 32 GB")]
  end
  VS["Spannungssensor (E8)"]
  MOS["MOSFET-Modul (C2)"]
  IRL["4× IR-LED 940 nm"]
  LS["IR-Lichtschranke"]
  CAM["📷 OV5640 ohne IR-Filter"]

  PV --> IN
  JST <--> BAT
  OUT -->|"USB-A → USB-C"| USBC
  BAT --> VS
  VS -->|"S"| D1
  D0 -->|"PWM"| MOS
  P33 -.-> MOS
  P5V --> MOS
  MOS --> IRL
  LS -->|"OUT"| D2
  P33 -.->|"VCC"| LS
  FPC -->|"FPC 10 cm"| CAM
```

### Pinbelegung

| Pin | GPIO | Belegung |
|---|---|---|
| **5V** | — | → MOSFET-Modul Lastseite `VIN+` |
| **3V3** | — | → MOSFET `VCC`, Lichtschranke `VCC` |
| **GND** | — | → gemeinsame Masse (dreimal nötig) |
| **D0** | GPIO1 | → MOSFET `SIG` (IR-Dimmung per PWM) |
| **D1** | GPIO2 | ← Spannungssensor `S` (Akkustand) |
| **D2** | GPIO3 | ← Lichtschranke `OUT` |
| D3–D5 | GPIO4–6 | frei, z. B. DS18B20 |
| **D8/D9/D10** | GPIO7/8/9 | **SD-Karte — nicht anfassen!** |
| *(intern)* | GPIO41/42 | Mikrofon auf dem Board (bei `AUDIO_AN = true`, Standard) |

> ⚠️ **Der häufigste Anfängerfehler:** D8, D9 oder D10 benutzen. Dann funktioniert plötzlich
> die SD-Karte nicht mehr, und man sucht den Fehler in der Software. **Merksatz: D8, D9, D10
> gehören der Speicherkarte.**

**Angenehm gegenüber Variante A:** Der ESP32 läuft komplett mit 3,3 V, und seine Pins
verzeihen mehr. Die „5 V zerstören den Pi"-Falle aus
[Schaltplan 3.3](03-schaltplan.md#33-️-die-wichtigste-warnung-33-v-nicht-5-v) gibt es hier
nicht. Trotzdem: Lichtschranke an **3V3**, dann passt das Signalniveau.

Für IR-LEDs ([3.5](03-schaltplan.md#35-die-ir-beleuchtung)), Lichtschranke im Einflugloch
([3.6](03-schaltplan.md#36-die-lichtschranke-im-einflugloch)) und die Kamera-Flachbandregeln
([3.7](03-schaltplan.md#37-kamera-und-ssd)) gilt Kapitel 3 unverändert.

---

## 8.5 Bauplan

**[Kapitel 4](04-bauplan.md) gilt komplett** — Deckel bohren, Acryl einkleben, LEDs setzen,
Lichtschranke justieren, Tropfschlaufen, Panelmontage.

Zwei Unterschiede:

1. **Kleineres Gehäuse.** Kein 12-Ah-Akku, kein Laderegler in Zigarettenschachtelgröße.
   Eine Box von ~120 × 80 × 50 mm reicht.
2. **Kleineres Panel.** Das 5-V-Kamerapanel misst etwa 17 × 16 cm und wiegt ~290 g — deutlich
   einfacher zu montieren als das 2-kg-Panel der Variante A.

---

## 8.6 Software-Tutorial (7 Lern-Sketches + die fertige Firmware)

Der Code liegt in [`software/firmware/`](../software/firmware/). Jeder Schritt ist ein
eigenes kleines Programm, das für sich funktioniert — wenn einer nicht klappt, ist der
Fehler in genau diesem Schritt.

### Schritt 0 — Arduino IDE einrichten (Erwachsener, 30 min)

1. [Arduino IDE 2.x](https://www.arduino.cc/en/software) installieren.
2. **Datei → Einstellungen → Zusätzliche Boardverwalter-URLs:**

```
https://espressif.github.io/arduino-esp32/package_esp32_index.json
```

3. **Werkzeuge → Board → Boardverwalter** → `esp32` von Espressif installieren
   (**Version 3.x oder neuer** — die Mikrofon-Bibliothek `ESP_I2S.h` gibt es erst dort).
4. Einstellungen unter **Werkzeuge**:

| Einstellung | Wert |
|---|---|
| Board | **XIAO_ESP32S3** |
| Port | der neue COM-Port |
| **PSRAM** | **OPI PSRAM** ⚠️ |
| Upload Speed | 921600 |

> ⚠️ **PSRAM ist die Einstellung, an der die meisten scheitern.** Steht sie auf „Disabled",
> startet die Kamera nie — und die Fehlermeldung sagt nicht, warum.

**Board wird nicht gefunden?** USB abziehen, **BOOT** gedrückt halten, USB anstecken, BOOT
loslassen. Nach dem Hochladen einmal **RESET** drücken.

### Die sieben Schritte

| # | Sketch | Was du siehst | Dauer |
|---|---|---|---|
| **1** | [`step1_hallo`](../software/firmware/steps/step1_hallo/step1_hallo.ino) | LED blinkt, Board erzählt von sich | 15 min |
| **2** | [`step2_sdkarte`](../software/firmware/steps/step2_sdkarte/step2_sdkarte.ino) | Karte erkannt, Schreibtest, Geschwindigkeit | 15 min |
| **3** | [`step3_kamera`](../software/firmware/steps/step3_kamera/step3_kamera.ino) | 🎉 **das erste Livebild im Browser** | 30 min |
| **4** | [`step4_irlicht`](../software/firmware/steps/step4_irlicht/step4_irlicht.ino) | unsichtbares Licht, Handy-Trick | 20 min |
| **5** | [`step5_akku`](../software/firmware/steps/step5_akku/step5_akku.ino) | Akku messen und **kalibrieren** | 25 min |
| **6** | [`step6_mikrofon`](../software/firmware/steps/step6_mikrofon/step6_mikrofon.ino) | Lautstärkebalken, Tonaufnahme — **prüft das Mikrofon** | 25 min |
| **7** | [`step7_lichtschranke`](../software/firmware/steps/step7_lichtschranke/step7_lichtschranke.ino) | Einflug/Ausflug mit Dauer | 30 min |
| **8** | [`birdycam`](../software/firmware/birdycam/) | die fertige Vogelkamera | 40 min |

Jeder Sketch enthält oben eine Anleitung, was vorher zu verkabeln ist, und unten eine
Fehlertabelle. Die Lernkästen („Was du gerade gelernt hast") stehen in
[Tutorial-Kapitel 5](05-software-tutorial.md) — die Erklärungen zu IR-Licht, PWM,
Spannungsteiler und Lichtschranke gelten für beide Varianten gleich.

### Schritt 8 — die fertige Firmware

Ordner [`software/firmware/birdycam/`](../software/firmware/birdycam/) in der Arduino IDE
öffnen. Alle Dateien erscheinen als Tabs. **Nur eine musst du anfassen:**
[`config.h`](../software/firmware/birdycam/config.h).

```cpp
#define NETZ_MODUS      NETZ_AUTO              // Router, eigenes WLAN, oder beides
#define WLAN_NAME       "HierDeinWLANName"     // <- eintragen (für Router/Auto)
#define WLAN_PASSWORT   "HierDeinPasswort"     // <- eintragen
#define AP_NAME         "BirdyCam"             // Name des eigenen WLAN
#define AP_PASSWORT     "vogelhaus"            // min. 8 Zeichen!
#define BATT_KALIBRIERUNG  5.00                // <- dein Wert aus Schritt 5
#define LICHTSCHRANKE_INVERTIERT  false        // <- ggf. aus Schritt 7
```

**Die Netzwerk-Einstellungen** (Erklärung in [8.2c](#82c-router-oder-eigenes-wlan--beides-geht)):

| Einstellung | Standard | Wirkung |
|---|---|---|
| `NETZ_MODUS` | `NETZ_AUTO` | Erst Router, sonst eigenes WLAN |
| | `NETZ_ROUTER` | Nur Heimnetz. Sparsam, Uhrzeit automatisch |
| | `NETZ_EIGENES` | Nur eigenes WLAN. Autark, **+40 % Strom** |
| `AP_NACHTS_AUS` | `false` | `true` spart ~2,5 Wh/Tag im eigenen WLAN |

Die vier Einstellungen, die über Bildqualität und Speicher entscheiden:

| Einstellung | Standard | Wirkung |
|---|---|---|
| `BILD_GROESSE` | `FRAMESIZE_UXGA` | `FRAMESIZE_FHD` nur mit OV5640 · `FRAMESIZE_SVGA` = ~15 Bilder/s |
| `BILD_QUALITAET` | `18` | kleiner = schöner + größer. **Unter 16 wird die SD-Karte zum Engpass** |
| `STREAM_HAT_VORRANG` | `true` | beim Zuschauen wird nicht aufgenommen |
| `RING_CLIPS` | `200` | ~1 GB. Auf 1000 erhöhen ist problemlos |

**Ton** (Erklärung in [8.2d](#82d-ton--bild-und-ton-zusammen)) und
**Tagesarchiv** (in [8.7c](#87c-verlauf-über-tage--tagecsv)):

| Einstellung | Standard | Wirkung |
|---|---|---|
| `AUDIO_AN` | `true` | Hauptschalter fürs Mikrofon |
| `TON_IN_CLIPS` | `true` | Clips bekommen eine Tonspur (+4 % Platz) |
| `TON_IM_STREAM` | `true` | Ton im Livestream, Knopf „🔊 Ton an" |
| `TON_VERSTAERKUNG` | `2` | lauter machen; zu hoch = es knackt |
| `TAGE_CSV_AN` | `true` | Tageszeile in `/tage.csv` schreiben |
| `TAGE_ANZEIGEN` | `30` | so viele Tage zeigt die Website |

Hochladen. Im Seriellen Monitor (115200 Baud):

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

Dann **`http://birdycam.local/`** öffnen.

---

## 8.7 Die Module der Firmware

| Datei | Aufgabe |
|---|---|
| [`birdycam.ino`](../software/firmware/birdycam/birdycam.ino) | Hauptprogramm, `setup()` und `loop()` |
| [`config.h`](../software/firmware/birdycam/config.h) | **alle Einstellungen** |
| [`camera_pins.h`](../software/firmware/birdycam/camera_pins.h) | Pinbelegung der Kamera (nicht ändern) |
| [`bewegung.cpp`](../software/firmware/birdycam/bewegung.cpp) | Bildvergleich auf verkleinertem Bild |
| [`lichtschranke.cpp`](../software/firmware/birdycam/lichtschranke.cpp) | Vögel zählen per Interrupt |
| [`avi.cpp`](../software/firmware/birdycam/avi.cpp) | AVI-Datei aus Bild **und Ton** bauen + SD-Selbstdiagnose |
| [`speicher.cpp`](../software/firmware/birdycam/speicher.cpp) | Ringspeicher, Statistik, **Tagesarchiv `/tage.csv`** |
| [`strom.cpp`](../software/firmware/birdycam/strom.cpp) | Akku, Akku-Trend, IR-PWM, Tiefschlaf bei Notaus |
| [`systeminfo.cpp`](../software/firmware/birdycam/systeminfo.cpp) | Systemzustand, Gesundheitsbewertung, WLAN-Aufsicht |
| [`audio.cpp`](../software/firmware/birdycam/audio.cpp) | Mikrofon, **Tonring für Clip und Stream**, Gesangserkennung |
| [`netzwerk.cpp`](../software/firmware/birdycam/netzwerk.cpp) | Router **oder** eigenes WLAN, Captive Portal, Uhrzeit |
| [`web.cpp`](../software/firmware/birdycam/web.cpp) | Website, Livestream (81) und **Tonstrom (82)** |

### Ein Blick in `avi.cpp` — für Neugierige

Der ESP32 kann kein H.264. Er kann aber sehr schnell Einzelfotos machen. Ein **MJPEG-AVI**
ist genau das: viele JPEG-Bilder hintereinander mit einem 224 Byte großen „Deckblatt" davor,
das sagt „spiel das mit 6 Bildern pro Sekunde ab".

Ein paar Zahlen im Deckblatt (wie viele Bilder es geworden sind) kennt man erst am **Ende**.
Deshalb schreibt das Programm zuerst Platzhalter, und springt zum Schluss zurück und trägt
die richtigen Werte ein. Das ist ein Muster, das in Dateiformaten sehr oft vorkommt — und
gut zum Verstehen, wie Dateien eigentlich aufgebaut sind.

Mit Ton wird das Deckblatt von 224 auf **326 Bytes** größer, weil eine zweite
Spur beschrieben werden muss. Danach wechseln sich die Häppchen ab: `00dc` für
ein Bild, `01wb` für das Stück Ton dazu. Am Ende trägt das Programm bei der
Bildspur die Bildrate ein und bei der Tonspur, aus wie vielen Messwerten sie
besteht — daraus errechnet der Player beide Längen, und wenn sie
zusammenpassen, ist der Clip synchron.

---

## 8.7b Die Website — Dashboard, Statistik und Systemzustand

Die Website läuft **auf dem ESP32 selbst**. Kein Server, keine Cloud, kein Abo.
`http://birdycam.local/` im Heimnetz — fertig.

```
┌──────────────────────────────────────────────────────────┐
│ 🐦 BirdyCam  ☀️Tag  ⏺aufnahmebereit  🔋78%⚡  Kasten leer │
├──────────────────────────────────────────────────────────┤
│ LIVEBILD                                                 │
│ ┌──────────────────────────────────────────────────────┐ │
│ │             [ Livestream, Port 81 ]                  │ │
│ └──────────────────────────────────────────────────────┘ │
│ (🔊 Ton an)                                              │
├──────────────────────────────────────────────────────────┤
│ HEUTE                                                    │
│ ┌────┐┌────┐┌─────┐┌─────┐                               │
│ │ 47 ││1382││ 5:41││20:12│                               │
│ └────┘└────┘└─────┘└─────┘                               │
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
│ Ø 41,3 Besuche am Tag · bester Tag 16.04. mit 78 ·        │
│ tiefster Akkustand 3,58 V am 12.04. · 30 Tage im Archiv   │
│ tage.csv herunterladen · eine Zeile je Tag, öffnet Excel  │
├──────────────────────────────────────────────────────────┤
│ AKKU                                                     │
│ [████████████████░░░░]  78 % (3.92 V)                    │
│ ⬆ steigt — die Sonne lädt   (Verlauf über 10 Minuten)    │
├──────────────────────────────────────────────────────────┤
│ SYSTEMZUSTAND                                            │
│ ┌──────────────────────────────────────────────────────┐ │
│ │ ✓ Alles in Ordnung                                   │ │
│ └──────────────────────────────────────────────────────┘ │
│ · Alles in Ordnung                                       │
│ ┌────────┐┌────────┐┌────────┐┌────────┐                 │
│ │gut     ││  42 °C ││ 121 KB ││ 3.4 MB │                 │
│ │(-61dBm)││  Chip  ││  Heap  ││ PSRAM  │                 │
│ └────────┘└────────┘└────────┘└────────┘                 │
│ SD-Karte schreibt 1,05 MB/s · reicht aus ✓                │
│ Karte 1204 von 30500 MB belegt (4 %) · 47 Clips · 213 F. │
│ ▸ Technische Details                                     │
├──────────────────────────────────────────────────────────┤
│ AUFNAHMEN   (Videoclips)( Fotos )( Vogelgesang )         │
│  ▶ clip_047.avi  (Bild+Ton)  5.1 MB · 16.04. 06:12       │
└──────────────────────────────────────────────────────────┘
```

### Die Systemzustand-Karte

Der Kasten hängt ab März unerreichbar im Garten. Man kann dann **nichts** mehr nachschauen
— außer über die Website. Also zeigt sie alles, was zur Beurteilung nötig ist.

**Oben eine Ampel** mit Klartext-Meldungen. Sie wird aus diesen Regeln gebildet:

| Prüfung | ⚠️ Achtung | ✕ Problem |
|---|---|---|
| Akkuspannung | unter 3,60 V | nahe Notaus-Schwelle |
| SD-Karte | Bilder verworfen (zu langsam) | keine Karte erkannt |
| Arbeitsspeicher | war schon unter 35 KB | jetzt unter 25 KB |
| WLAN | Signal unter −80 dBm | — |
| Chip-Temperatur | über 65 °C | über 80 °C |
| Kamerafehler | mehr als 50 | — |
| Letzter Neustart | Absturz oder Spannungseinbruch | — |

**Vier Kacheln** mit den Werten, die am häufigsten etwas erklären:

| Kachel | Warum sie wichtig ist |
|---|---|
| **WLAN-Signal** | In Worten statt dBm („gut", „schwach"). Erklärt ruckelnde Streams |
| **Chip-Temperatur** | Verrät, ob das Gehäuse in der Sonne hängt |
| **Arbeitsspeicher frei** | Sinkt er über Tage, gibt es ein Speicherleck → Neustarts |
| **PSRAM frei** | Zeigt, ob Vorlaufpuffer und Kamera noch Luft haben |

**Darunter zwei Zeilen zur Speicherkarte:** erreichte Schreibrate und ob sie ausreicht,
plus Belegung und Anzahl der Aufnahmen.

**Und aufklappbar „Technische Details"** — die Werte, die man einmal im Jahr braucht:

```
Laufzeit                     18 h 42 min
Letzter Start                Einschalten
WLAN-Neuverbindungen         2
IP-Adresse                   192.168.1.87
Kamerafehler                 0
Bilder pro Sekunde           9.4
Arbeitsspeicher frei / min.  121 / 96 KB
PSRAM frei / gesamt          3520 / 8189 KB
Chip                         ESP32-S3 @ 240 MHz
ESP-IDF                      5.x.x
Lichtschranke                Strahl frei
Durchflüge gezählt           94
IR-Licht                     aus (Tag)
```

> **„Letzter Start" ist das nützlichste Feld der ganzen Seite.** Steht dort
> *Spannungseinbruch*, war die Stromversorgung zu schwach. Steht dort *Watchdog*, hat sich
> das Programm aufgehängt. Steht dort *Einschalten*, war der Akku leer und der Laderegler
> hat wieder eingeschaltet. Drei völlig verschiedene Ursachen — und man erkennt sie ohne
> hinzugehen.

### Akku ohne Amperemeter

Anders als die Pi-Variante (die einen INA219 hat) misst der ESP32 nur **Spannung**. Ob die
Sonne lädt, erkennt die Firmware deshalb am **Verlauf**: Sie vergleicht die aktuelle
Spannung mit der von vor 10 Minuten. Mehr als 10 mV Anstieg → „lädt".

Das ist weniger präzise als eine Strommessung, reicht aber für die Frage, die man
tatsächlich hat: *Kommt der Akku über den Tag oder nicht?*

### Schnittstellen

| Adresse | Rückgabe |
|---|---|
| `http://birdycam.local/` **oder** `http://192.168.4.1/` | die Website |
| `…:81/` | reiner Livestream (MJPEG, ohne Ton) |
| `…:82/` | reiner Tonstrom (endlose WAV-Datei) |
| `/api/status` | alles als JSON — Statistik, Akku, System, Netzwerk, Ton |
| `/api/system` | nur die Systemdaten |
| `/api/liste?typ=clips` | Liste der Aufnahmen (`clips`, `fotos`, `audio`) |
| `/api/tage?n=30` | die letzten 30 Tage als JSON + Stunden-Durchschnitt |
| `/tage.csv` | das ganze Tagesarchiv zum Herunterladen |
| `/api/zeit?ts=…` | Uhrzeit stellen (macht die Website automatisch) |
| `/datei?p=/clips/clip_047.avi` | die Datei |

Im **eigenen WLAN** ist die feste Adresse `192.168.4.1` immer erreichbar — `.local` kann
je nach Handy funktionieren oder nicht.

> 💡 **Bastelidee:** `/api/system` alle 5 Minuten von einem Rechner im Haus abfragen und
> mitschreiben. Dann sieht man über Wochen, wie sich Akku, Temperatur und freier Speicher
> entwickeln — und erkennt Probleme, bevor sie zum Ausfall werden.

### Die Netzwerk-Aufsicht

Ohne sie wäre die Kamera nach dem ersten Router-Neustart dauerhaft offline: Sie würde weiter
aufnehmen, aber niemand käme mehr dran. Deshalb prüft
[`netzwerk.cpp`](../software/firmware/birdycam/netzwerk.cpp) alle 15 Sekunden:

| Situation | Was passiert |
|---|---|
| Router weg (Modus ROUTER) | Neuverbindung versuchen, immer wieder |
| Router weg (Modus AUTO) | Nach ~2,5 Minuten eigenes WLAN aufmachen |
| Router wieder da | Verbindung nutzen, Uhrzeit nachholen |
| Eigenes WLAN + Nacht + niemand verbunden | Bei `AP_NACHTS_AUS` abschalten, morgens wieder an |

**Sie wartet dabei nie** — die Kamera nimmt auch ohne Netzwerk weiter auf. Wie oft
neu verbunden wurde, steht in den technischen Details.

---

## 8.7c Verlauf über Tage — `/tage.csv`

„War letzte Woche mehr los als diese?", „Wie tief ist der Akku in der Regenwoche
gefallen?", „Wann fing die Fütterung an, sich zu häufen?" — dafür reicht eine
Tagesstatistik nicht. Deshalb schreibt die Kamera **jede Stunde eine Zeile über
den laufenden Tag** in die Datei `/tage.csv` und um Mitternacht die endgültige.

```
datum;besuche;erster;letzter;drin_s;clips;fotos;audio;akku_min;akku_max;h00;…;h23
2026-04-14;0039;05:47;20:03;001504;0026;0092;0000;3.71;4.11;000;000;…
2026-04-15;0044;05:44;20:08;001702;0029;0104;0000;3.68;4.12;000;000;…
2026-04-16;0047;05:41;20:12;001820;0031;0112;0000;3.61;4.09;000;000;…
```

| Spalte | Bedeutung |
|---|---|
| `datum` | Tag als `JJJJ-MM-TT` |
| `besuche` | gezählte Einflüge (Lichtschranke) |
| `erster` / `letzter` | erster und letzter Anflug als `HH:MM` |
| `drin_s` | Aufenthaltsdauer aller Vögel zusammen, in Sekunden |
| `clips` / `fotos` / `audio` | an diesem Tag entstandene Dateien |
| `akku_min` / `akku_max` | tiefster und höchster Akkustand des Tages |
| `h00`…`h23` | Anflüge je Stunde — daraus wird die Ø-Rushhour |

### Warum CSV und nicht JSON — die Entscheidung im Klartext

Das war die eigentliche Frage, und sie hat drei Antworten, alle drei zugunsten
von CSV:

1. **Anhängen ist ein einziger Schreibvorgang.** Eine Zeile, 156 Bytes, fertig.
   Bei JSON müsste die Kamera die ganze Liste einlesen, im Arbeitsspeicher
   ergänzen und komplett neu ausgeben. Mit 30 Tagen geht das noch, mit 300 nicht
   mehr — und ein Stromausfall mitten im Neuschreiben würde **die ganze Datei**
   zerstören, nicht nur die letzte Zeile.
2. **Alle Zeilen sind gleich lang.** Genau 156 Bytes. Damit kann die Kamera
   direkt an die Stelle springen, wo die letzten 30 Tage anfangen, statt die
   Datei von vorne zu lesen. Bei einem Jahr Verlauf ist das der Unterschied
   zwischen „sofort" und „56 KB durchkauen" — und der ESP32 hat nur ein paar
   Kilobyte freien Arbeitsspeicher übrig.
3. **Man kann sie in Excel öffnen.** Doppelklick auf die heruntergeladene Datei,
   Diagramm markieren, fertig. Ein JSON-Array müsste man erst umwandeln.

Und derselbe Trick mit der festen Zeilenlänge erlaubt noch etwas: Die Kamera
liest die **letzte** Zeile, und wenn sie vom selben Tag ist, springt sie zurück
und überschreibt sie. Deshalb kann sie stündlich mitschreiben, ohne dass 24
Zeilen pro Tag entstehen. Ein Stromausfall kostet damit höchstens die letzte
Stunde — nicht den ganzen Tag.

> **Der JSON-Teil fehlt trotzdem nicht:** `/api/tage?n=30` liefert die Zeilen
> als JSON, so wie die Website es braucht. JSON entsteht also beim Abruf und
> nicht beim Speichern — genau dort, wo es billig ist.

### Was die Website daraus macht

- **Ein Balken je Tag** über die letzten `TAGE_ANZEIGEN` Tage (Standard 30).
  Antippen zeigt Datum, Besuche, Clips, Akkufenster und Minuten im Kasten.
- **Eine Zeile Zusammenfassung:** Durchschnitt, bester Tag, tiefster Akkustand
  mit Datum.
- **Die Rushhour umschaltbar** zwischen *heute* und *Ø der letzten Tage*. Der
  Durchschnitt kommt fertig aus der Kamera — der Browser muss nicht 720
  Einzelwerte verrechnen, und die Antwort bleibt klein.
- **Einen Link auf `tage.csv`** zum Herunterladen.

### Platzbedarf und Grenzen

| | |
|---|---|
| Eine Zeile | 156 Bytes |
| Ein Jahr | 56 KB |
| Website zeigt | `TAGE_ANZEIGEN` Tage, höchstens 60 |
| Datei behält | **alles**, unbegrenzt |

Der Tageswechsel braucht eine **gestellte Uhr**. Im Routerbetrieb kommt sie vom
Zeitserver; im eigenen WLAN schenkt sie der erste Website-Besuch
([8.2c](#82c-router-oder-eigenes-wlan--beides-geht)). Solange die Uhr fehlt,
zählt die Kamera weiter, schreibt aber keine Tageszeile — sonst stünde dort
das Datum 1970.

War die Kamera ein paar Tage aus (leerer Akku), entsteht **keine** Zeile für
die fehlenden Tage. Im Diagramm ist dann eine Lücke zu sehen, und das ist
richtig so: Es gab keine Messung, also gibt es auch keine Null.

### Wenn der Verlauf leer bleibt

| Symptom | Ursache | Abhilfe |
|---|---|---|
| „Noch keine Tageszeile" bleibt stehen | Uhr nicht gestellt | Details → „Uhrzeit gestellt: NEIN" → im eigenen WLAN einmal die Website aufrufen |
| Verlaufskarte fehlt ganz | `TAGE_CSV_AN false` | in `config.h` einschalten |
| Alle Akkuwerte 0.00 | Spannungssensor nicht angeschlossen | [Schaltplan 8.4](#84-schaltplan), oder `AKKU_MESSEN false` — dann bleibt die Spalte leer |
| `/tage.csv` bringt 404 | Noch keine Karte oder noch keine Stunde vergangen | eine Stunde warten oder `/tage.csv` erneut aufrufen |
| Besuche immer 0, obwohl Clips entstehen | Lichtschranke zählt nicht | `LICHTSCHRANKE_INVERTIERT` prüfen ([Sketch 7](../software/firmware/steps/step7_lichtschranke/step7_lichtschranke.ino)) |

---

## 8.8 Speicher — warum die SD-Karte hier hält

Variante B kann keine SSD betreiben; ein ESP32 hat kein USB-Host für Massenspeicher. Das
Problem ist aber kleiner, als es klingt.

**Warum SD-Karten in Raspberry-Pi-Projekten sterben:** nicht die Videos, sondern das
**Betriebssystem** — Systemlogs im Sekundentakt, Swap-Datei, Dateisystem-Journal, unsauberes
Ausschalten mitten im Schreiben.

**Ein ESP32 hat kein Betriebssystem auf der Karte.** Keine Logs, kein Swap, kein Journal.
Die Karte sieht ausschließlich große, zusammenhängende Schreibvorgänge: ein Clip, ein Bild.

| Posten | Menge/Tag (UXGA, Qualität 18) |
|---|---|
| Bewegungsclips (~60 × 8 s × 0,80 MB/s) | ~384 MB |
| Tonspur in den Clips (~480 s × 32 KB/s) | ~16 MB |
| Vogelfotos (~200 × 80 KB) | ~16 MB |
| Statistik-Textdatei (1× pro Minute) | < 1 MB |
| Tagesarchiv `/tage.csv` (1 Zeile) | 156 Bytes |
| **Summe** | **≈ 0,42 GB/Tag** |

> **Überraschend, aber wichtig:** Die Tagesmenge hängt fast **nicht** von der Auflösung ab,
> sondern nur davon, **wie viele Sekunden aufgenommen werden**. Der Grund: In beiden Fällen
> ist die SD-Karte der Engpass, und die Firmware füllt sie bis knapp unter die Grenze — bei
> Full HD mit 6 großen Bildern pro Sekunde, bei SVGA mit 15 kleinen. Unterm Strich landen
> beide bei ~0,6–0,7 MB/s.
>
> **SVGA kauft also Flüssigkeit, nicht Speicherplatz.** Wer wirklich weniger Daten will,
> senkt `CLIP_MAX_SEKUNDEN` oder erhöht `BILD_QUALITAET` (z. B. auf 22).

Ein 200er-Clipring belegt damit rund **1,3 GB** — auf einer 32-GB-Karte reichlich Luft. Wer
mehr Rückblick will, kann `RING_CLIPS` problemlos auf 1000 erhöhen (~6,4 GB).

Drei Maßnahmen in der Firmware:

1. **High-Endurance-Karte** (Dashcam-Klasse, für Dauervideo gebaut).
2. **Ringspeicher mit festen Dateinamen** — Verzeichniseinträge werden wiederverwendet statt
   ständig angelegt und gelöscht. Der Ordner wächst nie.
3. **Datei sofort schließen.** Nach jedem Clip. Ein Stromausfall kostet dann maximal den
   *einen* laufenden Clip, nie das Dateisystem.

**Rechnung:** 0,4 GB/Tag ergibt bei einer Brutsaison von etwa fünf aktiven Monaten rund
**60 GB pro Jahr**; im Dauerbetrieb über zwölf Monate wären es ~146 GB. Eine
32-GB-High-Endurance-Karte (Dashcam-Klasse) verträgt mehrere Terabyte an Schreibvolumen —
das sind auch pessimistisch gerechnet **viele Jahre**. Und der Ausfallmechanismus
„Betriebssystem zerschreibt die Karte" existiert hier gar nicht.

> **Und wenn sie stirbt?** Karte tauschen, 12 €. Die Firmware legt Ordner und Ring
> automatisch neu an. Was wichtig war, ist über die Website heruntergeladen — die Karte ist
> Puffer, nicht Archiv.

---

## 8.9 Grenzen dieser Variante

| Was nicht geht | Warum |
|---|---|
| Clips im Browser abspielen | MJPEG-AVI kann kein Browser, auch mit Ton nicht. Herunterladen und [VLC](https://www.videolan.org/) |
| Ton lippensynchron im Stream | Der Browser puffert ~1 s. Im **Clip** ist er synchron |
| Ton im Stream auf iPhone/iPad | Safari verlangt HTTP-Range, ein endloser Strom kann das nicht → VLC oder Clip herunterladen |
| Mehrere Zuschauer | Ein Livestream und ein Tonstrom gleichzeitig, der zweite muss warten |
| Full HD flüssig | ~6 Bilder/s. SVGA ist die angenehmere Wahl |
| Verlauf mit Uhr-Ausfall | Ohne gestellte Uhr keine Tageszeile — [8.7c](#87c-verlauf-über-tage--tagecsv) |
| Gesang mit Artnamen | Braucht BirdNET auf einem Pi 4/5 — [1.7](01-machbarkeit.md#17-was-bewusst-fehlt-und-warum) |
| Artenerkennung | Bewusst weggelassen |
| Von unterwegs zuschauen | Nur im Heim-WLAN. Kein Passwortschutz → **nicht** ins Internet stellen |

*Der „Verlauf über Wochen" stand hier früher als Grenze — er ist jetzt drin,
siehe [8.7c](#87c-verlauf-über-tage--tagecsv).*

---

## 8.10 Der Bonus: Vogelgesang als eigene Datei

Ton in Clip und Stream ist ab Werk an ([8.2d](#82d-ton--bild-und-ton-zusammen)).
Darüber hinaus kann die Kamera **einzelne Gesangsstücke** als WAV-Datei
mitschneiden — unabhängig davon, ob gerade ein Clip läuft. In `config.h`:

```cpp
#define GESANG_AUFNEHMEN  true
```

Dann horcht die Daueraufgabe auf Kern 0 zusätzlich mit: Wird es im
Vogelgesang-Frequenzbereich laut, speichert sie eine WAV-Datei — inklusive der
2 Sekunden *davor*, weil sonst der Anfang jedes Rufs fehlt. Round-Robin über
100 Dateien.

**Was es kann:** „Heute war um 5:40 Uhr das erste Vogelkonzert, 40 Sekunden lang."
**Was es nicht kann:** sagen, *welcher* Vogel singt.

Der Trick zur Erkennung ist eine einzige Rechenzeile: die **Differenz** zwischen
benachbarten Messwerten. Tiefes Brummen (Verkehr, Wind) ändert sich langsam → kleine
Differenzen. Vogelgesang ist hoch und ändert sich schnell → große Differenzen. Damit hört
das Programm Vögel besser als Autos, ohne irgendeine Signalverarbeitungs-Bibliothek.

Die Schwelle stellt man mit [Schritt 6](../software/firmware/steps/step6_mikrofon/step6_mikrofon.ino)
ein: Wert bei Stille notieren, Wert beim Pfeifen notieren, `GESANG_SCHWELLE` in die Mitte
legen. Wichtig: Die Erkennung rechnet mit den **unverstärkten** Werten — an
`TON_VERSTAERKUNG` zu drehen verändert die Schwelle also nicht.

**Kosten:** 480 KB PSRAM für den Aufnahmepuffer (nur wenn eingeschaltet) und
~1,4 MB je Minute Gesang auf der Karte.

---

← Zurück zur [Übersicht](../README.md) · Hauptplan: [1. Machbarkeit](01-machbarkeit.md)
