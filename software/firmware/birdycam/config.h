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
//    schlafen. Das kostet etwa 0,25 W mehr — bei 16 Wh/Tag Grundverbrauch
//    (mit Ton) sind das +6 Wh/Tag, also rund +40 %.
//
//    -> Mit dem 15-W-Panel ist das an hellen Tagen kein Problem. Knapp wird
//       es im März und bei Dauerbewölkung — dann ist NETZ_ROUTER die
//       sparsamere Wahl. Rechnung dazu in Überblick 1.6.
//    -> Oder hier auf true stellen: nachts schaltet das eigene WLAN ab,
//       wenn niemand verbunden ist. Spart etwa die Hälfte davon.
//       Nachteil: Nachts kommt man nicht dran, ohne zu warten.
#define AP_NACHTS_AUS   false

// --- Funkeinstellungen (gelten fuer beide Betriebsarten) --------------------
//
// ⚠️ ZUERST DIE ANTENNE PRÜFEN. Der XIAO bringt seine WLAN-Antenne nicht fest
//    verbaut mit: Sie liegt als kleines Plättchen mit Kabel in der Packung und
//    muss auf den winzigen u.FL-Stecker oben links auf der Platine gedrückt
//    werden. Ohne sie ist die Reichweite erbärmlich — das ist mit Abstand die
//    häufigste Ursache für "WLAN zu schwach". Keine Software-Einstellung kann
//    das ausgleichen. Siehe Bauplan 4.5 (Regel 7) und Software 5.2.

// Funk-Land. Wichtig: Im Auslieferungszustand kennt der ESP32 nur die Kanäle
// 1-11. Viele Router in Deutschland funken aber auf 12 oder 13 — dann ist das
// WLAN für die Kamera schlicht unsichtbar, obwohl das Handy daneben vier
// Balken zeigt. Mit "DE" sind die Kanäle 1-13 erlaubt.
#define WLAN_LAND       "DE"

// Sendeleistung in dBm. 19.5 ist das Maximum des Chips und die richtige Wahl
// für einen Nistkasten im Garten. Kleinere Werte sparen ein bisschen Strom,
// kosten aber sofort Reichweite — nur verkleinern, wenn der Router direkt
// neben dem Kasten steht.
#define WLAN_SENDELEISTUNG  WIFI_POWER_19_5dBm

// Stromsparmodus des Funkmoduls ("Modem-Sleep"): Das WLAN döst zwischen zwei
// Funkkontakten. Das spart rund 0,1 W, macht die Kamera aber träge und bei
// schwachem Empfang unzuverlässig — Livestream und Website ruckeln oder
// brechen ab.
//
//   false = immer wach. Stabil. Empfohlen, solange der Akku mitspielt.
//   true  = sparsam. Nur sinnvoll bei starkem Empfang und knappem Strom.
#define WLAN_STROMSPAREN    false

// --- Bluetooth ------------------------------------------------------------
// Bluetooth bleibt aus. Hier gibt es bewusst KEINEN Schalter: Der Nistkasten
// spricht ausschließlich über WLAN. birdycam.ino schaltet den BLE-Teil beim
// Start ausdrücklich ab und gibt seinen Speicher frei (rund 60 KB, die der
// Kamera zugutekommen). Danach lässt er sich bis zum Neustart nicht mehr
// einschalten — genau so ist es gewollt.

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
// Bei Streifen oder Rauschen im Bild: auf 10 stellen! Siehe Schaltplan 3.9.
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

// Wie oft im Nachtbetrieb kurz nachgesehen wird, ob es draußen hell geworden
// ist ("Lichtprobe"). Dafür geht das IR-Licht für ein Bild aus.
//
// Das muss sein: Eine Tag/Nacht-Entscheidung anhand eines Bildes, das vom
// EIGENEN IR-Licht beleuchtet ist, dreht sich im Kreis — Licht an, Bild hell,
// "ist ja Tag", Licht aus, Bild schwarz, "ist ja Nacht", Licht an ...
//
// 60 Sekunden heißt: Die Dämmerung wird höchstens eine Minute zu spät
// bemerkt, und die Probe kostet praktisch nichts.
#define NACHT_PROBE_SEKUNDEN  60

// IR-LED-Helligkeit 0-255. 110 entspricht etwa 43 %.
//
// ⚠️ Mehr ist hier NICHT besser, und das ist der am häufigsten falsch
//    verstandene Wert dieser Datei. Diese LED-Module sind dafür gemacht, eine
//    Einfahrt auszuleuchten — mehrere Meter. Du leuchtest vom Deckel auf das
//    Nest, also rund 20 cm. Licht nimmt im Quadrat des Abstands ab: Auf 20 cm
//    statt 2 m kommt das HUNDERTFACHE an.
//
//    Zu viel Licht macht das Bild schlechter, nicht besser: Die Automatik
//    sieht den überstrahlten Fleck in der Mitte und blendet das ganze Bild
//    ab — die Ecken, in denen der Vogel sitzt, werden dunkler statt heller.
//
// So stellst du es richtig ein — mit einer Zahl statt nach Gefühl:
//
//    Die Website zeigt unter "Technische Details" die BILDHELLIGKEIT (0-255).
//    Diesen Wert hier so lange erhöhen, bis sie nachts bei etwa 90-120 liegt.
//    Dann aufhören. Ab da kostet jede Stufe nur noch Akku.
//
// Das eigentliche Ziel ist nämlich nicht "hell", sondern "Verstärkung
// runter": Das Rauschen im Nachtbild kommt aus der Sensorverstärkung. Genug
// IR-Licht heißt, dass die Automatik sie zurücknehmen kann — und ab diesem
// Punkt bringt mehr Licht gar nichts mehr.
//
// 940-nm-LEDs sieht der Kamerasensor nur etwa halb so gut wie 850-nm-LEDs;
// dafür sind sie für Menschen unsichtbar. Für den VOGEL sind beide
// unsichtbar. Details in Überblick 1.5.
#define IR_HELLIGKEIT         110

// Nachts wird nur alle X Millisekunden ein Bild gemacht (spart viel Strom).
#define NACHT_PAUSE_MS        900

// --- Dauerlicht oder Blitz? ------------------------------------------------
//
// Ab Werk leuchten die IR-LEDs nachts DURCHGEHEND. Das klingt verschwenderisch
// und ist trotzdem richtig — der Grund ist die Belichtungsautomatik:
//
//   Beim Blitzen ist es zwischen zwei Bildern stockdunkel. Die Automatik fährt
//   die Verstärkung ans Maximum, weil sie Schwarz sieht. Dann gehen die LEDs
//   für 120 ms an, und das Bild entsteht MIT dieser Maximalverstärkung — also
//   mit maximalem Rauschen. Die Automatik kommt nie zur Ruhe.
//
//   Bei Dauerlicht pendelt sie sich auf das tatsächliche IR-Licht ein. Das ist
//   der Unterschied zwischen brauchbar und unbrauchbar, nicht ein bisschen
//   heller.
//
// ⚡ STROM: Im Nistkasten ist es auch tagsüber dunkel — die Kamera ist also
//    praktisch immer im Nachtbetrieb, die LEDs liefen 24 h. Vier Module an
//    5 V ziehen voll aufgedreht 82 mA (0,41 W):
//
//      IR_HELLIGKEIT  75  ->  0,12 W  ->  3,0 Wh/Tag
//      IR_HELLIGKEIT 110  ->  0,18 W  ->  4,3 Wh/Tag   <- Standard
//      IR_HELLIGKEIT 255  ->  0,41 W  ->  9,8 Wh/Tag
//
//    Zum Vergleich: Der Grundverbrauch liegt bei 16 Wh/Tag (Router-Betrieb,
//    mit Ton). Die Rechnung mit Panel und Akku steht in Überblick 1.6.
#define IR_DAUERLICHT         true

// Damit das Dauerlicht keine Regenwoche killt, schaltet die Kamera darunter
// von allein auf Blitzbetrieb zurück — der spart ~90 % der LED-Energie.
// Steigt der Akku wieder, kommt das Dauerlicht zurück. Du musst nichts tun.
//
// 3,80 V ist gut die Hälfte. Der Notaus bei AKKU_NOTAUS_VOLT bleibt davon
// unberührt, das ist die letzte Sicherung. Ohne Spannungssensor
// (AKKU_MESSEN false) bleibt das Dauerlicht immer an.
#define IR_DAUERLICHT_AB_V    3.80f

// Zeit in Millisekunden zum Vorglühen, bevor im Blitzbetrieb ausgelöst wird.
#define IR_BLITZ_VORLAUF_MS   120

// So viele Bilder werden nach dem Einschalten des IR-Lichts WEGGEWORFEN.
//
// Das ist kein Schnickschnack, sondern eine Notwendigkeit: Die Kamera nimmt
// durchgehend auf und legt die fertigen Bilder in zwei Puffer. Ein frisch
// abgeholtes Bild ist deshalb ein paar Hundertstel ALT — belichtet, bevor die
// LEDs angingen. Ohne dieses Leerlaufen beleuchtet der Blitz nur Bilder, die
// nie jemand zu sehen bekommt.
#define IR_SETTLE_BILDER      2

// ----------------------------------------------------------------------------
//  4a. Nachtbild — was der Sensor nachts anders machen soll
// ----------------------------------------------------------------------------
//  Bis hierher ging es um das Licht. Jetzt geht es um die Kamera selbst: Sie
//  bekommt nachts ein eigenes Profil, weil IR-Licht etwas völlig anderes ist
//  als Tageslicht.

// Graustufen bei Nacht.
//
// Unter reinem Infrarot gibt es keine echten Farben mehr — der Weißabgleich
// versucht trotzdem, welche herzustellen, und produziert dabei zuverlässig
// einen Farbstich und grünes Farbrauschen. Graustufen schalten das komplett
// ab: kein Grünstich, kein Farbrauschen, und die JPEGs werden nebenbei
// kleiner. So sieht jede Nachtsichtkamera aus, und zwar aus genau diesem Grund.
//
// Auf false nur, wenn du sehen willst, was der Sensor ohne das macht.
#define NACHT_GRAUSTUFEN      true

// Obergrenze der Sensorverstärkung bei Nacht.
//
// ⚠️ Gegen die Intuition: KLEINER ist hier oft besser. Verstärkung IST das
//    Rauschen — sie macht aus wenig Licht ein helles, aber griesiges Bild.
//    Lieber eine Stufe mehr IR-Licht (IR_HELLIGKEIT) und eine Stufe weniger
//    Verstärkung.
//
// Erlaubt: GAINCEILING_2X, _4X, _8X, _16X, _32X, _64X, _128X
#define NACHT_GAINCEILING     GAINCEILING_8X

// ----------------------------------------------------------------------------
//  4b. Sitzen die IR-LEDs hinter DEMSELBEN Fenster wie die Linse?
// ----------------------------------------------------------------------------
//  Bauplan 4.10 sagt für Variante B: Die LEDs gehören in den Nistraum, nicht
//  mit in die Box. Wer sie aus Platz- oder Wettergründen trotzdem drin lässt,
//  hat ein Problem — und dieser Schalter ist die Gegenmaßnahme.
//
//  Was passiert: Das Nutzlicht läuft 20 cm zum Nest, wird von dunklem Holz
//  schwach zurückgeworfen und läuft 20 cm zurück. Das Störlicht läuft 2 mm
//  zum Deckel, von dem ein paar Prozent direkt zurückkommen. Die paar Prozent
//  aus 2 mm sind heller als das Nutzlicht aus 40 cm. Die Automatik sieht also
//  vor allem den Schleier, blendet ab — und der Nistraum wird schwarz.
//
//  Was dieser Schalter tut: Er schaltet die Belichtungsautomatik nachts AB und
//  stellt feste Werte ein. Der Schleier ist konstant; wenn niemand mehr dagegen
//  regelt, kann er das Bild auch nicht mehr abdunkeln. Er liegt dann als
//  gleichmäßiger Grauschleier darüber. Das kostet Kontrast — aber man sieht
//  etwas, statt in Schwarz zu starren.
//
//  Das ist Schadensbegrenzung, keine Reparatur. Was wirklich hilft, steht in
//  Bauplan 4.10.8: Moosgummi-Kragen um jede LED, schwarze Trennwand, Blendring.
//
//  ℹ️ Steht hier auf true, weil in diesem Kasten die LEDs aus Platz- und
//     Wettergründen in der Box geblieben sind. Hast du sie nach Bauplan
//     4.10.2 draußen im Nistraum: auf false stellen, dann arbeitet die
//     Kamera nachts wieder mit Belichtungsautomatik.
#define IR_HINTER_DEM_FENSTER true

// Die festen Werte dafür. Gelten NUR, wenn IR_HINTER_DEM_FENSTER true ist.
//
// ⚠️ Eine Sache vorweg, die beim Einstellen viel Zeit spart: IR_HELLIGKEIT,
//    NACHT_BELICHTUNG und NACHT_VERSTAERKUNG multiplizieren sich. Für die
//    Bildhelligkeit ist es egal, an welchem der drei du drehst — nur der
//    PREIS ist unterschiedlich:
//
//      IR_HELLIGKEIT        kostet Akku
//      NACHT_BELICHTUNG     kostet Bewegungsunschärfe (Vogel im Anflug)
//      NACHT_VERSTAERKUNG   kostet Rauschen
//
//    Daraus folgt die Reihenfolge:
//
//      Bild zu DUNKEL?   1. NACHT_BELICHTUNG hoch (kostet nichts, solange
//                           niemand fliegt)  2. IR_HELLIGKEIT hoch
//                        3. ganz zuletzt NACHT_VERSTAERKUNG
//
//      Bild zu HELL / milchig-weiß?  Zuerst IR_HELLIGKEIT RUNTER. Das ist
//                        die einzige Stellschraube, die dabei auch noch Akku
//                        spart — und der Schleier vom Deckel wird kleiner.
//
//    Und der Grund, warum mehr IR-Licht den Schleier NICHT überstrahlt:
//    Schleier und Nutzlicht kommen beide von denselben LEDs. Drehst du auf,
//    wächst beides im selben Verhältnis. Was du siehst, entscheidet allein
//    die Geometrie — also Kragen, Trennwand und Blendring, nicht die
//    Helligkeit. Siehe Bauplan 4.10.8.

// Belichtungszeit 0-1200 (in Bildzeilen). Größer = länger belichtet = helleres
// Bild, aber mehr Bewegungsunschärfe beim anfliegenden Vogel.
#define NACHT_BELICHTUNG      600

// Verstärkung 0-30. So klein wie möglich — siehe NACHT_GAINCEILING.
#define NACHT_VERSTAERKUNG    8

// Wie oft pro Sekunde das Dimmsignal ein- und ausschaltet (Hertz).
//
// 1000 Hz, weil das kleine MOSFET-Modul "HW-532" (23 x 17 mm, 2 + 3
// Anschluesse) einen Optokoppler PC817 im Signalweg hat. Der braucht einige
// Dutzend Mikrosekunden zum Schalten und kaeme bei 20 kHz nicht hinterher —
// aus dem Dimmen wuerde Matsch. Bei 1000 Hz schafft er es locker.
//
// Hast du das grosse Modul ohne Optokoppler ("XY-MOS", 3 + 4 Anschluesse)
// und hoerst nachts ein Pfeifen in der Tonaufnahme: 20000 eintragen. Das
// liegt ueber dem Hoerbaren und ueber dem, was das Mikrofon durchlaesst.
// Details in Schaltplan 3.6.
#define IR_PWM_FREQUENZ       1000

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
// Siehe Überblick 1.3.
//
// Ab Werk AUS: Die Lichtschranke ist optionales Zubehör, und ohne eingebautes
// Modul würde ein offener Eingang nur Phantom-Besuche zählen. Hast du sie
// eingebaut und nach Sketch 7 justiert, hier auf true stellen.
//
// Steht sie auf false, läuft alles weiter: Ausgelöst wird dann allein über die
// Bilderkennung, und gezählt wird pro Clip statt pro Durchflug (birdycam.ino).
#define LICHTSCHRANKE_AN      false

// Kürzere Unterbrechungen sind kein Vogel (Insekt, Zittern).
// 60 ms statt 30: Die einfachen Gabellichtschranken zittern am Umschaltpunkt
// mehr, als man denkt. Ein Vogel im Einflugloch braucht immer länger.
#define MIN_UNTERBRECHUNG_MS  60

// Längere auch nicht (Blatt im Loch, Verschmutzung).
#define MAX_UNTERBRECHUNG_MS  2000

// Entprellung: So lange nach einem Signalwechsel werden weitere Wechsel
// ignoriert. Billige Module "prellen" am Umschaltpunkt, das heißt sie
// schalten in wenigen Millisekunden mehrfach hin und her. Ohne diesen Filter
// zählt die Kamera aus einem Durchflug drei — oder gar keinen, weil jede
// Einzelunterbrechung unter MIN_UNTERBRECHUNG_MS bleibt.
// Größer = ruhiger, aber sehr schnelle Vögel gehen verloren. 30 ist erprobt.
#define LICHTSCHRANKE_ENTPRELL_MS  30

// Notbremse für "Vogel drin". Geht ausgerechnet die AUSFLUG-Unterbrechung
// verloren, bleibt die Kamera für immer der Meinung, es sitze jemand im
// Kasten — und schlimmer: Ab dann ist die Zählung vertauscht, weil jeder
// weitere Einflug als Ausflug gewertet wird. Nach dieser Zeit setzt die
// Kamera deshalb zurück und fängt wieder sauber mit "Einflug" an.
//
// ⚠️ Der Wert muss LÄNGER sein als der längste echte Aufenthalt, sonst
//    macht die Notbremse genau den Schaden, den sie verhindern soll.
//    Und ein brütendes Weibchen sitzt die ganze Nacht auf den Eiern.
//    Deshalb ab Werk 12 Stunden — das ist lang, aber sicher.
//
//    720 = 12 Stunden (Werkseinstellung, überlebt eine Brutnacht)
//    120 = 2 Stunden. Nur, wenn dich ausschließlich Tagesbesuche
//          interessieren und der Kasten nachts leer ist.
//      0 = aus. Dann bleibt ein verpasster Ausflug für immer stehen.
#define VOGEL_MAX_DRIN_MINUTEN  720

// Manche Module liefern LOW bei freiem Strahl, andere HIGH.
// Wenn der Zähler ohne Vögel hochläuft: hier umstellen.
#define LICHTSCHRANKE_INVERTIERT  false

// ----------------------------------------------------------------------------
//  5c. Tagesstatistik über Wochen  ->  /tage.csv
// ----------------------------------------------------------------------------
// Um Mitternacht schreibt die Kamera EINE Zeile über den vergangenen Tag in
// die Datei /tage.csv: Besuche, erster und letzter Anflug, Aufenthaltsdauer,
// Clips, Fotos, Tonaufnahmen, Akku-Minimum und -Maximum und die 24
// Stundenwerte. Damit gibt es einen Verlauf über Wochen und Monate — nicht
// nur "heute".
//
// ⚙️ Warum CSV und nicht JSON (die Frage lohnt sich):
//
//    Eine Zeile anhängen ist EIN Schreibvorgang. Kein Einlesen, kein Parser,
//    kein Neuschreiben der Datei. Bei JSON müsste die Kamera die ganze Liste
//    laden, ergänzen und wieder ausgeben — mit 30 oder 300 Tagen wird das
//    zum Problem, weil der Arbeitsspeicher knapp ist. Und ein Stromausfall
//    mitten im Neuschreiben würde die ganze Datei zerstören, nicht nur die
//    letzte Zeile.
//
//    Zweiter Grund: Alle Zeilen sind GLEICH LANG (156 Bytes). Die Kamera
//    kann deshalb direkt an die letzten 30 Zeilen springen, statt die Datei
//    von vorne zu lesen. Bei einem Jahr Verlauf ist das der Unterschied
//    zwischen "sofort" und "56 KB durchkauen".
//
//    Und drittens: Man kann die Datei in Excel oder LibreOffice öffnen.
//    Die Website liefert daraus JSON — aber das entsteht erst beim Abruf.
//
// Platzbedarf: 156 Bytes am Tag. Ein Jahr = 56 KB. Auf einer 32-GB-Karte
// also nichts.
#define TAGE_CSV_AN           true

// So viele Tage zeigt die Website. Die Datei behält alle Tage, unabhängig
// von diesem Wert. Mehr als 60 nimmt die Kamera nicht an (Arbeitsspeicher).
#define TAGE_ANZEIGEN         30

// ----------------------------------------------------------------------------
//  6. Ton — Mikrofon, Clips mit Ton, Ton im Livestream
// ----------------------------------------------------------------------------
// Das Mikrofon sitzt fest auf der XIAO Sense. Es läuft auf dem zweiten
// Prozessorkern mit, damit das Bild davon nichts merkt.
//
// AUDIO_AN ist der Hauptschalter. Steht er auf false, ist Ton komplett aus —
// egal, was in den drei Zeilen darunter steht.
#define AUDIO_AN              true

// Ton in die Videoclips schreiben. Die AVI-Datei bekommt dann zwei Spuren,
// Bild und Ton, und der VLC Player spielt sie zusammen ab.
//
// Was das kostet: 32 KB pro Sekunde, also ~4 % mehr Platz auf der Karte.
// Was es bringt: Man hört die Jungen betteln. Das ist der halbe Nistkasten.
#define TON_IN_CLIPS          true

// Ton im Livestream. Die Website holt ihn von Tür 82 als endlose WAV-Datei.
//
// ⚠️ Zwei Dinge, die man vorher wissen muss:
//    1. Browser dürfen Ton nicht von selbst anfangen. Auf der Website muss
//       man einmal auf "🔊 Ton an" tippen — danach bleibt er an.
//    2. Der Ton läuft dem Bild etwa eine Sekunde nachher, weil der Browser
//       ihn puffert. Für "was ist da los?" reicht das; lippensynchron ist
//       es nicht und wird es auf diesem Weg auch nicht.
#define TON_IM_STREAM         true

// Abtastrate in Messungen pro Sekunde. 16000 erfasst Töne bis 8 kHz und
// deckt damit Vogelstimmen ab. 8000 halbiert den Platzbedarf, klingt aber
// dumpf (dann fehlt das Feine an Meisengesang).
#define TON_ABTASTRATE        16000

// Lautstärke-Verstärkung 1-32 für Clip und Stream. Das PDM-Mikrofon auf dem
// XIAO liefert sehr leise Werte — ohne kräftige Verstärkung hört man auf der
// Aufnahme fast nichts. 16 ist in Sketch 6 erprobt und ein guter Startwert.
// Zu hoch = die Spitzen werden abgeschnitten und es knackt; dann 8 nehmen.
// Die Gesangserkennung unten arbeitet weiter mit den unverstärkten Werten —
// an GESANG_SCHWELLE ändert sich also nichts.
#define TON_VERSTAERKUNG      16

// So viele Millisekunden Ton VOR dem Auslöser landen mit im Clip.
// Sinnvoll ist etwa so viel, wie der Bild-Vorlauf lang ist (~2,4 s bei UXGA).
// Sind noch nicht so viele da (kurz nach dem Start), wird vorne mit Stille
// aufgefüllt — dann bleiben Bild und Ton gleich lang.
#define TON_VORLAUF_MS        2500

// --- Vogelgesang zusätzlich als eigene WAV-Datei ---------------------------
// Das ist unabhängig vom Ton in Clip und Stream: Wird es im
// Vogelgesang-Frequenzbereich laut, legt die Kamera eine WAV-Datei an —
// mit den 2 Sekunden davor, sonst fehlt der Anfang jedes Rufs.
//
// Sie sagt NICHT, welcher Vogel singt. Dafür bräuchte es BirdNET auf einem
// Raspberry Pi 4/5, siehe Überblick 1.7.
#define GESANG_AUFNEHMEN      false

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
//  8. Pins  (siehe Schaltplan 3.5 — nur ändern, wenn du umgesteckt hast)
// ----------------------------------------------------------------------------
#define PIN_IR_LED      1       // D0 -> MOSFET-Modul PWM (auf manchen Modulen "SIG")
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
