# 7. Wartung & Fehlersuche

---

## 7.1 Der Jahresrhythmus

Das Projekt hat einen festen Takt, den die Vögel vorgeben — nicht wir.

| Monat | Was zu tun ist |
|---|---|
| **September** | 🔧 **Wartungsfenster.** Kasten öffnen: altes Nest raus, Linse putzen (Spinnennetze!), Lichtschranke reinigen, Silikagel tauschen, Akku prüfen, Panel abwischen |
| Oktober | Software-Updates, Umbauten, Erweiterungen — **jetzt** ist die Zeit dafür |
| November–Februar | 🛠️ **Bastelzeit.** Neue Features, Website erweitern, Daten des Vorjahres auswerten |
| **Ende Februar** | ⚠️ **Letzter Termin.** Danach wird der Kasten nicht mehr geöffnet |
| März | 👀 Zuschauen. Erste Besichtigungen durch Interessenten |
| April–Juni | 🥚 Brut und Aufzucht. **Kasten bleibt zu.** Nur Website beobachten |
| Juli–August | Kasten wird leer. Warten bis September (Zweitbruten sind möglich) |

> **Der wichtigste Satz des ganzen Projekts:** Ab März wird der Kasten nicht geöffnet. Nicht
> für ein Kabel, nicht für eine Schraube, nicht „nur kurz". § 44 BNatSchG, und davor
> gesunder Menschenverstand. Wenn im April etwas kaputt ist, ist es bis September kaputt.
> Deshalb steckt so viel Aufwand im Testen *vor* dem Einbau.

---

## 7.2 Wöchentlicher Blick (30 Sekunden)

Website aufrufen und vier Dinge prüfen:

| Prüfpunkt | Gut | Nicht gut |
|---|---|---|
| **Akku** | steigt tagsüber, sinkt nachts | sinkt über Tage durchgehend |
| **Ladestrom mittags** | positiv, ≥ 0,3 A | 0 A → Panel prüfen |
| **Besuche heute** | passt zur Jahreszeit | 0, oder 500 (Fehlalarme) |
| **Laufzeit** | wächst | springt immer wieder auf 0 → Neustarts! |

Springt die Laufzeit immer wieder zurück, startet der Pi ständig neu. Meist ist die
5-V-Versorgung zu schwach — siehe [7.4](#74-der-häufigste-fehler-ständige-neustarts).

---

## 7.3 Fehlersuche: Symptom → Ursache

### Die Kamera ist gar nicht erreichbar

| Prüfen | Wie |
|---|---|
| Akku leer? | Nach mehreren Regentagen hat der Laderegler die Last abgeschaltet. Bei Sonne kommt sie von selbst zurück |
| Hängt sie? | Powerbank an die Notlade-Buchse. Kommt sie dann → Stromproblem |
| WLAN weg? | Router neu gestartet? Der Pi verbindet sich selbst wieder, ein Neustart hilft |
| `.local` geht nicht | IP direkt versuchen. Sie steht im Router unter „Netzwerk/Geräte" |

**Wenn wochenlang nichts geht:** Nicht den Kasten öffnen. Die Elektronikbox sitzt *außen* —
genau dafür. Box öffnen, Bildschirm/SSH anschließen, Protokoll lesen:

```bash
sudo journalctl -u birdycam -b --no-pager | tail -50
```

### Es entstehen keine Clips

| Ursache | Lösung |
|---|---|
| Lichtschranke verstellt oder verschmutzt | Justage prüfen ([Tutorial 7](05-software-tutorial.md#schritt-7--die-lichtschranke-justieren)) |
| Bilderkennung zu unempfindlich | `BEWEGUNG_SCHWELLE` senken |
| Datenordner voll oder nicht schreibbar | `df -h /srv/birdycam` |
| Kamera abgestürzt | `sudo systemctl restart birdycam` |
| Nachts: zu dunkel | `IR_HELLIGKEIT` erhöhen |

### Zu viele Clips (100 pro Stunde)

Der Klassiker im Frühling: **Sonnenflecken wandern durch den Kasten**, Blattschatten
flackern.

1. `BEWEGUNG_SCHWELLE` erhöhen: 8 → 15 → 25
2. `CLIP_PAUSE_S` erhöhen: 5 → 15
3. Notfalls **`BEWEGUNG_AN = False`** — dann löst nur noch die Lichtschranke aus. Man
   verliert Aufnahmen vom Füttern im Kasten, bekommt aber ausschließlich echte Anflüge.

**Die Besuchszahlen bleiben davon unberührt** — die kommen von der Lichtschranke und werden
von Sonnenflecken nicht beeinflusst. Das war der Grund für dieses Bauteil.

### Der Besuchszähler läuft ohne Vögel hoch

Die Lichtschranke flattert. Ursachen in dieser Reihenfolge:

1. **Sender/Empfänger schauen sich nicht genau an** → nachjustieren
2. **Sonne fällt schräg ins Loch und blendet den Empfänger** → Strohhalm-Blende
3. **Spinnennetz oder Blatt im Strahl** → putzen
4. `MIN_UNTERBRECHUNG_MS` erhöhen: 30 → 60

### Nachtbild ist weiß in der Mitte

Die IR-LEDs leuchten direkt ins Objektiv. Das ist ein **Bau**problem, kein Softwareproblem:

- LEDs weiter von der Linse weg (mindestens 3 cm)
- kleine Pappblende um das Objektiv kleben
- `IR_HELLIGKEIT` reduzieren

### Nachtbild hat waagerechte Streifen

Das PWM-Dimmen der LEDs und der Bildaufbau der Kamera geraten in Konflikt.

- `IR_HELLIGKEIT = 1.0` setzen — dann ist PWM ganz aus, die LEDs leuchten konstant
- oder in `strom.py` die PWM-Frequenz von 1000 auf 5000 Hz erhöhen

### Akku wird nicht voll

| Ursache | Prüfen |
|---|---|
| Panel verschmutzt | Abwischen — bringt oft 20 % |
| Verschattung dazugekommen | Ast gewachsen? Jahreszeit? |
| Panelkabel zu dünn/lang | Bei über 5 m mindestens 0,75 mm² |
| Frost | Unter 0 °C lädt auch LiFePO4 **nicht**. Kein Defekt, sondern Physik |
| Laderegler falsch eingestellt | Steht das Akkuprofil auf **LiFePO4**? Ein Blei-Profil lädt zu hoch |
| Laderegler defekt | Panelspannung im Leerlauf messen: sollte 17–22 V sein bei einem 12-V-Panel |

---

## 7.4 Der häufigste Fehler: ständige Neustarts

Wenn die Laufzeit auf der Website immer wieder auf 0 springt, bricht die 5-V-Versorgung
zusammen. Die Ursache ist fast immer **die SSD am Micro-USB-Port des Zero**.

Reihenfolge zum Abarbeiten:

1. **DC-DC-Wandler prüfen:** Kann er wirklich 3 A? Ein 1-A-Wandler reicht nicht.
2. **Kabel:** Dünne Micro-USB-Kabel haben hohen Widerstand. Ein kurzes, dickes nehmen.
3. **SSD-Verbrauch:** Eine NVMe-SSD im USB-Gehäuse zieht deutlich mehr als eine
   2,5-Zoll-SATA-SSD. Tauschen hilft.
4. **USB-Y-Kabel** (~6 €) mit separater Stromeinspeisung für die SSD — das ist die sichere
   Lösung.
5. **Notfall-Rückfallebene:** Von SD-Karte booten, SSD nur für Daten. Dann zieht die SSD nur
   beim Schreiben Strom, nicht dauernd.

Dieses Problem war in [1.10](01-machbarkeit.md#110-wo-es-realistisch-klemmen-wird) als
Hauptrisiko der Variante A benannt — es ist keine Überraschung, sondern der bekannte
Schwachpunkt.

---

## 7.5 Wenn die SSD stirbt

Erkennbar daran: Das System hängt, oder im Protokoll stehen Ein-/Ausgabefehler.

```bash
sudo dmesg | grep -i "I/O error"
sudo smartctl -a /dev/sda        # falls smartmontools installiert ist
```

1. Neue SSD am Computer mit dem Raspberry Pi Imager bespielen (wie
   [Tutorial 2b](05-software-tutorial.md#2b--betriebssystem-auf-die-ssd))
2. Umstecken, Software neu installieren (`sudo ./install.sh`)
3. Ringe legt das Programm automatisch neu an

**Kein Datenverlust an der Hardware** — deshalb ist die SSD Puffer und nicht Archiv. Was
wichtig war, ist längst heruntergeladen (siehe
[6.8](06-website-und-daten.md#68-und-wenn-doch-mal-ein-rechner-im-haus-läuft)).

> **Die SD-Karte aus Schritt 1 nicht wegwerfen!** Sie ist die schnellste Rückfallebene: Bei
> SSD-Ausfall Karte rein, Pi bootet, man kann wenigstens nachschauen, was los ist.

---

## 7.6 Ersatzteile, die man im Haus haben sollte

| Teil | Preis | Warum |
|---|---|---|
| CSI-Kabel für Zero | 5 € | Bricht am ehesten |
| NoIR-Kameramodul | 16 € | Zweites Ausfallteil |
| microSD 16 GB | 6 € | Rückfallebene zum Booten |
| Sicherungen | 2 € | Verschleißteil |
| USB-Y-Kabel | 6 € | Löst das Neustart-Problem |

Zusammen 35 € — und sie verhindern, dass mitten in der Brutzeit drei Wochen Stillstand
entstehen, weil eine Lieferung unterwegs ist.

---

## 7.7 Wenn das Projekt größer werden soll

Der ehrliche Ausblick: Dieser Aufbau ist am Ende seiner Möglichkeiten, wenn es um
**Artenerkennung** und **Gesangsbestimmung** geht. Das ist keine Frage besserer Software,
sondern von Rechenleistung und damit von Strom
([1.7](01-machbarkeit.md#17-was-bewusst-fehlt-und-warum)).

Der nächste Schritt wäre ein **Raspberry Pi 4/5 im Haus am Stromkabel**:

| Was er hinzufügt | Wie |
|---|---|
| Gesang mit Artnamen | [BirdNET-Go](https://github.com/tphakala/birdnet-go), 6000+ Arten, USB-Mikro am Fenster. Hört den Garten genauso gut wie am Kasten |
| Echte Artenerkennung | Zweite Kamera am **Futterhaus**: Farbe, Tageslicht, Seitenansicht → 20+ Arten |
| Dauerarchiv | Festplatte statt Ringspeicher |
| Statistik über Jahre | Die SQLite-Datei einfach dorthin kopieren und weiterführen |

Der Nistkasten-Node bleibt dabei **unverändert**. Er ist so gebaut, dass er auch dann noch
Sinn hat: Er ist das Gerät, das dort hängt, wo kein Kabel hinkommt.

---

## 7.8 Was das Kind selbst können sollte

Nach dem Projekt eine schöne Prüfliste — wenn all das sitzt, hat es wirklich etwas gelernt:

- [ ] Die Website aufrufen und erklären, was die zwei Balkendiagramme bedeuten
- [ ] Einen Clip abspielen und den **Vorlauf** darin zeigen
- [ ] In `einstellungen.py` die Empfindlichkeit ändern und den Dienst neu starten
- [ ] Am Ladestrom erkennen, ob die Sonne reicht
- [ ] Erklären, warum die IR-LEDs unsichtbar sind
- [ ] Erklären, warum die Lichtschranke genauer zählt als die Bilderkennung
- [ ] Erklären, warum der Kasten ab März zubleibt
- [ ] `sudo journalctl -u birdycam -f` aufrufen und die Meldungen vorlesen

Der letzte Punkt ist der wertvollste. Wer Logmeldungen lesen kann, findet Fehler selbst —
und braucht keinen Erwachsenen mehr.

---

## 7.9 Notfallkarte zum Ausdrucken

```
 ┌────────────────────────────────────────────────────────┐
 │  BIRDYCAM — SCHNELLHILFE                               │
 ├────────────────────────────────────────────────────────┤
 │  Website:   http://birdycam.local:8080/                │
 │  SSH:       ssh pi@birdycam.local                      │
 │                                                        │
 │  Protokoll ansehen                                     │
 │    sudo journalctl -u birdycam -f                      │
 │  Neu starten                                           │
 │    sudo systemctl restart birdycam                     │
 │  Einstellungen ändern                                  │
 │    sudo nano /opt/birdycam/birdycam/einstellungen.py   │
 │                                                        │
 │  Nicht erreichbar?                                     │
 │    1. Bei Sonne nochmal probieren (Akku-Abschaltung)   │
 │    2. Powerbank an die Notlade-Buchse                  │
 │    3. Box öffnen, SSH, Protokoll lesen                 │
 │                                                        │
 │  Laufzeit springt auf 0?   -> 5-V-Versorgung zu schwach │
 │  Zu viele Clips?           -> BEWEGUNG_SCHWELLE hoch    │
 │  Keine Clips?              -> Lichtschranke justieren   │
 │  Nachtbild dunkel?         -> IR_HELLIGKEIT hoch        │
 │  Nachtbild gestreift?      -> IR_HELLIGKEIT = 1.0       │
 │                                                        │
 │  ⚠️  KASTEN VON MÄRZ BIS AUGUST NICHT ÖFFNEN            │
 └────────────────────────────────────────────────────────┘
```

---

← Zurück zur [Übersicht](../README.md) · Alternative: [8. Variante ESP32](08-variante-esp32.md)
