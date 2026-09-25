# 4. Bauplan — Einbau in den Nistkasten

> 🚦 **Bevor irgendetwas gebohrt wird:** Dieses Kapitel kommt erst dran, wenn alles aus
> [Kapitel 5](05-software.md) **auf dem Tisch** funktioniert hat — inklusive der zwei
> Wochen Probebetrieb.
>
> **Gebaut wird September bis Februar. Nie im März oder später.** Ein eingebauter Fehler
> kostet ein ganzes Jahr, weil ein belegter Kasten nicht geöffnet werden darf
> ([1.8](01-ueberblick.md#18-rechtliches-und-tierschutz)).

---

## 4.1 Das Grundprinzip: der Nistraum bleibt leer

Nur vier Dinge dürfen in den Kasten hinein:

1. das Kameramodul (etwa 2,5 × 2,5 cm, im Deckel)
2. vier IR-LEDs (im Deckel)
3. die Lichtschranke (seitlich am Einflugloch)
4. Kabel

Alles andere — Bastelcomputer, Akku, Laderegler — sitzt in einer Box **außen an der Wand**.
Das hat vier Gründe: keine Wärme im Nest, kein Geruch, der Akku bleibt tauschbar, ohne den
Kasten zu öffnen, und die Elektronik bleibt trocken.

> 🧪 **Es geht auch umgekehrt.** Wer die Box **in** den Kasten bauen will, so dass von außen
> nur noch das Panelkabel hineinführt, findet das durchgerechnet in
> [4.10 Variante B](#410-variante-b--alles-im-kasten-kamera-hinter-dem-klardeckel) — mitsamt
> den drei Dingen, die das kostet. Alles bis [4.9](#49-was-man-am-kasten-selbst-nicht-verändert)
> beschreibt **Variante A**.

```
                        ☀️ Solarpanel (an der sonnigsten Stelle
                           im Garten, per Kabel — NICHT am Kasten)
                              ╱
                             ╱  Kabel
      ┌────────────────────┐╱
      │  Elektronikbox     │      ← IP65, Nordseite, im Schatten
      │  XIAO · Laderegler │        Akku und USB bleiben zugänglich
      │  · Akku            │
      └──────┬─────┬───────┘
   ═══════════│═════│════════════   ← Kastendeckel (aufklappbar)
              │     │
     Kamera-  │     │  IR-LEDs + Lichtschranke
     Flachband▼     ▼
      ┌───────────────────────┐
      │  📷        ✦    ✦     │      ← Deckelunterseite:
      │        ✦        ✦     │        Kamera mittig, 4 LEDs außen
      │                       │
      │        Nistraum       │  ca. 20–25 cm
      │        (leer!)        │
      │                       │
      │   ═══○═══  ← Lichtschranke quer im Einflugloch
      │                       │
      └───────────────────────┘
              Nestboden
```

---

## 4.2 Werkzeug für diesen Teil

| Werkzeug | Wofür | Kind darf? |
|---|---|---|
| Akkuschrauber + Holzbohrer 3, 5, 6, 12 mm | Löcher für Linse, LEDs, Kabel, Mikrofon | mit Hilfe |
| Forstnerbohrer 20 mm *(optional)* | saubere Senkung für die Acrylscheibe | Erwachsener |
| Laubsäge oder Cutter | Acrylglas zuschneiden | Erwachsener |
| Schleifpapier 120er | Bohrlöcher entgraten | ✅ ja |
| Heißklebepistole | LEDs, Acryl, Lichtschranke fixieren | ✅ mit Aufsicht |
| Bleistift, Lineal | Anreißen | ✅ ja |
| Handy | IR-Test, Bildkontrolle | ✅ ja ⭐ |

---

## 4.3 Der Deckel

### Maße anreißen

Deckel abnehmen, **Innenseite nach oben**. Mittelpunkt finden: die beiden Diagonalen
kreuzen.

| Bohrung | Ø | Position | Zweck |
|---|---|---|---|
| **L** Linse | 12 mm | genau in der Mitte | Blickfeld der Kamera |
| **K** Kabel | 12 mm | 4 cm von L, Richtung Rückwand | Kamerakabel + IR-Litzen |
| **1–4** LEDs | 5 mm | vier Ecken, je 3 cm vom Rand | IR-Beleuchtung |

```
        Deckel von INNEN gesehen (Draufsicht)

     ┌─────────────────────────────────────┐
     │                                     │
     │   ✦1                          ✦2    │  ← 3 cm vom Rand
     │                                     │
     │              ● L                    │  ← Linse, Mitte
     │              ○ K   ← 4 cm hinter L  │
     │                                     │
     │   ✦3                          ✦4    │
     │                                     │
     └─────────────────────────────────────┘
                  ↑ Rückwand
```

> ⚠️ **Zur LED-Position:** Die LEDs müssen **mindestens 3 cm** von der Linse entfernt sein.
> Sitzen sie zu nah, leuchten sie direkt ins Objektiv, und das Nachtbild wird ein weißer
> Nebel — derselbe Effekt wie Blitzlicht auf einer Fensterscheibe.

### Die Kameraöffnung bauen

Von **innen** nach außen gearbeitet:

1. Loch **L** mit 12 mm durchbohren, beide Seiten entgraten.
2. Acrylglas auf **3 × 3 cm** zuschneiden. Die Schutzfolie **erst ganz zum Schluss**
   abziehen.
3. Die Acrylscheibe **von innen** über das Loch kleben — Heißkleber nur am Rand, dünn und
   **rundherum geschlossen**. Das ist die Dichtung gegen feuchte Luft.
4. Das Kameramodul **von außen** so über das Loch legen, dass die Linse mittig in die
   12-mm-Öffnung schaut. Mit zwei Punkten Heißkleber an den **Ecken der Platine** fixieren
   — **nie auf der Linse oder dem Sensor**.

```
   Querschnitt durch die Kameraöffnung:

        Elektronikbox oben
   ═══════════════════════════════
        ┌──────────┐
        │ Kamera-  │              ← außen auf dem Deckel
        │ modul    │
        └────┬─────┘
   ─────────┤ ├──────────────────  ← Deckel, 12 mm Bohrung
        ▓▓▓▓▓▓▓▓▓▓▓                ← Acrylscheibe, innen aufgeklebt
   ═══════════════════════════════
        ↓ Blickrichtung
          Nistraum
```

**Warum Acryl und nicht Glas?** Normales Fensterglas dämpft Infrarot merklich. Acryl lässt
940 nm gut durch — und es splittert nicht.

**Warum die Scheibe innen und die Kamera außen?** Damit ein Vogel niemals an Elektronik oder
Kabel kommt, und damit sich Kondenswasser nicht auf dem Sensor niederschlägt.

### Die IR-LEDs setzen

1. Die vier 5-mm-Löcher bohren.
2. LED-Module von außen einstecken, die LED-Kuppe schaut in den Nistraum.
3. Leicht **zur Mitte hin neigen** (etwa 15°), damit sich die Lichtkegel über dem Nest
   treffen.
4. Mit Heißkleber fixieren, die Bohrung rundherum abdichten.
5. Alle vier LEDs **parallel** verkabeln — alle Plus zusammen, alle Minus zusammen
   ([Schaltplan 3.6](03-schaltplan.md#36-das-unsichtbare-nachtlicht--mosfet-und-ir-leds)) —
   und die zwei Litzen durch Loch **K** nach oben führen.

> ⚠️ **Nur wenn du nackte 5-mm-LEDs verbaust** (oder Module ohne bestückten Vorwiderstand,
> etwa den KY-005): Dann gehört in **jeden** der vier Plus-Zweige ein eigener
> **Vorwiderstand**, 180 oder 220 Ω. Ohne ihn schmort das MOSFET-Modul, während die LEDs
> munter weiterleuchten. Rechnung, Bild und Klemmenliste stehen in
> [Schaltplan 3.6](03-schaltplan.md#nackte-leds-statt-module-dann-brauchst-du-vier-vorwiderstände).
>
> **Für den Deckel heißt das:** Vier Widerstände und **sechs** Klemmen kommen mit hier
> hinein — ein 5-fach-Verteiler für Plus, vier 2-fach-Klemmen (je Widerstand + LED) und ein
> 5-fach-Sammler für Minus. Nach oben durch Loch **K** gehen trotzdem nur die gewohnten
> zwei Litzen.
>
> Bau das **fertig und probiere es aus, bevor du die LEDs einklebst** — danach kommst du an
> die Beinchen nicht mehr heran. Wenn alles leuchtet: die Klemmen mit einem Tupfer
> Heißkleber gegen die Deckelunterseite fixieren, damit nichts wackelt.

### Die Kabeldurchführung K

Loch **K** ist der einzige Weg für Kamerakabel, IR-Litzen und Lichtschranken-Kabel.
Danach:

- Das **Flachbandkabel** der Kamera in einem **sanften Bogen** legen — nie gespannt, nie
  geknickt.
- **Zugentlastung:** direkt über dem Loch einen Kabelbinder um die *Litzen* legen (nicht um
  das Flachbandkabel!) und ihn an einer kleinen Schraube im Deckel einhängen.
- **Schlaufe lassen!** Der Deckel muss im Herbst zum Reinigen aufgehen können, ohne am
  Kabel zu ziehen.
- Das Loch von oben mit Heißkleber oder Silikon abdichten — **nicht** die Kabel eingießen,
  sondern nur den Spalt füllen.

---

## 4.4 Die Lichtschranke einbauen

Der Teil, der die Statistik ehrlich macht — und der einzige Eingriff an der Vorderwand.

> **Dieses Kapitel darfst du überspringen.** Die Lichtschranke ist optional und in der
> Firmware ab Werk abgeschaltet (`LICHTSCHRANKE_AN false`). Ohne sie zählt die
> Bilderkennung — ungenauer, aber der Kasten funktioniert vollständig. Weil der Einbau
> nur zwei kleine Bohrungen in der Vorderwand braucht, lässt er sich auch in einem
> späteren Jahr nachholen, ohne etwas aufzutrennen.

![Die Lichtschranke im Einflugloch](bilder/lichtschranke.svg)

**Vorgehen:**

1. Zwei Bohrungen (Ø nach Bauteil, meist 3–5 mm) **seitlich neben dem Einflugloch**, genau
   gegenüberliegend, etwa **5 mm unterhalb der Lochmitte**. Der Vogel fliegt eher
   mittig-oben durch, der Strahl soll ihn aber sicher treffen.
2. Sender und Empfänger von außen einstecken, so dass sie sich **genau anschauen**.
3. **Noch nicht kleben!** Erst mit [Sketch 7](05-software.md#52-die-sieben-lern-sketches)
   justieren, bis „frei“ stabil angezeigt wird und ein Finger sicher auslöst.
4. Dann mit Heißkleber fixieren und die Bohrungen abdichten.

**Drei Tipps aus der Praxis:**

- **Bauteile bündig oder minimal versenkt** einsetzen — nichts darf in den Flugweg ragen.
- **Streulicht abschirmen:** Ein 5 mm langes Stück Strohhalm oder Schrumpfschlauch über den
  Empfänger schieben. Sonne, die schräg ins Loch fällt, blendet ihn sonst.
- **Kabel innen an der Wand entlang** nach oben zum Deckelloch **K** führen, mit
  Heißklebepunkten fixieren. Keine Schlaufen, an denen ein Vogel zupfen kann.

> ⚠️ **Das Einflugloch wird nicht verändert.** Nicht größer bohren, nicht anschrägen. Sein
> Durchmesser bestimmt, welche Art einzieht (28 mm Blaumeise, 32 mm Kohlmeise), und hält
> Nesträuber draußen.

---

## 4.5 Die Elektronikbox

Die IP65-Box kommt an die **Nordseite** des Kastens oder auf den Deckel — nie in die Sonne.
Ein Akku über 45 °C altert schnell.

```
   Box ca. 120 × 80 × 50 mm:

   ┌──────────────────────────────────────────┐
   │  [M12] ← Panelkabel   [M12] ← Notladung  │
   │                                          │
   │  ┌───────────────────┐  ┌─────────────┐  │
   │  │ Waveshare         │  │ 🔋 LiPo     │  │
   │  │ Solar Power       │  │ auf         │  │
   │  │ Manager           │  │ Klettband   │  │
   │  └───────────────────┘  └─────────────┘  │
   │                                          │
   │  ┌──────────┐  ┌──────────┐              │
   │  │ XIAO     │  │ MOSFET   │  🎤 ← Loch   │
   │  │ ESP32-S3 │  │ + Spg.-  │     zum      │
   │  └────┬─────┘  │  Sensor  │     Kasten   │
   │       ▲        └──────────┘              │
   │   USB-C erreichbar lassen!               │
   │  [Silikagel]                             │
   │  [M12] ← Kamerakabel, IR, Lichtschranke  │
   └──────────────────────────────────────────┘
```

**Sieben Regeln für die Box:**

1. **Der USB-C-Anschluss des XIAO muss erreichbar bleiben.** Dort steckt das Kabel vom
   Laderegler, und dort hängst du im Notfall den Computer an.
2. **Akku auf Klettband**, nicht festgeklebt und nicht mit Kabelbindern gequetscht. Er ist
   das Teil, das man am ehesten austauscht.
3. **Die Lämpchen des Ladereglers sichtbar lassen** — am besten so einbauen, dass man sie
   beim Öffnen der Box sofort sieht.
4. **Silikagel-Beutel** in eine Ecke legen. Nimmt Restfeuchte auf.
5. **Kabelverschraubungen nach unten**, nie nach oben. Wasser läuft nach unten ab.
6. **Tropfschlaufe:** Jedes Kabel vor dem Eintritt einen Bogen nach unten machen lassen.
7. **Die WLAN-Antenne des XIAO flach an die Innenseite des Deckels kleben** — mit einem
   Streifen doppelseitigem Klebeband, möglichst weit weg von Akku und Laderegler. Metall
   und Akku direkt davor schlucken das Signal. Das dünne Kabel dabei **nicht knicken** und
   nicht straff ziehen: Der u.FL-Stecker ist winzig und reißt sonst beim nächsten Öffnen
   der Box ab. Ohne Antenne ist die Kamera praktisch funkstumm
   ([Software 5.2](05-software.md#52-die-sieben-lern-sketches)).

```
   richtig:              falsch:
   Box                   Box
    │                     │
    └──┐              ────┘
       │  ← Wasser        ← Wasser läuft
       ▼    tropft ab       in die Box
```

### Die Notlade-Buchse

Der Retter für Regenwochen: ein kurzes **Micro-USB-Kabel** von der `USB IN`-Buchse des
Ladereglers nach außen durch eine M12-Verschraubung, mit Gummikappe drüber.

Bei Dauerregen einfach eine Powerbank anstecken — der Akku lädt sich, ohne dass die Box
geöffnet werden muss. Anders als bei einer reinen Notversorgung lädt hier wirklich der
**Akku**, nicht nur das Board.

### 4.5b Das Mikrofon hören lassen

Das Mikrofon sitzt **fest auf dem XIAO-Board**, und das liegt in der geschlossenen Box an
der Außenwand des Kastens. Ohne einen Weg für den Schall hört es gedämpften Garten und kaum
das Nest.

**Die Lösung nutzt aus, dass die Box direkt an der Kastenwand sitzt:**

```
   Nistkasten              Elektronikbox
   ┌────────────┐         ┌──────────────┐
   │            │  6 mm   │              │
   │   innen    │ ──○───→ │  🎤 XIAO      │
   │            │  Loch   │              │
   └────────────┘   ↑     └──────────────┘
                Moosgummi-Dichtung
                dazwischen (2–3 mm)
```

1. Ein **6-mm-Loch durch die Kastenwand** bohren, dort wo die Box sitzt — am besten in der
   oberen Hälfte, hinter dem Nest, nicht darüber.
2. Ein **gleich großes Loch in die Boxwand**, die an der Kastenwand anliegt.
3. Zwischen Box und Kasten einen **Streifen Moosgummi** mit passendem Loch — dichtet gegen
   Regen, ohne den Schall zu blockieren.
4. Boxseitig ein Stückchen **atmungsaktive Membran** (Gore-Tex-Rest, Vlies aus einem
   Belüftungsstopfen) über das Loch kleben. Lässt Schall durch, hält Spritzwasser und
   Insekten draußen.

Weil beide Löcher **einander zugewandt** sind und vom Wetter abgeschirmt liegen, bleibt die
Dichtigkeit nach außen erhalten. Das Loch geht nicht ins Freie, sondern von Box zu Kasten.

> **Kein Loch gebohrt?** Der Ton funktioniert trotzdem — er ist dann nur leiser und
> dumpfer, weil er über die Kabelverschraubungen kommt. Nachrüsten geht, aber nur zwischen
> September und Februar. **Wer Ton will, bohrt also beim Bauen.**

---

## 4.6 Solarpanel montieren

> ⭐ **Der wichtigste Satz des Kapitels: Das Panel gehört nicht an den Nistkasten.**

Nistkästen hängen gern halbschattig unter Bäumen — genau falsch für Solar. Das Panel kommt
per Kabel an die sonnigste erreichbare Stelle: Schuppendach, Zaunpfosten, Garagenwand,
Balkongeländer.

| Kriterium | Sollwert | Warum |
|---|---|---|
| Ausrichtung | **Süden** | maximale Tagesausbeute |
| Neigung | **30–40°** | guter Kompromiss Frühling/Sommer, und Regen wäscht es sauber |
| Verschattung | **keine, den ganzen Tag** | Ein Ast, der um 11 Uhr eine Ecke abschattet, kostet mehr Ertrag als man denkt |
| Höhe | ≥ 1,5 m | weniger Laub, weniger Schnee, weniger Neugier |
| Kabelquerschnitt | **0,75 mm²** reicht | bei 12 V fällt eine Verlängerung kaum ins Gewicht |
| Befestigung | Schrauben, keine Kabelbinder | ein 15-W-Panel wiegt ~1 kg und fängt Wind |

**Zum Kabel:** Bei 12 Volt darfst du entspannt verlängern. Bis etwa 10 Meter mit 0,75 mm²
verlierst du nur ein paar Zehntelvolt — und der Laderegler hat bis 6 Volt hinunter Luft.
Das war bei 5-Volt-Panels ganz anders und ist einer der Gründe für diese Bauweise.

> 🧒 **Kinderaufgabe mit echtem Ergebnis:** An einem sonnigen Wintertag stündlich notieren,
> wo im Garten Sonne ist und wo Schatten. Daraus wird die Panelposition **begründet** statt
> geraten. Das ist eine echte Messreihe, und das Ergebnis hält Jahre.

---

## 4.7 Endmontage — die Reihenfolge

> 🧪 Für **Variante B** gilt stattdessen die Reihenfolge in
> [4.10.5](#4105-der-einbau-ein-belüfteter-dachboden).

| # | Schritt | Prüfen, bevor es weitergeht |
|---|---|---|
| 1 | Deckel bohren, Acryl einkleben, trocknen lassen | Kleber wirklich hart? |
| 2 | Kamera + LEDs im Deckel montieren | Sitzt alles fest? |
| 3 | Mikrofonloch bohren, Moosgummi und Membran setzen | Beide Löcher zeigen aufeinander? |
| 4 | *(optional)* Lichtschranke einsetzen, **justieren**, dann erst kleben — und `LICHTSCHRANKE_AN true` setzen | Löst ein Finger sicher aus? |
| 5 | Kabel durch **K**, Zugentlastung, Schlaufe für den Deckel | Kein Knick im Flachbandkabel? |
| 6 | Box am Kasten befestigen, Verschraubungen setzen | Alle nach unten? |
| 7 | Alles verbinden — Reihenfolge: **Akku, dann Panel, dann Board** | MPPT-Schalter auf 12 V? Schalter auf ON? |
| 8 | Am Computer testen: Bild? LEDs? Lichtschranke? Website? | **Jetzt** ist der letzte einfache Moment für Korrekturen |
| 9 | Box schließen | Silikagel drin? |
| 10 | Panel montieren, Kabel verlegen | Tropfschlaufen? |
| 11 | Kasten aufhängen | Einflugloch nach **Ost/Südost**, nicht nach Westen (Wetterseite) |
| 12 | **Zwei Wochen Probebetrieb**, täglich Akkustand prüfen | Steigt der Akku tagsüber? |

**Schritt 12 ist nicht optional.** Er ist der Unterschied zwischen „hat funktioniert“ und
„hat im April funktioniert“.

---

## 4.8 Blickfeld richtig einstellen

Ein Modul mit 60–70° Bildwinkel deckt bei 22 cm Abstand rund 25 × 19 cm ab — also den
Kastenboden. Trotzdem lohnt sich Feinjustierung:

1. Kasten aufrecht hinstellen, Deckel drauf.
2. Ein Kuscheltier oder einen zerknüllten Papierball auf den Boden legen — als
   „Vogel-Platzhalter“.
3. Livestream auf dem Handy öffnen.
4. Kamera minimal drehen, bis der Boden mittig und vollständig im Bild ist.
5. **Erst dann** den zweiten Heißkleberpunkt setzen.

| Problem | Lösung |
|---|---|
| Bild zu dunkel | `IR_HELLIGKEIT` erhöhen |
| Bildmitte weiß überstrahlt | Die LEDs leuchten in die Linse — Abstand vergrößern oder eine kleine Pappblende ums Objektiv kleben |
| Bild auf dem Kopf | `BILD_DREHEN true` |
| Bild seitenverkehrt | `BILD_SPIEGELN true` |

---

## 4.9 Was man am Kasten selbst *nicht* verändert

Der Nistkasten war schon fertig, und das soll so bleiben:

- **Einflugloch nicht vergrößern.** Der Durchmesser bestimmt, welche Art einzieht, und hält
  Nesträuber draußen.
- **Keine Sitzstange** vor dem Loch — die ist eine Einladung für Katzen und Elstern.
- **Keine zusätzlichen Lüftungslöcher.** Der Kasten ist so ausgelegt, wie er ist.
- **Nicht innen streichen oder lackieren.** Ausgasende Lösemittel in einem Brutraum sind ein
  echtes Problem.
- **Das Reinigungskonzept erhalten.** Der Deckel muss im Herbst aufgehen, um das alte Nest
  zu entfernen. Deshalb die Kabelschlaufe.

---

## 4.10 Variante B — alles im Kasten, Kamera hinter dem Klardeckel

> 🧪 **Durchgerechnet, aber noch nicht gebaut.** Alle Maße unten sind aus Datenblättern
> zusammengerechnet, nicht an einem fertigen Kasten abgemessen. Wer zum ersten Mal baut,
> nimmt **Variante A** ([4.1](#41-das-grundprinzip-der-nistraum-bleibt-leer) bis
> [4.9](#49-was-man-am-kasten-selbst-nicht-verändert)) — die ist im Fehlerfall in zwei
> Minuten offen.

In Variante A hängt die Elektronikbox außen an der Wand. In **Variante B sitzt alles im
Kasten**, in einem eigenen kleinen Dachboden über dem Nistraum. Von außen führt nur noch
das Panelkabel hinein. Man sieht dem Kasten nichts an.

---

### Was das kostet — drei Punkte, bevor du weiterliest

**1. Kein USB-Zugang heißt: kein Update, kein Reset, keine SD-Karte — fünf Monate lang.**
Die Firmware in diesem Repo hat **heute kein OTA und keinen Watchdog**. Ein Hänger im April
ist damit endgültig, und die Saison ist vorbei. Gegenmaßnahmen stehen in
[4.10.6](#4106-vor-dem-zuschrauben-drei-versicherungen) — **die sind in dieser Variante
keine Kür.**

**2. Wärme.** Der Akku sitzt jetzt unter dem Dach statt an der Nordwand — genau die Stelle,
vor der [1.6](01-ueberblick.md) warnt. Über 45 °C altert er schnell. Beherrschbar, aber nur
mit dem **belüfteten** Dachboden aus [4.10.5](#4105-der-einbau-ein-belüfteter-dachboden).

**3. Der 10-000-mAh-Akku passt nicht mehr rein.** Bei 45 mm Bautiefe ist bei 5000 mAh
Schluss. Die lohnendste Ausbaustufe aus [2.4](02-stueckliste.md) fällt damit weg.

---

### 4.10.1 Die Box

**Empfehlung: Hammond `1555FL2GY` (Gehäuse) + `1554FCL` (Klardeckel) → 120 × 90 × 45 mm.**

| | |
|---|---|
| Maß außen | 120 × 90 × 45 mm |
| Material | Polycarbonat, UV-stabilisiert; Klardeckel UL94V-0 |
| Dichtung | Silikon, Nut und Feder, Edelstahlschrauben — mit Dichtung bis IP68 geprüft |
| Temperatur | −40 bis +110 °C — im Dachboden nicht unwichtig |
| Klardeckel | 15 mm tief. **Das ist der Platz, in dem die Kamera sitzt** |
| Preis | zusammen ≈ 20–28 €. Der Deckel wird **separat** bestellt und passt auf die Größen F, FL und G |

> ⚠️ **Die billigen ABS-Klardeckel-Dosen fallen fast alle durch.** Die gängigen Maße sind
> 125 × 80 × **32** (zu flach) und 158 × 90 × 46 (zu lang). Wenn du etwas anderes nimmst,
> prüf diese drei Zahlen:
>
> - Grundfläche **innen mindestens 110 × 80 mm**
> - Tiefe **innen mindestens 36 mm** ← daran scheitern die flachen
> - Deckel **glasklar**, nicht mattiert, nicht geriffelt — er ist das Objektivfenster

---

### 4.10.2 Der Klardeckel *ist* das Kamerafenster

Das ist der Kern der Variante. Solange die Kamera am Flachbandkabel aus der Box herausgeht,
wäre ein durchsichtiger Deckel bloß Dekoration — die Laderegler-Lämpchen siehst du im
geschlossenen Kasten ja ohnehin nicht, und den Akkustand liefert der Spannungssensor an die
Website.

Also andersherum: **Box mit dem Deckel nach unten in die Kastendecke, der XIAO innen mit dem
Objektiv direkt ans Fenster.**

Was das einbringt:

- **Kein FPC-Kabel verlässt die Box.** Die 15-cm-Grenze und das Knickrisiko aus
  [4.3](#43-der-deckel) sind weg — das war bisher das empfindlichste Teil im ganzen Aufbau.
- **Kein 12-mm-Loch, keine eingeklebte Acrylscheibe.** Eine Dichtstelle weniger.
- **Nichts ragt in den Nistraum**, an dem ein Vogel zupfen könnte.
- **Das Mikrofon bekommt direkten Zugang** ([4.10.4](#4104-die-durchführungen)). Die
  Moosgummi-Konstruktion aus [4.5b](#45b-das-mikrofon-hören-lassen) entfällt ersatzlos.

**Zwei Bedingungen, ohne die es nicht funktioniert:**

1. **Die IR-LEDs bleiben draußen.** Sie kommen wie gehabt in die Decke, **≥ 3 cm** vom
   Objektiv. Hinter demselben Fenster wie die Linse geben sie nur weißen Nebel — derselbe
   Effekt wie Blitzlicht auf einer Fensterscheibe.
2. **Blendring ums Objektiv.** Ein Ring aus schwarzem Moosgummi (außen ~15 mm, innen ~8 mm),
   der beim Zuschrauben leicht gegen die Deckelinnenseite drückt. Der killt die
   Innenreflexion des Fensters. Die Deckelinnenseite rundherum **mattschwarz**, nur das
   Sichtfeld frei.

---

### 4.10.3 Innenaufbau — es wird gestapelt

Innen hast du etwa **112 × 82 × 37 mm** (Wandstärke abgezogen — im Datenblatt gegenprüfen).
Die Teile, die da hinein müssen:

| Teil | Maß |
|---|---|
| Waveshare Solar Power Manager (E5) | 65,2 × 56,2 × **22,9** mm |
| LiPo 5000 mAh 1S (E7) | typisch 90 × 55 × 11 mm |
| XIAO ESP32-S3 Sense mit Kameramodul (E1 + E2) | 21 × 17,5 × ~12 mm |
| MOSFET HW-532, Bauform A (E10) | 23 × 17 mm |
| Spannungssensor (E8) | 13 × 22 mm |

> ⚠️ **Den Akku vor dem Kauf nachmessen lassen.** „5000 mAh 1S“ gibt es in einem Dutzend
> Bauformen. Hinein passt nur bis **95 × 58 × 12 mm**. Steht das Maß nicht in der
> Artikelbeschreibung, nimm einen anderen Anbieter.

Nebeneinander geht das nicht auf (65 + 90 = 155 mm bei 112 mm Platz). Also **zwei Ebenen**:

```
   Blick von unten durch den Klardeckel.
   Der Deckel ist UNTEN (zum Nistraum), der Boden der Box zeigt
   nach OBEN in den Dachboden.

   ┌───────────────────────────────────────────────┐
   │ ┌───────────────────────────┐ ┌─────────────┐ │
   │ │  Akku 5000 mAh  90×55×11  │ │ XIAO        │ │ ← liegt am Deckel,
   │ │  flach, auf Klettband     │ │ 🔵 Linse    │ │   Linse mit Blendring
   │ │                           │ └─────────────┘ │   ans Fenster
   │ │  DARÜBER, auf 2-mm-       │ ┌────┐ ┌──────┐ │
   │ │  Zwischenboden:           │ │MOS-│ │Spg.- │ │
   │ │  ▓ Waveshare 65×56×23 ▓   │ │FET │ │Sensor│ │
   │ └───────────────────────────┘ └────┘ └──────┘ │
   │ [Silikagel]   [M12] [M12] [M12]               │
   └───────────────────────────────────────────────┘
                     ↑ zur Kastenrückwand
```

**Die Höhenrechnung, und warum sie nicht verhandelbar ist:**

```
   Akku                11 mm
   Zwischenboden        2 mm
   Waveshare           23 mm
   ───────────────────────────
   zusammen            36 mm    von 37 mm verfügbar
```

Deshalb der flache Akku, deshalb kein 10-000er, und deshalb die kleine MOSFET-Bauform A.

**Vier Details, die einem sonst erst beim Zuschrauben auffallen:**

- **Der Zwischenboden** ist ein Stück 2-mm-PVC oder Platinenmaterial auf vier
  M3-Distanzhülsen **neben** dem Akku. Er darf **nicht** auf dem Akku aufliegen — LiPos
  dürfen sich ausdehnen, und ein eingeklemmter Akku ist ein kaputter Akku.
- **USB A → C mit 90°-Winkelstecker**, 10–15 cm (statt E17). Ein gerader Stecker braucht
  4 cm Biegeradius, die hast du nicht.
- **Die WLAN-Antenne** flach an die Boxwand *gegenüber* von Akku und Laderegler — **nicht**
  auf den Klardeckel, da sitzt die Kamera. Ansonsten gilt Regel 7 aus
  [4.5](#45-die-elektronikbox) unverändert: den u.FL-Stecker nicht knicken.
- **Silikagel** in die freie Ecke. Bei fünf Monaten ohne Öffnen doppelt wichtig.

---

### 4.10.4 Die Durchführungen

Drei M12-Verschraubungen (E14) an der Stirnseite — auf 90 mm Breite passen genau drei:

| # | Kabel | Wohin |
|---|---|---|
| **1** | Panelkabel, 2-adrig | durch den Dachboden und die **Kastenrückwand nach draußen** |
| **2** | **Steuerkabel 6-adrig** (z. B. LiYY 6 × 0,14) | in den Nistraum: `IR+`, `IR−`, `LS-VCC`, `LS-GND`, `LS-OUT`, eine Ader Reserve |
| **3** | kurzes USB-Kabel von `USB IN` des Ladereglers | nach draußen, Gummikappe drüber → **Notladung** |

> ⭐ **Nr. 2 ist der Punkt, an dem man es falsch macht.** Ein Mehraderkabel durch *eine*
> Verschraubung dichtet sauber. Fünf Einzellitzen durch dieselbe Verschraubung dichten
> **nicht** — die Dichtung braucht einen runden Querschnitt zum Anpressen. Aufgeteilt wird
> innen, in der Schraubklemme des Spannungssensors bzw. am Masse-Sammelpunkt
> ([3.6](03-schaltplan.md#36-das-unsichtbare-nachtlicht--mosfet-und-ir-leds)).

> 💡 **Nr. 3 würde ich trotz „nur das Panel von außen“ mitnehmen.** Es geht denselben Weg
> durch die Rückwand wie das Panelkabel und kostet 2 €. Bei einer Regenwoche im April ist
> es der Unterschied zwischen „Powerbank anstecken“ und „Saison vorbei“.

**Das Mikrofonloch kommt in den Deckel:** 6 mm, genau unter dem PDM-Mikrofon der
Sense-Platine, von innen eine atmungsaktive Membran drübergeklebt (Gore-Tex-Rest oder Vlies
aus einem Belüftungsstopfen). Das Loch geht direkt in den Nistraum — akustisch ist das
deutlich besser als alles in Variante A. Nach außen bleibt trotzdem alles dicht, weil der
Nistraum selbst schon im Wetterschutz liegt.

---

### 4.10.5 Der Einbau: ein belüfteter Dachboden

Die Box wird **nicht** unter den Deckel geschraubt. Sie kommt in eine Zwischendecke — so
bleibt der Nistraum glatt, und die Wärme bleibt draußen.

```
   ═══════════════════════════════════════════  ← Original-Deckel, bleibt Wetterdeckel
        ↑ 12 mm Luft   ○ ← 2× Ø 8 mm Lüftung seitlich, mit Fliegengitter
   ┌──────────────┐          DACHBODEN
   │  Boxkörper   │          (Kabel, Luft, Wärmepuffer)
   ──┤ Klardeckel ├──────────────────────────   ← Zwischendecke, 10-mm-Brett
       ✦    🔵    ✦           ✦    ✦           ← IR-LEDs in der Zwischendecke,
                                                 ≥ 3 cm vom Objektiv
              Nistraum (leer!)

       ═══○═══  ← Lichtschranke im Einflugloch

              Nestboden
```

**Reihenfolge:**

| # | Schritt | Prüfen, bevor es weitergeht |
|---|---|---|
| 1 | **Zwischendecke** aus 10-mm-Brett auf vier Leisten, **50 mm unter** dem Original-Deckel | Abstand Einflugloch → Nestboden bleibt **unverändert** |
| 2 | **Ausschnitt 122 × 92 mm** hineinsägen, Box von oben einsetzen | Klardeckel bündig mit der Nistraumdecke? |
| 3 | **Halten** über zwei Alu- oder Holzleisten quer über den Boxboden, in die Zwischendecke geschraubt | **Nicht durch die Box bohren** — die Dichtung bleibt unangetastet |
| 4 | **Zwei Ø-8-mm-Lüftungslöcher** in die Seitenwände des Dachbodens, unter dem Dachüberstand, mit Fliegengitter | Kommt dort wirklich kein Regen hin? |
| 5 | **IR-LEDs** in die Zwischendecke ([4.3](#die-ir-leds-setzen)), Litzen nach oben, ins Steuerkabel | ≥ 3 cm vom Objektiv, 15° zur Mitte geneigt? |
| 6 | **Lichtschranke** wie in [4.4](#44-die-lichtschranke-einbauen), Kabel an der Wand hoch durch ein kleines Loch in der Zwischendecke | Mit Sketch 7 justiert, **bevor** geklebt wird? |
| 7 | Alles verbinden und **auf dem Tisch komplett testen** — Bild, LEDs, Ton, Lichtschranke, Website, OTA | Läuft OTA wirklich? Siehe [4.10.6](#4106-vor-dem-zuschrauben-drei-versicherungen) |
| 8 | Silikagel rein, Box zuschrauben | Blendring sitzt am Fenster? |
| 9 | Panel montieren ([4.6](#46-solarpanel-montieren)), Kasten aufhängen, **zwei Wochen Probebetrieb** | Steigt der Akku tagsüber? |

> ⭐ **Schritt 4 ist die Antwort auf das Wärmeproblem.** Der Dachboden ist kein Brutraum —
> da darf durchziehen. Das ist der Unterschied zwischen „Akku hält vier Jahre“ und „Akku
> hält eine Saison“. [4.9](#49-was-man-am-kasten-selbst-nicht-verändert) verbietet
> zusätzliche Lüftungslöcher im **Nistraum** — der Dachboden ist davon nicht betroffen.

> 💡 **Ausrichten nach dem Objektiv, nicht nach der Box.** Der XIAO sitzt seitlich in der
> Box. Leg sie so, dass die **Linse** über der Nistraummitte steht. Feinjustage wie in
> [4.8](#48-blickfeld-richtig-einstellen) — nur dass du hier die ganze Box drehst.

---

### 4.10.6 Vor dem Zuschrauben: drei Versicherungen

Weil du fünf Monate nicht mehr an die Hardware kommst, sind diese drei Dinge in Variante B
**keine Kür**:

| Was | Wofür | Aufwand |
|---|---|---|
| **OTA-Update** — Firmware über WLAN nachladen | Ohne das ist jeder Fehler bis August eingefroren. Der Webserver läuft ohnehin schon | Software |
| **Watchdog + Nacht-Neustart** — `esp_task_wdt` plus geplanter Neustart um 3 Uhr | Fängt Hänger ab, die dir sonst die Brutzeit kosten | Software, klein |
| **Reed-Kontakt auf `RST`** — Reedschalter zwischen `RST` und `GND` in der Box | Magnet von außen an die Kastenwand = Hardware-Reset, ohne zu öffnen | ~1 €, zwei Lötpunkte |

> ⚠️ **Die ersten beiden sind heute nicht in der Firmware.** In
> [`software/firmware/birdycam/`](../software/firmware/birdycam/) gibt es weder `Update.h`
> noch `esp_task_wdt`. Wer Variante B baut, baut das vorher ein — sonst ist der einzige
> Notausstieg der Akku, der irgendwann leer wird, und der Neustart daraus
> (`NOTAUS_SCHLAF_MINUTEN`, [config.h](../software/firmware/birdycam/config.h)).

---

### 4.10.7 Was sich gegenüber Variante A ändert

**Unverändert gültig:** [4.2 Werkzeug](#42-werkzeug-für-diesen-teil) ·
[4.4 Lichtschranke](#44-die-lichtschranke-einbauen) ·
[4.6 Solarpanel](#46-solarpanel-montieren) ·
[4.8 Blickfeld](#48-blickfeld-richtig-einstellen) ·
[4.9 Was man nicht verändert](#49-was-man-am-kasten-selbst-nicht-verändert) · der ganze
[Schaltplan](03-schaltplan.md).

**Ersetzt oder entfallen:**

| Kapitel | In Variante B |
|---|---|
| [4.3](#43-der-deckel) Linsenloch **L** + Acrylscheibe | **entfällt** — der Klardeckel ist das Fenster |
| [4.3](#43-der-deckel) Kabelloch **K** | wird zum kleinen Loch in der **Zwischendecke** für IR und Lichtschranke |
| [4.3](#die-ir-leds-setzen) IR-LEDs | wie gehabt, aber in der **Zwischendecke** statt im Deckel |
| [4.5](#45-die-elektronikbox) Box außen an der Wand | ersetzt durch [4.10.5](#4105-der-einbau-ein-belüfteter-dachboden) |
| [4.5](#45-die-elektronikbox) Regel 1 „USB-C erreichbar lassen“ | **nicht erfüllbar** — ersetzt durch OTA + Reed-Reset |
| [4.5](#45-die-elektronikbox) Regel 3 „Laderegler-Lämpchen sichtbar“ | **entfällt** — der Akkustand kommt über die Website |
| [4.5b](#45b-das-mikrofon-hören-lassen) Mikrofon, Moosgummi, zwei Löcher | **entfällt** — ein 6-mm-Loch mit Membran im Klardeckel |
| [4.7](#47-endmontage--die-reihenfolge) Endmontage | ersetzt durch die Tabelle in [4.10.5](#4105-der-einbau-ein-belüfteter-dachboden) |

**Stückliste-Delta** gegenüber [2.1](02-stueckliste.md#21-die-teileliste):

| | Änderung | Δ |
|---|---|---|
| **E4** FPC-Verlängerung | **entfällt** | −3 € |
| **E13** IP65-Gehäuse 120 × 80 × 50 | → **Hammond 1555FL2GY + Klardeckel 1554FCL** | +14 € |
| **E15** Acrylglas 3 mm | **entfällt** | −6 € |
| **E17** USB A → C, 30 cm | → dasselbe, aber **mit 90°-Winkelstecker, 10–15 cm** | 0 € |
| **E7** Ausbaustufe 10 000 mAh | **nicht mehr möglich** — passt nicht in 45 mm | — |
| *neu* | Steuerkabel LiYY 6 × 0,14, ~1 m | +3 € |
| *neu* | Reedschalter (Glasröhrchen, Schließer) | +1 € |
| *neu* | PVC-Platte 2 mm + 4 × M3-Distanzhülse 12 mm | +3 € |
| *neu* | Moosgummi schwarz (Blendring), Brett 10 mm + Leisten (Zwischendecke) | Restholz |
| | **unterm Strich** | **≈ +12 €** |

---

→ Weiter mit [5. Software](05-software.md)
