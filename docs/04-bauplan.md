# 4. Bauplan — Einbau in den Nistkasten

> 🚦 **Bevor irgendetwas gebohrt wird:** Dieses Kapitel wird erst ausgeführt, wenn
> [Tutorial Schritte 1–8](05-software-tutorial.md) **auf dem Tisch** funktioniert haben —
> inklusive der zwei Wochen Probebetrieb. Ein eingebauter Fehler kostet im Frühling ein
> ganzes Jahr, weil der belegte Kasten nicht geöffnet werden darf.
>
> **Bauzeit: September bis Februar.** Nie im März oder später.

Dieses Kapitel gilt für **beide Varianten**. Wo es Unterschiede gibt, steht es dabei.

---

## 4.1 Das Grundprinzip

Der Nistraum bleibt **leer**. Nur vier Dinge dürfen hinein:

1. das Kameramodul (2,5 × 2,5 cm, im Deckel)
2. vier IR-LEDs (im Deckel)
3. die Lichtschranke (seitlich am Einflugloch)
4. Kabel

Alles andere — Rechner, Akku, Laderegler — sitzt in einer Box **außen**. Das hat vier
Gründe: keine Wärme im Nest, kein Geruch, Akku tauschbar ohne den Kasten zu öffnen, und die
Elektronik bleibt trocken.

```
                        ☀️ Solarpanel (an der sonnigsten Stelle,
                           per Kabel, NICHT am Kasten)
                              ╱
                             ╱  Kabel 0,75 mm²
      ┌────────────────────┐╱
      │  Elektronikbox     │      ← IP65, Nordseite, im Schatten
      │  Pi · SSD · Akku · │        Akku und USB bleiben zugänglich
      │  Laderegler        │
      └──────┬─────┬───────┘
   ═══════════│═════│════════════   ← Kastendeckel (aufklappbar)
              │     │
       CSI    │     │  IR + Lichtschranke
              ▼     ▼
      ┌───────────────────────┐
      │  📷        ✦    ✦     │      ← Deckelunterseite:
      │        ✦        ✦     │        Kamera mittig, 4 LEDs außen
      │                       │
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

## 4.2 Werkzeug

| Werkzeug | Wofür | Kind darf? |
|---|---|---|
| Akkuschrauber + Holzbohrer 3, 5, 12 mm | Löcher für Linse, LEDs, Kabel | mit Hilfe |
| Forstnerbohrer 20 mm *(optional)* | saubere Senkung für die Acrylscheibe | Erwachsener |
| Laubsäge oder Cutter | Acrylglas zuschneiden | Erwachsener |
| Schleifpapier 120er | Bohrlöcher entgraten | ✅ ja |
| Heißklebepistole | LEDs, Acryl, Lichtschranke fixieren | ✅ mit Aufsicht |
| Bleistift, Lineal | Anreißen | ✅ ja |
| Handy | IR-Test, Bildkontrolle | ✅ ja ⭐ |

---

## 4.3 Der Deckel

### Maße anreißen

Deckel abnehmen, **Innenseite nach oben**. Mittelpunkt finden: Diagonalen kreuzen.

| Bohrung | Ø | Position | Zweck |
|---|---|---|---|
| **L** Linse | 12 mm | genau in der Mitte | Blickfeld der Kamera |
| **K** Kabel | 12 mm | 4 cm von L, Richtung Rückwand | CSI + IR-Litzen |
| **1–4** LEDs | 5 mm | 4 Ecken, je 3 cm vom Rand | IR-Beleuchtung |

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

**Wichtig zur LED-Position:** Die LEDs müssen **mindestens 3 cm** von der Linse entfernt
sein. Sitzen sie zu nah, leuchten sie direkt ins Objektiv und das Bild wird ein weißer
Nebel — derselbe Effekt wie Blitzlicht auf einer Fensterscheibe.

### Kameraöffnung bauen

Von **innen** nach außen gearbeitet:

1. Loch **L** mit 12 mm durchbohren, beide Seiten entgraten.
2. Acrylglas auf **3 × 3 cm** zuschneiden, Schutzfolie **erst zum Schluss** abziehen.
3. Acrylscheibe **von innen** über das Loch kleben — Heißkleber nur am Rand, dünn und
   **rundherum geschlossen** (das ist die Dichtung gegen feuchte Luft).
4. Kameramodul **von außen** so über das Loch legen, dass die Linse mittig in die
   12-mm-Öffnung schaut. Mit zwei Punkten Heißkleber an den *Ecken* der Platine fixieren —
   **nie auf der Linse oder dem Sensor**.

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

**Warum Acryl und nicht Glas?** Normales Fensterglas dämpft Infrarot merklich. Acryl (PMMA)
lässt 940 nm gut durch. Und es splittert nicht.

**Warum die Scheibe innen und die Kamera außen?** Damit ein Vogel niemals an Elektronik oder
Kabel kommt, und damit sich Kondenswasser nicht auf dem Sensor niederschlägt.

### IR-LEDs setzen

1. Die vier 5-mm-Löcher bohren.
2. LED-Module von außen einstecken, LED-Kuppe schaut in den Nistraum.
3. Leicht **zur Mitte hin neigen** (ca. 15°), damit sich die Lichtkegel über dem Nest
   treffen.
4. Mit Heißkleber fixieren, Bohrung rundherum abdichten.
5. Alle vier LEDs **parallel** verkabeln (alle Plus zusammen, alle Minus zusammen), die zwei
   Litzen durch Loch **K** nach oben führen.

### Kabeldurchführung K

Loch **K** ist der einzige Weg für CSI-Kabel, IR-Litzen und Lichtschranken-Kabel. Danach:

- CSI-Kabel mit einem **sanften Bogen** legen, nie gespannt und nie geknickt
- **Zugentlastung:** direkt über dem Loch einen Kabelbinder um die *Litzen* (nicht um das
  CSI-Flachband!) und ihn an einer kleinen Schraube im Deckel einhängen
- **Schlaufe lassen!** Der Deckel muss im Herbst zum Reinigen aufgehen, ohne am Kabel zu
  ziehen
- Loch von oben mit Heißkleber oder Silikon abdichten — **nicht** die Kabel eingießen,
  sondern nur den Spalt füllen

---

## 4.4 Die Lichtschranke einbauen

Der Teil, der die Statistik ehrlich macht — und der einzige Eingriff an der Vorderwand.

```
      Vorderwand von außen:              Schnitt von oben:

     ┌─────────────────┐            ┌──────────────────────┐
     │                 │            │                      │
     │       ○         │      ┌─────┤ ●              ●     ├─────┐
     │   ▪       ▪     │      │ Send│   ────────────►      │ Empf│
     │   ↑       ↑     │      └─────┤   IR-Strahl          ├─────┘
     │ Sender  Empf.   │            └──────────────────────┘
     └─────────────────┘              5 mm unter dem Loch-
      Ø 3–5 mm Bohrungen              Mittelpunkt
```

**Vorgehen:**

1. Zwei Bohrungen (Ø nach Bauteil, meist 3–5 mm) **seitlich neben dem Einflugloch**,
   genau gegenüberliegend, ca. **5 mm unterhalb** der Lochmitte. Der Vogel fliegt eher
   mittig-oben durch, der Strahl soll ihn aber sicher treffen.
2. Sender und Empfänger von außen einstecken, so dass sie sich **genau anschauen**.
3. **Noch nicht kleben!** Erst mit [Tutorial Schritt 7](05-software-tutorial.md#schritt-7--die-lichtschranke-justieren)
   justieren, bis „frei" stabil angezeigt wird und ein Finger sicher auslöst.
4. Dann mit Heißkleber fixieren und die Bohrungen abdichten.

**Drei Tipps aus der Praxis:**

- **Bauteile bündig oder minimal versenkt** einsetzen — nichts darf in den Flugweg ragen.
- **Streulicht abschirmen:** Ein 5 mm langes Stück Strohhalm oder Schrumpfschlauch über den
  Empfänger schieben. Sonne, die schräg ins Loch fällt, blendet ihn sonst.
- **Kabel innen an der Wand entlang** nach oben zum Deckelloch **K** führen, mit
  Heißklebepunkten fixieren. Keine Schlaufen, an denen ein Vogel zupfen kann.

> ⚠️ **Das Einflugloch wird nicht verändert.** Nicht größer bohren, nicht anschrägen. Sein
> Durchmesser bestimmt, welche Art einzieht (28 mm Blaumeise, 32 mm Kohlmeise) und hält
> Nesträuber draußen.

---

## 4.5 Die Elektronikbox

Die IP65-Box kommt an die **Nordseite** des Kastens oder auf den Deckel — nie in die Sonne.
Ein Akku über 45 °C altert schnell.

**Variante A** braucht deutlich mehr Platz als B, weil der 12-Ah-Akku hineinmuss:

```
   Variante A (Pi), Box ca. 200 x 150 x 75 mm:

   ┌────────────────────────────────────────────────┐
   │  [M16] ← Panelkabel      [M12] ← Notladung     │
   │                                                │
   │  ┌──────────────┐   ┌───────────────────────┐  │
   │  │ Laderegler   │   │  🔋 LiFePO4 12 V/12 Ah │  │
   │  │  + Sicherung │   │     auf Klettband     │  │
   │  └──────────────┘   └───────────────────────┘  │
   │                                                │
   │  ┌────────┐ ┌────────┐ ┌────────┐ ┌─────────┐  │
   │  │ DC-DC  │ │ INA219 │ │ Pi     │ │ SSD     │  │
   │  │ 12→5 V │ │        │ │ Zero 2 │ │         │  │
   │  └────────┘ └────────┘ └───┬────┘ └─────────┘  │
   │                            ▲                   │
   │      USB-Buchsen erreichbar lassen!            │
   │  [Silikagel]                                   │
   │  [M12] ← CSI + IR + Lichtschranke              │
   └────────────────────────────────────────────────┘
```

**Sechs Regeln für die Box:**

1. **USB-Buchsen müssen erreichbar bleiben.** Dort hängt die SSD, und dort kommt im Notfall
   die Powerbank an. Pi so einbauen, dass man ohne Ausbau ein Kabel anstecken kann.
2. **Akku auf Klettband**, nicht festgeklebt und nicht mit Kabelbindern gequetscht. Er ist
   das Teil, das man am ehesten austauscht.
3. **Sicherung gut zugänglich** — sie ist ein Verschleißteil.
4. **Silikagel-Beutel** in eine Ecke legen. Nimmt Restfeuchte auf.
5. **Kabelverschraubungen nach unten**, nie nach oben. Wasser läuft nach unten ab.
6. **Tropfschlaufe:** Jedes Kabel vor dem Eintritt einen Bogen nach unten machen lassen.

```
   richtig:              falsch:
   Box                   Box
    │                     │
    └──┐              ────┘
       │  ← Wasser        ← Wasser läuft
       ▼    tropft ab       in die Box
```

### Die Notlade-Buchse

Der Retter für Regenwochen: eine **Micro-USB-Verlängerung** von der Pi-PWR-Buchse nach
außen durch eine M12-Verschraubung, mit Gummikappe. Bei Dauerregen einfach eine Powerbank
anstecken — der Pi läuft weiter, ohne dass die Box geöffnet werden muss.

> **Achtung bei Variante A:** Die Powerbank versorgt dann den Pi, lädt aber **nicht** den
> Akku. Das ist eine Überbrückung, kein Ladegerät. Für echtes Nachladen den Akku
> ausbauen — deshalb das Klettband.

### 4.5b Das Mikrofon hören lassen (nur Variante B)

Bei Variante B nimmt die Kamera Ton auf ([8.2d](08-variante-esp32.md#82d-ton--bild-und-ton-zusammen)).
Das Mikrofon sitzt aber **fest auf dem XIAO-Board** — und das liegt in der
geschlossenen IP65-Box an der Außenwand. Ohne einen Weg für den Schall hört man
gedämpften Garten und kaum den Kasten.

**Die Lösung nutzt aus, dass die Box direkt an der Kastenwand sitzt:**

```
   Nistkasten              Elektronikbox
   ┌────────────┐         ┌──────────────┐
   │            │  6 mm   │              │
   │   innen    │ ──○───→ │  🎤 XIAO      │
   │            │  Loch   │              │
   └────────────┘   ↑     └──────────────┘
                Schaumstoff-Dichtung
                dazwischen (2-3 mm)
```

1. Ein **6-mm-Loch durch die Kastenwand** bohren, dort wo die Box sitzt — am
   besten in der oberen Hälfte, hinter dem Nest, nicht darüber.
2. Ein **gleich großes Loch in die Boxwand**, die an der Kastenwand liegt.
3. Zwischen Box und Kasten einen **Streifen Moosgummi** mit passendem Loch —
   dichtet gegen Regen, ohne den Schall zu blockieren.
4. Boxseitig ein Stückchen **atmungsaktive Membran** (Gore-Tex-Rest, Vlies aus
   einem Belüftungsstopfen) über das Loch kleben. Lässt Schall durch, hält
   Spritzwasser und Insekten draußen.

Weil beide Löcher **einander zugewandt** und vom Wetter abgeschirmt sind, bleibt
die IP65-Wirkung nach außen erhalten. Das Loch geht nicht ins Freie, sondern von
Box zu Kasten.

> **Kein Loch gebohrt?** Der Ton funktioniert trotzdem — er ist dann leiser und
> dumpfer, weil er über die Kabelverschraubungen kommt. Man kann das später
> nachrüsten, allerdings nur zwischen September und Februar
> ([1.8](01-machbarkeit.md#18-rechtliches-und-tierschutz)). Wer Ton will, bohrt
> also **beim Bauen**.

---

## 4.6 Solarpanel montieren

**Der wichtigste Satz des Kapitels: Das Panel gehört nicht an den Nistkasten.**

Nistkästen hängen gern halbschattig unter Bäumen — genau falsch für Solar. Das Panel wird
per Kabel an die sonnigste erreichbare Stelle gesetzt: Schuppendach, Zaunpfosten,
Garagenwand, Balkongeländer.

| Kriterium | Sollwert | Warum |
|---|---|---|
| Ausrichtung | **Süden** | maximale Tagesausbeute |
| Neigung | **30–40°** | Kompromiss Frühling/Sommer; Regen wäscht es sauber |
| Verschattung | **keine**, ganztägig | Ein Ast, der um 11 Uhr die Ecke abschattet, kostet mehr Ertrag als man denkt |
| Höhe | ≥ 1,5 m | weniger Laub, weniger Schnee, weniger Neugier |
| Kabelquerschnitt | **≥ 0,75 mm²** (A) · **≥ 1,5 mm² und kurz** (B) | bei 12 V und 2,5 A sind Verluste spürbar — bei **6 V** sind sie kritisch, siehe unten |

**Ein 30-W-Panel ist etwa 35 × 45 cm groß und wiegt ~2 kg** — es braucht eine richtige
Befestigung, keine Kabelbinder. Bei Variante B (10 W) ist es deutlich kleiner und
unkritisch.

> ⚠️ **Bei Variante B zählt jedes Zehntelvolt auf dem Panelkabel.** Der
> Laderegler arbeitet erst ab **4,5 V**, und das 5-V-Panel hat nur 0,5 V Luft
> nach unten. Zum Glück fließt höchstens **1 A** (mehr nimmt der Regler nicht
> an) — damit bleibt das Kabel unkritisch, solange man nicht übertreibt:
>
> | Verlängerung (Länge einfach) | 0,5 mm² | 0,75 mm² |
> |---|---|---|
> | 3 m | 0,14 V | 0,09 V |
> | 5 m | 0,23 V | 0,15 V |
> | 10 m | **0,46 V — zu viel** | 0,31 V |
>
> Die Kamerapanels aus [9.3b](09-bestellliste.md#93b-️-welches-solarpanel-passt-zum-laderegler)
> bringen **3 m Kabel** mit. Das reicht meistens vom Kasten zur sonnigen Ecke.
> Wird verlängert: **0,75 mm², höchstens 5 m dazu** — und danach am Dashboard
> prüfen, ob der Akku-Pfeil bei Sonne auf „steigt" geht.

> 🧒 **Kinderaufgabe mit echtem Ergebnis:** An einem sonnigen Wintertag stündlich notieren,
> wo im Garten Sonne ist und wo Schatten. Daraus wird die Panelposition **begründet** statt
> geraten. Das ist eine echte Messreihe, und das Ergebnis hält Jahre.
>
> Und nach der Montage: Website öffnen, Hand über das Panel halten, Ladestrom beobachten.
> Der Zusammenhang „Schatten = kein Strom" wird dann nie wieder vergessen.

---

## 4.7 Endmontage-Reihenfolge

| # | Schritt | Prüfen bevor weiter |
|---|---|---|
| 1 | Deckel bohren, Acryl einkleben, trocknen lassen | Kleber wirklich hart? |
| 2 | Kamera + LEDs im Deckel montieren | Sitzt alles fest? |
| 3 | Lichtschranke einsetzen, **justieren**, dann kleben | Löst ein Finger sicher aus? |
| 4 | Kabel durch **K**, Zugentlastung, Schlaufe für den Deckel | Kein Knick im CSI? |
| 5 | Box am Kasten befestigen, Verschraubungen setzen | Alle nach unten? |
| 6 | Alles verbinden — Reihenfolge: Akku, Panel, Last | Polarität doppelt geprüft? Sicherung drin? |
| 7 | Am Netzteil testen: Bild? LEDs? Lichtschranke? Website? | **Jetzt** ist der letzte einfache Moment für Korrekturen |
| 8 | Akku anstecken, Box schließen | Silikagel drin? |
| 9 | Panel montieren, Kabel verlegen | Tropfschlaufen? |
| 10 | Kasten aufhängen | Einflugloch nach **Ost/Südost**, nicht nach Westen (Wetterseite) |
| 11 | **Zwei Wochen Probebetrieb**, täglich Akkustand prüfen | Steigt der Akku tagsüber? |

Schritt 11 ist nicht optional. Er ist der Unterschied zwischen „hat funktioniert" und „hat
im April funktioniert".

---

## 4.8 Blickfeld richtig einstellen

Ein Modul mit 60–70° Bildwinkel deckt bei 22 cm Abstand rund 25 × 19 cm ab — also den
Kastenboden. Trotzdem lohnt sich Feinjustierung:

1. Kasten aufrecht hinstellen, Deckel drauf.
2. Ein Kuscheltier oder einen zerknüllten Papierball auf den Boden legen — als
   „Vogel-Platzhalter".
3. Livestream auf dem Handy öffnen.
4. Kamera minimal drehen, bis der Boden mittig und vollständig im Bild ist.
5. **Erst dann** den zweiten Heißkleberpunkt setzen.

**Bild zu dunkel?** `IR_HELLIGKEIT` erhöhen. **Bildmitte weiß überstrahlt?** Die LEDs
leuchten in die Linse — Abstand vergrößern oder eine kleine Pappblende um das Objektiv
kleben. **Bild auf dem Kopf?** `BILD_DREHEN = True`.

---

## 4.9 Was man am Kasten selbst *nicht* verändert

Der Nistkasten war schon fertig, und das soll so bleiben:

- **Einflugloch nicht vergrößern.** Der Durchmesser bestimmt, welche Art einzieht, und hält
  Nesträuber draußen.
- **Keine Sitzstange** vor dem Loch — die ist eine Einladung für Katzen und Elstern.
- **Keine zusätzlichen Lüftungslöcher.** Der Kasten ist wie er ist ausgelegt.
- **Nicht innen streichen oder lackieren.** Ausgasende Lösemittel in einem Brutraum sind ein
  echtes Problem.
- **Das Reinigungskonzept erhalten.** Der Deckel muss im Herbst aufgehen, um das alte Nest
  zu entfernen. Deshalb die Kabelschlaufe.

→ Weiter mit [5. Software-Tutorial](05-software-tutorial.md)
