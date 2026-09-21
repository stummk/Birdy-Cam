# 7. Wartung & Fehlersuche

---

## 7.1 Der Jahresrhythmus

Das Projekt hat einen festen Takt, den die Vögel vorgeben — nicht wir.

| Monat | Was zu tun ist |
|---|---|
| **September** | 🔧 **Wartungsfenster.** Kasten öffnen: altes Nest raus, Linse putzen (Spinnennetze!), Lichtschranke reinigen, Silikagel tauschen, Akku prüfen, Panel abwischen |
| Oktober | Software-Updates, Umbauten, Erweiterungen — **jetzt** ist die Zeit dafür |
| November–Februar | 🛠️ **Bastelzeit.** Neue Ideen umsetzen, Daten des Vorjahres auswerten |
| **Ende Februar** | ⚠️ **Letzter Termin.** Danach wird der Kasten nicht mehr geöffnet |
| März | 👀 Zuschauen. Erste Besichtigungen durch Interessenten |
| April–Juni | 🥚 Brut und Aufzucht. **Kasten bleibt zu.** Nur Website beobachten |
| Juli–August | Der Kasten wird leer. Warten bis September (Zweitbruten sind möglich) |

> ⭐ **Der wichtigste Satz des ganzen Projekts:** Ab März wird der Kasten nicht geöffnet.
> Nicht für ein Kabel, nicht für eine Schraube, nicht „nur ganz kurz“. § 44 BNatSchG — und
> davor gesunder Menschenverstand. Wenn im April etwas kaputt ist, ist es bis September
> kaputt. Deshalb steckt so viel Aufwand im Testen **vor** dem Einbau.

---

## 7.2 Der wöchentliche Blick (30 Sekunden)

Website aufrufen, vier Dinge prüfen:

| Prüfpunkt | Gut | Nicht gut |
|---|---|---|
| **Akku** | steigt tagsüber, sinkt nachts | sinkt über mehrere Tage durchgehend |
| **Akku-Pfeil mittags** | ⬆ steigt | ⬇ sinkt trotz Sonne → Panel prüfen |
| **Besuche heute** | passt zur Jahreszeit | 0, oder unrealistisch viele |
| **Laufzeit** | wächst | springt immer wieder auf 0 → Neustarts! |
| **Systemzustand** | grüner Haken | gelbe oder rote Meldung |

Springt die Laufzeit immer wieder zurück, startet das Board ständig neu — siehe
[7.4](#74-der-häufigste-fehler-ständige-neustarts).

---

## 7.3 Fehlersuche: Symptom → Ursache

### Die Kamera ist gar nicht erreichbar

Arbeite die Liste **von oben nach unten** ab. Der erste Punkt ist der häufigste.

| Prüfen | Wie |
|---|---|
| **Antenne ab?** | Das Plättchen am u.FL-Sockel des XIAO löst sich leicht, wenn beim Warten am Kabel gezogen wurde. Ohne Antenne ist die Kamera praktisch funkstumm |
| **Akku leer?** | Nach mehreren Regentagen legt sich die Kamera schlafen. Bei Sonne kommt sie von selbst zurück — erst mal einen sonnigen Tag abwarten |
| **Router neu gestartet?** | Die Kamera verbindet sich selbst wieder. Bis zu 15 Sekunden warten |
| **`.local` geht nicht** | Die IP-Adresse direkt versuchen. Sie steht im Router unter „Netzwerk“ oder „Geräte“ |
| **Eigenes WLAN?** | `http://192.168.4.1` probieren |
| **Immer noch nichts** | Powerbank an die Notlade-Buchse. Kommt sie dann → es war der Strom |

> **Und wenn wochenlang nichts geht:** Nicht den Kasten öffnen. Die Elektronikbox sitzt
> **außen** — genau dafür. Box öffnen, USB-Kabel an den Computer, Seriellen Monitor
> aufmachen und lesen, was das Board sagt.

Beim Start sagt die Kamera im Seriellen Monitor selbst, wie gut der Empfang ist —
`[Netz] Verbunden. IP: … (-58 dBm)`. Kommt sie gar nicht durch, **listet sie alle WLANs
auf, die sie hört**, mit Stärke und Kanal. Das unterscheidet „Passwort falsch“ von
„Signal kommt nicht an“:

| Zahl | Bedeutung |
|---|---|
| −30 bis −60 dBm | ausgezeichnet, Livestream läuft flüssig |
| −60 bis −70 dBm | brauchbar |
| −70 bis −80 dBm | Bilder ja, flüssiger Livestream meist nicht |
| unter −80 dBm | zu wenig. Antenne prüfen, sonst Repeater |

### Es entstehen keine Clips

| Ursache | Lösung |
|---|---|
| Lichtschranke verstellt oder verschmutzt | Justage prüfen ([Sketch 7](05-software.md#schritt-7--die-lichtschranke-justieren)) |
| Bilderkennung zu unempfindlich | `BEWEGUNG_SCHWELLE` senken (22 → 15) |
| Karte voll oder defekt | Systemzustand auf der Website ansehen |
| Nachts: zu dunkel | `IR_HELLIGKEIT` erhöhen |
| Es schaut dauernd jemand zu | Beim Streamen wird nicht aufgenommen — das ist Absicht |

### Zu viele Clips (100 pro Stunde)

Der Klassiker im Frühling: **Sonnenflecken wandern durch den Kasten**, Blattschatten
flackern.

1. `BEWEGUNG_SCHWELLE` erhöhen: 22 → 30 → 40
2. `CLIP_PAUSE_SEKUNDEN` erhöhen: 4 → 15
3. Notfalls **`BEWEGUNG_AN false`** — dann löst nur noch die Lichtschranke aus. Du
   verlierst Aufnahmen vom Füttern im Kasten, bekommst aber ausschließlich echte Anflüge.

> **Die Besuchszahlen bleiben davon unberührt** — *sofern du die Lichtschranke eingebaut
> und `LICHTSCHRANKE_AN true` gesetzt hast.* Die zählt selbst, und Sonnenflecken
> interessieren sie nicht. Genau dafür ist sie da.
>
> **Ohne Lichtschranke** — das ist die Voreinstellung — kommen die Besuchszahlen aus den
> Clips. Dann verfälschen Fehlauslöser sie mit, und `BEWEGUNG_AN false` würde das Zählen
> ganz beenden. Schritt 3 ist in dem Fall keine Option.

### Der Besuchszähler läuft ohne Vögel hoch

Die Lichtschranke flattert. Ursachen in dieser Reihenfolge:

1. **Sender und Empfänger schauen sich nicht genau an** → nachjustieren
2. **Sonne fällt schräg ins Loch und blendet den Empfänger** → Strohhalm als Blende
3. **Spinnennetz oder Blatt im Strahl** → putzen (nur im Wartungsfenster!)
4. `MIN_UNTERBRECHUNG_MS` erhöhen: 30 → 60

### Nachtbild ist weiß in der Mitte

Die IR-LEDs leuchten direkt ins Objektiv. Das ist ein **Bauproblem**, kein Softwareproblem:

- LEDs weiter weg von der Linse (mindestens 3 cm)
- kleine Pappblende um das Objektiv kleben
- `IR_HELLIGKEIT` reduzieren

### Nachtbild ist zu dunkel

In dieser Reihenfolge probieren:

1. `IR_HELLIGKEIT` erhöhen — bis 255 ist Luft, das ist mehr als das Dreifache
2. Bleibt es auch bei 255 dunkel, ist es nicht die Helligkeit, sondern der **MOSFET**:
   Das kleine Modul „HW-532“ bekommt an unseren 5 V nur die halbe Gate-Spannung ab und
   schaltet dann nicht voll durch. [Sketch 4](05-software.md#52-die-sieben-lern-sketches)
   zeigt es in zwei Minuten, [Schaltplan 3.6](03-schaltplan.md#36-das-unsichtbare-nachtlicht--mosfet-und-ir-leds)
   erklärt den Ausweg
3. Zwei LEDs mehr einbauen (6 statt 4) — für MOSFET und 5-V-Zweig kein Problem
4. Auf 850-nm-LEDs wechseln — der Vogel merkt es nicht, nur ein Mensch, der nachts direkt
   ins Einflugloch schaut, sieht ein schwaches rotes Glimmen

### Der Ton stimmt nicht

| Symptom | Ursache | Lösung |
|---|---|---|
| „Mikrofon startet nicht“ im Monitor | ESP32-Paket älter als Version 3.x | Boardverwalter → `esp32` aktualisieren |
| „Zu wenig PSRAM fuer den Tonring“ | PSRAM steht auf „Disabled“ | Werkzeuge → PSRAM → **OPI PSRAM** |
| Clip hat keine Tonspur | `TON_IN_CLIPS false`, oder Mikro nicht gestartet | Startmeldung `[Audio] Mikrofon laeuft` prüfen |
| Ton im Clip **kürzer** als das Bild | Karte zu langsam | `BILD_QUALITAET` erhöhen |
| Alles sehr leise | Kein Schallloch zur Box ([4.5b](04-bauplan.md#45b-das-mikrofon-hören-lassen)) | Loch bohren, dann `TON_VERSTAERKUNG` auf 3–4 |
| Es knackt und verzerrt | `TON_VERSTAERKUNG` zu hoch | zurück auf 1–2 |
| Knopf „Ton an“ fehlt | `TON_IM_STREAM false` oder Mikro aus | `config.h` prüfen |

### Akku wird nicht voll

| Ursache | Prüfen |
|---|---|
| **Panel verschmutzt** | Abwischen — bringt oft 20 % |
| **Verschattung dazugekommen** | Ist ein Ast gewachsen? Steht die Sonne im März tiefer? |
| **MPPT-Schalter falsch** | Muss auf **12V** stehen ([3.3](03-schaltplan.md#33-der-laderegler--das-herz-der-stromversorgung)) |
| **Rotes Lämpchen am Laderegler** | *Solar Warning* → Paneladern vertauscht, einfach tauschen |
| **Panelkabel lose** | Schraubklemme nachziehen |
| **Frost** | Unter 0 °C lädt ein Lithium-Akku nicht. Kein Defekt, sondern Physik |
| **Akku am Ende** | Nach vielen Jahren normal. Neuer LiPo, 14 € |

---

## 7.4 Der häufigste Fehler: ständige Neustarts

Wenn die Laufzeit auf der Website immer wieder auf 0 springt, bricht die Stromversorgung
kurz zusammen. In dieser Reihenfolge abarbeiten:

1. **Das USB-Kabel zwischen Laderegler und Board tauschen.** Das ist mit Abstand die
   häufigste Ursache. Viele billige Kabel haben sehr dünne Stromadern. Ein **kurzes,
   dickes** Kabel nehmen.
2. **Steckt es fest?** Auch ein wackeliger USB-C-Stecker macht genau dieses Bild.
3. **Akku prüfen.** Ein alter oder zu kleiner LiPo bricht bei Stromspitzen ein. Auf der
   Website unter Technische Details nachsehen, ob dort „Spannungseinbruch“ steht.
4. **Steht dort stattdessen „Watchdog“?** Dann hat sich das Programm aufgehängt, und es ist
   kein Stromproblem. Meist hilft ein Firmware-Update oder eine niedrigere `BILD_GROESSE`.

---

## 7.5 Ersatzteile, die man im Haus haben sollte

| Teil | Preis | Warum |
|---|---|---|
| **Kameramodul ohne IR-Filter** | 10 € | Das Teil, das am ehesten kaputtgeht |
| FPC-Verlängerung 24-polig | 3 € | Flachbandkabel brechen beim Basteln |
| microSD 32 GB High Endurance | 12 € | Verschleißteil, wenn auch ein langsames |
| Kurzes, dickes USB-A→C-Kabel | 5 € | Löst das Neustart-Problem |

Zusammen 30 € — und sie verhindern, dass mitten in der Brutzeit drei Wochen Stillstand
entstehen, weil eine Lieferung aus Asien unterwegs ist.

---

## 7.6 Wenn das Projekt größer werden soll

Der ehrliche Ausblick: Dieser Aufbau ist am Ende seiner Möglichkeiten, wenn es um
**Artenerkennung** und **Gesangsbestimmung** geht. Das ist keine Frage besserer Software,
sondern von Rechenleistung — und damit von Strom
([1.7](01-ueberblick.md#17-was-bewusst-fehlt--und-warum)).

Der nächste Schritt wäre ein **Raspberry Pi 4 oder 5 im Haus am Stromkabel**:

| Was er hinzufügt | Wie |
|---|---|
| **Gesang mit Artnamen** | [BirdNET-Go](https://github.com/tphakala/birdnet-go), 6000+ Arten, USB-Mikrofon am Fenster. Hört den Garten genauso gut wie am Kasten |
| **Echte Artenerkennung** | Zweite Kamera am **Futterhaus**: Farbe, Tageslicht, Seitenansicht → 20+ Arten |
| **Dauerarchiv** | Festplatte statt Ringspeicher |
| **Statistik über Jahre** | Die `tage.csv` einfach dorthin kopieren und weiterführen |

Der Nistkasten bleibt dabei **unverändert**. Er ist so gebaut, dass er auch dann noch Sinn
hat: Er ist das Gerät, das dort hängt, wo kein Kabel hinkommt.

---

## 7.7 Was ein Kind nach dem Projekt können sollte

Eine schöne Prüfliste. Wenn all das sitzt, hat es wirklich etwas gelernt:

- [ ] Die Website aufrufen und erklären, was die zwei Balkendiagramme bedeuten
- [ ] Einen Clip abspielen und den **Vorlauf** darin zeigen
- [ ] In `config.h` die Empfindlichkeit ändern und neu hochladen
- [ ] Am Akku-Pfeil erkennen, ob die Sonne reicht
- [ ] Erklären, warum die IR-LEDs für uns unsichtbar sind
- [ ] Erklären, warum die Lichtschranke genauer zählt als die Bilderkennung
- [ ] Erklären, warum alle schwarzen Kabel zusammenhängen müssen
- [ ] Erklären, warum der Kasten ab März zubleibt
- [ ] Den Seriellen Monitor öffnen und die Meldungen vorlesen

Der letzte Punkt ist der wertvollste. Wer Startmeldungen lesen kann, findet Fehler selbst —
und braucht keinen Erwachsenen mehr.

---

## 7.8 Notfallkarte zum Ausdrucken

```
 ┌────────────────────────────────────────────────────────┐
 │  BIRDYCAM — SCHNELLHILFE                               │
 ├────────────────────────────────────────────────────────┤
 │  Website:   http://birdycam.local/                     │
 │  Eigenes WLAN: BirdyCam / vogelhaus                    │
 │               dann http://192.168.4.1                  │
 │                                                        │
 │  NICHT ERREICHBAR?                                     │
 │    1. Bei Sonne nochmal probieren (Akku war leer)      │
 │    2. IP-Adresse statt .local versuchen                │
 │    3. Powerbank an die Notlade-Buchse                  │
 │    4. Box öffnen, USB an den Computer,                 │
 │       Seriellen Monitor lesen (115200 Baud)            │
 │                                                        │
 │  Laufzeit springt auf 0?  -> USB-Kabel tauschen        │
 │  Zu viele Clips?          -> BEWEGUNG_SCHWELLE hoch    │
 │  Keine Clips?             -> Lichtschranke justieren   │
 │  Nachtbild dunkel?        -> IR_HELLIGKEIT hoch        │
 │  Nachtbild weiß in Mitte? -> LEDs weiter von der Linse │
 │  Akku wird nicht voll?    -> MPPT-Schalter auf 12V,    │
 │                              Panel putzen, Schatten?   │
 │  Rotes Lämpchen (Solar)?  -> Paneladern vertauscht     │
 │                                                        │
 │  ⚠️  KASTEN VON MÄRZ BIS AUGUST NICHT ÖFFNEN            │
 └────────────────────────────────────────────────────────┘
```

---

← Zurück zur [Übersicht](../README.md) · Zum Bestellen: [8. Bestellliste](08-bestellliste.md)
