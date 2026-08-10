// ============================================================================
//  BirdyCam — Einstellungen
// ============================================================================
//  Das ist die einzige Datei, die du normalerweise anfasst.
//  Alles hier kann man ändern, ohne den Rest zu verstehen.
//
//  Nach jeder Änderung: Speichern -> Hochladen (Pfeil-Symbol in der Arduino IDE).
// ============================================================================

#pragma once

// ----------------------------------------------------------------------------
//  1. Netzwerk — zwei Betriebsarten
// ----------------------------------------------------------------------------
//  Die Kamera kann auf zwei Wegen erreichbar sein:
//
//   ROUTER   Sie verbindet sich mit deinem Heim-WLAN. Du rufst sie vom Sofa
//            aus auf. Sparsam, und die Uhrzeit kommt automatisch aus dem
//            Internet. Braucht aber WLAN-Empfang am Nistkasten.
//
//   EIGENES  Sie macht ein EIGENES WLAN auf. Du verbindest dein Handy direkt
//            mit der Kamera — ganz ohne Router, überall im Garten.
//            Kostet mehr Strom (siehe unten) und hat keine Internet-Uhrzeit.
//
//   AUTO     Erst Router probieren. Klappt das nicht, eigenes WLAN aufmachen.
//            Das ist die entspannteste Einstellung: Zu Hause sparsam am
//            Router, und wenn der Router mal weg ist, kommt man trotzdem dran.
//
#define NETZ_AUTO       0
#define NETZ_ROUTER     1
#define NETZ_EIGENES    2

#define NETZ_MODUS      NETZ_AUTO

// --- Für ROUTER und AUTO:  << HIER MUSST DU WAS EINTRAGEN >> ---
#define WLAN_NAME       "HierDeinWLANName"
#define WLAN_PASSWORT   "HierDeinPasswort"

// So lange wird auf den Router gewartet, bevor AUTO auf eigenes WLAN umschaltet.
#define ROUTER_TIMEOUT_S  20

// --- Für EIGENES und AUTO ---
// So heißt das WLAN, das die Kamera aufmacht.
#define AP_NAME         "BirdyCam"

// ⚠️ Mindestens 8 Zeichen! Ein leerer Text ("") macht ein offenes WLAN —
//    das würde ich nicht tun, dann kann jeder in der Nachbarschaft zusehen.
#define AP_PASSWORT     "vogelhaus"

// Funkkanal 1-13. 1, 6 oder 11 sind die üblichen, weil sie sich nicht
// überlappen. Wenn die Verbindung im Garten schlecht ist: anderen probieren.
#define AP_KANAL        6

// ⚡ STROM: Ein eigenes WLAN muss ständig Funkbaken senden und darf nicht
//    schlafen. Das kostet etwa 0,25 W mehr — bei 15 Wh/Tag Grundverbrauch
//    sind das +6 Wh/Tag, also rund +40 %.
//
//    -> Für Dauerbetrieb mit eigenem WLAN: 15-20-W-Panel statt 10 W.
//    -> Oder hier auf true stellen: nachts schaltet das eigene WLAN ab,
//       wenn niemand verbunden ist. Spart etwa die Hälfte davon.
//       Nachteil: Nachts kommt man nicht dran, ohne zu warten.
#define AP_NACHTS_AUS   false

// Name, unter dem die Kamera erreichbar ist: http://birdycam.local
// Im eigenen WLAN geht immer auch die feste Adresse http://192.168.4.1
#define GERAETE_NAME    "birdycam"

// Zeitzone für Deutschland (Sommer-/Winterzeit automatisch)
#define ZEITZONE        "CET-1CEST,M3.5.0,M10.5.0/3"
#define NTP_SERVER      "de.pool.ntp.org"

// Im eigenen WLAN gibt es kein Internet und damit keine Uhrzeit vom
// Zeitserver. Lösung: Der erste Besucher der Website schenkt der Kamera
// die Uhrzeit seines Handys. Ohne das wären alle Zeitstempel falsch und
// das Stunden-Diagramm sinnlos.
#define ZEIT_VOM_BROWSER  true

// ----------------------------------------------------------------------------
//  2. Bild
// ----------------------------------------------------------------------------
// Auflösung. WICHTIG: Muss zu deinem Kameramodul passen!
//
//   Wert              Pixel        Megapixel  ca. Bilder/s  Modul
//   FRAMESIZE_VGA      640x480       0,31        20+        beide
//   FRAMESIZE_SVGA     800x600       0,48        15         beide
//   FRAMESIZE_UXGA    1600x1200      1,92        8-12       OV2640 + OV5640
//   FRAMESIZE_FHD     1920x1080      2,07        ~6         nur OV5640
//
// ⚠️ Der wichtigste Satz dieser Datei:
//    UXGA hat mit 1,92 Megapixel praktisch GENAUSO VIELE PIXEL wie
//    Full HD (2,07) — nur im Format 4:3 statt 16:9. Dafuer laeuft es
//    doppelt so fluessig und funktioniert auch mit dem gut lieferbaren
//    OV2640-Nachtsichtmodul.
//
//    -> Hast du ein OV2640-Modul: FRAMESIZE_UXGA. Kein Verlust an Detail.
//    -> Hast du ein OV5640-Modul: FRAMESIZE_FHD, wenn du 16:9 willst,
//       sonst auch hier UXGA.
//    -> Willst du fluessiges Video: FRAMESIZE_SVGA (~15 Bilder/s).
//
//    Der Speicherbedarf pro Tag ist bei allen aehnlich, weil immer die
//    SD-Karte die Obergrenze setzt. Auflösung kauft Detail, nicht Platz.
#define BILD_GROESSE    FRAMESIZE_UXGA

// JPEG-Qualität: 10 = super Bild/große Datei ... 30 = grobes Bild/kleine Datei
//
// ⚠️ Bei hoher Auflösung ist das die WICHTIGSTE Einstellung, und zwar nicht
//    wegen der Bildschönheit, sondern wegen der Schreibgeschwindigkeit der
//    SD-Karte. Beispiel UXGA bei 10 Bildern/s:
//
//      Qualität 12  ->  ~150 KB je Bild  ->  1,50 MB/s   ZU VIEL
//      Qualität 16  ->  ~95 KB je Bild   ->  0,95 MB/s   knapp (79 %)
//      Qualität 18  ->  ~80 KB je Bild   ->  0,80 MB/s   gut (67 %)  <- Standard
//      Qualität 22  ->  ~60 KB je Bild   ->  0,60 MB/s   viel Luft
//
//    Die SD-Karte am XIAO hängt an SPI und schafft praktisch ~1,2 MB/s.
//
//    Woran merkst du, dass es zu viel ist? Nicht an einer Fehlermeldung,
//    sondern an der BILDRATE: Die Karte bremst dann das ganze Programm,
//    und im Clip landen weniger Bilder pro Sekunde. Die Website zeigt
//    beides an — "SD-Karte schreibt X MB/s" und die Bilder/s oben rechts.
//
//    Deine Kartengeschwindigkeit misst du vorab in Tutorial-Schritt 2.
#define BILD_QUALITAET  18

// Takt für die Kamera in MHz.
// Bei Streifen oder Rauschen im Bild: auf 10 stellen! Siehe Schaltplan 3.5.
#define XCLK_MHZ        20

// Bild spiegeln / auf den Kopf stellen (je nachdem, wie die Kamera eingebaut ist)
#define BILD_SPIEGELN   false
#define BILD_DREHEN     false

// ----------------------------------------------------------------------------
//  3. Bewegungserkennung
// ----------------------------------------------------------------------------
// Die Erkennung läuft auf einem winzigen 80x60-Bild — das spart viel Strom.

// Wie stark muss sich ein Punkt ändern, damit er als "hat sich bewegt" zählt?
// Kleiner = empfindlicher. 15-40 ist sinnvoll.
#define BEWEGUNG_SCHWELLE     22

// Wie viele Punkte müssen sich ändern? (von 4800 Punkten insgesamt)
// Größer = unempfindlicher gegen Sonnenflecken und wackelnde Blätter.
#define BEWEGUNG_MIN_PUNKTE   45

// Nach dem Ende einer Bewegung noch so viele Sekunden weiter aufnehmen.
#define NACHLAUF_SEKUNDEN     3

// Der VORLAUF (die Sekunden vor der Bewegung — der Anflug!) wird nicht in
// Sekunden eingestellt, sondern in Bildern: siehe VORLAUF_MAX_BILDER weiter
// unten. Grund: Der Speicherbedarf hängt an der Anzahl der Bilder, nicht an
// der Zeit, und der PSRAM ist begrenzt.

// Ein Clip ist höchstens so lang.
#define CLIP_MAX_SEKUNDEN     20

// --- Größe des Vorlauf-Gedächtnisses ---------------------------------------
// Der Vorlauf liegt im PSRAM: feste Plätze, in die je EIN JPEG passen muss.
// Ist ein Platz zu klein, wird das Bild verworfen und der Vorlauf fehlt —
// die Firmware warnt dann im Seriellen Monitor.
//
// Passende Werte je Auflösung (bei BILD_QUALITAET 16-20):
//
//   Auflösung          JPEG ca.   Plätze   Platzgröße   PSRAM    Vorlauf
//   VGA   640x480        20 KB      24        50000     1,2 MB    ~1,2 s
//   SVGA  800x600        35 KB      24        80000     1,9 MB    ~1,6 s
//   UXGA 1600x1200       95 KB      24       150000     3,6 MB    ~2,4 s  <- Standard
//   FHD  1920x1080      110 KB      16       200000     3,2 MB    ~2,7 s
//
// Vom PSRAM (8 MB) brauchen Kamera und Bewegungserkennung zusammen ~0,5 MB —
// es bleibt also genug Luft. Wie viel wirklich frei ist, zeigt die Website
// unter "Systemzustand".
#define VORLAUF_MAX_BILDER    24
#define VORLAUF_SLOT_BYTES    150000

// Kürzeste Pause zwischen zwei Clips (verhindert 100 Clips pro Minute).
#define CLIP_PAUSE_SEKUNDEN   4

// ----------------------------------------------------------------------------
//  4b. Stream ODER Aufnahme — nicht beides gleichzeitig
// ----------------------------------------------------------------------------
// Bei Full HD ist das keine Bequemlichkeit, sondern nötig:
// Ein Bild wandert entweder ins WLAN (Stream) oder auf die SD-Karte (Clip).
// Beides gleichzeitig überfordert die Schreibgeschwindigkeit der Karte, und
// dann fehlen Bilder im Clip.
//
//   true  = Wenn jemand zuschaut, wird nicht aufgenommen.   (empfohlen)
//           Ein laufender Clip wird sauber beendet.
//   false = Beides parallel versuchen. Nur bei kleiner Auflösung sinnvoll.
#define STREAM_HAT_VORRANG    true

// ----------------------------------------------------------------------------
//  4c. SD-Karten-Takt
// ----------------------------------------------------------------------------
// Höher = schneller schreiben = längere Clips ohne Bildverlust.
// Zu hoch = Karte meldet sich nicht mehr. Das Programm probiert der Reihe
// nach 40, 20 und 10 MHz und nimmt den ersten Takt, der funktioniert.
// Hier steht nur, wo es anfängt.
#define SD_TAKT_START_HZ      40000000

// ----------------------------------------------------------------------------
//  4. Nachtsicht
// ----------------------------------------------------------------------------
// Ab welcher Helligkeit gilt es als Nacht? (0 = stockdunkel, 255 = gleißend)
#define NACHT_SCHWELLE        45

// IR-LED-Helligkeit 0-255. 75 entspricht etwa 30 %.
// Höher = helleres Nachtbild, aber mehr Stromverbrauch und mehr Wärme.
//
// Nachtbild zu dunkel? Hier ist die erste Stellschraube — bis 255 ist Luft,
// also mehr als das Dreifache. 940-nm-LEDs sieht der Kamerasensor nur etwa
// halb so gut wie 850-nm-LEDs; dafür sind sie für Menschen unsichtbar.
// Für den VOGEL sind beide unsichtbar. Details in Machbarkeit 1.5.
//
// Reicht 255 nicht: mehr LEDs, oder auf 850 nm wechseln.
#define IR_HELLIGKEIT         75

// Nachts wird nur alle X Millisekunden ein Bild gemacht (spart viel Strom).
#define NACHT_PAUSE_MS        900

// Die IR-LEDs leuchten nur kurz um die Aufnahme herum ("Blitz").
// Das spart ~90 % der LED-Energie. Zeit in Millisekunden zum Vorglühen.
#define IR_BLITZ_VORLAUF_MS   120

// ----------------------------------------------------------------------------
//  5. Speicher (Round-Robin-Ringe)
// ----------------------------------------------------------------------------
// So viele Dateien werden im Kreis überschrieben. Die älteste fällt raus.
#define RING_CLIPS      200     // Videoclips  (.avi)
#define RING_FOTOS      1000    // Vogelfotos  (.jpg)
#define RING_AUDIO      100     // Tonaufnahmen (.wav)

// ----------------------------------------------------------------------------
//  5b. Lichtschranke im Einflugloch
// ----------------------------------------------------------------------------
// Zählt Ein- und Ausflüge exakt — viel genauer als Bildvergleich, weil
// Sonnenflecken und Blattschatten sie nicht interessieren.
// Siehe Machbarkeit 1.3.
#define LICHTSCHRANKE_AN      true

// Kürzere Unterbrechungen sind kein Vogel (Insekt, Zittern).
#define MIN_UNTERBRECHUNG_MS  30

// Längere auch nicht (Blatt im Loch, Verschmutzung).
#define MAX_UNTERBRECHUNG_MS  2000

// Manche Module liefern LOW bei freiem Strahl, andere HIGH.
// Wenn der Zähler ohne Vögel hochläuft: hier umstellen.
#define LICHTSCHRANKE_INVERTIERT  false

// ----------------------------------------------------------------------------
//  6. Ton / Vogelgesang  —  AUS (auf Wunsch gestrichen)
// ----------------------------------------------------------------------------
// Der Code funktioniert und liegt bereit. Er erkennt Gesang und speichert
// ihn als WAV-Datei, sagt aber NICHT, welcher Vogel singt — dafür bräuchte
// es BirdNET auf einem Raspberry Pi 4/5. Siehe Machbarkeit 1.7.
//
// Zum Ausprobieren einfach auf true stellen.
#define AUDIO_AN              false

// Ab welcher Lautstärke gilt es als Gesang? Größer = unempfindlicher.
// Nach dem Einbau draußen einmal nachjustieren (Wind!).
#define GESANG_SCHWELLE       1400

// So lange vor dem erkannten Gesang wird mitgespeichert.
#define AUDIO_VORLAUF_MS      2000

// Aufnahme endet, wenn es so lange wieder still ist.
#define AUDIO_STILLE_MS       1500

// Längste Tonaufnahme.
#define AUDIO_MAX_MS          15000

// ----------------------------------------------------------------------------
//  7. Strom & Akku
// ----------------------------------------------------------------------------
#define AKKU_MESSEN           true

// Kalibrierung des Spannungssensors.
// Rechnung: FAKTOR = echte_Spannung / angezeigte_Spannung
// Anleitung dazu in Tutorial Schritt 5.
#define BATT_KALIBRIERUNG     5.00

// Unter dieser Spannung legt sich die Kamera schlafen, um den Akku zu retten.
// LiPo unter 3,3 V dauerhaft = Akku kaputt.
#define AKKU_NOTAUS_VOLT      3.40

// Ab dieser Spannung wacht sie wieder auf.
#define AKKU_WIEDER_AN_VOLT   3.60

// So lange schläft sie im Notaus, dann schaut sie nochmal nach (Minuten).
#define NOTAUS_SCHLAF_MINUTEN 30

// ----------------------------------------------------------------------------
//  8. Pins  (siehe Schaltplan 3.2 — nur ändern, wenn du umgesteckt hast)
// ----------------------------------------------------------------------------
#define PIN_IR_LED      1       // D0 -> MOSFET-Modul SIG
#define PIN_AKKU        2       // D1 <- Spannungssensor S
#define PIN_LICHTSCHRANKE 3     // D2 <- Lichtschranke OUT
#define PIN_SD_CS       21      // SD-Karte (fest verbaut, nicht ändern)

// PDM-Mikrofon (fest auf dem Board verbaut, nicht ändern)
#define PIN_MIC_CLK     42
#define PIN_MIC_DATA    41

// ----------------------------------------------------------------------------
//  9. Optional: Dateien auf einen Rechner im Haus hochladen
// ----------------------------------------------------------------------------
// Aus, solange kein Rechner im Haus läuft. Siehe Website-Doku 6.5.
#define UPLOAD_AN       false
#define UPLOAD_HOST     "192.168.1.50"
#define UPLOAD_PORT     21
#define UPLOAD_USER     "birdy"
#define UPLOAD_PASS     "birdy"
