// ============================================================================
//  SCHRITT 4 — Unsichtbares Licht
// ============================================================================
//  Ziel: Die IR-LEDs schalten und dimmen. Und beweisen, dass sie leuchten,
//        obwohl man nichts sieht.
//
//  Und noch etwas: Dieser Sketch sagt dir, ob dein MOSFET-Modul an unseren
//  5 Volt ueberhaupt sauber durchschaltet. Lies dazu die Auswertung unten.
//
//  ---------------------------------------------------------------------
//  VORHER VERKABELN — zaehl zuerst die Anschluesse deines Moduls!
//  (ausfuehrlich in Schaltplan 3.6)
//
//  VARIANTE A — kleines Modul "HW-532", 23 x 17 mm
//               Steuerseite 2 Anschluesse, Lastseite 3 Schraubklemmen
//
//     D0 (GPIO1) -> MOSFET PWM
//     GND        -> MOSFET GND          (Steuerseite)
//     5V         -> MOSFET +            (Lastseite, Schraubklemme)
//     GND        -> MOSFET -            (Lastseite, Schraubklemme)
//     LED Plus   -> MOSFET +            (ja, dieselbe Klemme wie 5V!)
//     LED Minus  -> MOSFET LOAD
//
//     Dieses Modul hat KEIN "OUT+". Es schaltet nur die Minus-Leitung.
//     In die +-Klemme kommen zwei Adern: die vom XIAO und die der LEDs.
//     Beide vorher verdrillen, dann haelt die Schraubklemme sauber.
//
//  VARIANTE B — grosses Modul "XY-MOS", ca. 50 x 25 mm
//               Steuerseite 3 Stifte, Lastseite 4 Schraubklemmen
//
//     D0 (GPIO1) -> MOSFET SIG
//     GND        -> MOSFET GND          (Steuerseite)
//     3V3        -> MOSFET VCC          (nur falls vorhanden)
//     5V         -> MOSFET VIN+
//     GND        -> MOSFET VIN-
//     LED Plus   -> MOSFET OUT+
//     LED Minus  -> MOSFET OUT-
//
//  Alle vier LEDs parallel: alle Plus zusammen, alle Minus zusammen.
//
//  ⚠️ NACKTE LEDs STATT MODULE? DANN ERST VIER VORWIDERSTAENDE EINBAUEN.
//     Die Module E9 haben den Widerstand auf der Platine. Eine nackte LED
//     direkt an 5 V zieht statt 20 mA schnell ein Ampere: Das MOSFET-Modul
//     wird heiss und riecht verschmort - und die LEDs leuchten dabei munter
//     weiter, du merkst es also nicht am Licht. 180 Ohm je LED, ein eigener
//     Widerstand pro Zweig. Rechnung in Schaltplan 3.6.
//  ---------------------------------------------------------------------
//
//  🔍 DAS EXPERIMENT:
//     Halte die FRONTKAMERA deines Handys auf die LEDs (nicht die Rückkamera —
//     die hat oft einen IR-Filter). Auf dem Handydisplay siehst du sie
//     schwach violett leuchten. Mit den Augen: nichts. Unsichtbares Licht!
// ============================================================================

#define PIN_IR_LED   1      // = Pin D0 auf dem Board

// 1000 Hz. Hoeher geht nur mit Variante B — der Optokoppler auf Variante A
// kaeme bei 20 kHz nicht mehr hinterher. Siehe Schaltplan 3.6.
#define PWM_FREQUENZ 1000

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n=== Schritt 4: IR-Licht ===\n");

  // PWM einrichten: 1000 Schaltvorgaenge pro Sekunde, Werte von 0 bis 255
  ledcAttach(PIN_IR_LED, PWM_FREQUENZ, 8);   // 8 Bit Aufloesung: 0-255
  ledcWrite(PIN_IR_LED, 0);

  Serial.println("Halte jetzt die FRONTKAMERA deines Handys auf die LEDs.");
  Serial.println("Mit den Augen siehst du nichts - auf dem Display schon!\n");
  delay(3000);
}

void loop() {
  // ---- 0. Der Modul-Test: einmal ohne Dimmen voll an ---------------------
  //
  // Hier wird NICHT gedimmt, sondern hart durchgeschaltet. Das ist der
  // einfachste Fall fuer das MOSFET-Modul. Geht es hier nicht, liegt es
  // nicht am Dimmen.
  Serial.println("--- TEST 1: hart voll an (kein Dimmen), 5 Sekunden ---");
  Serial.println("    Schau jetzt mit der Handy-Frontkamera drauf.");
  ledcWrite(PIN_IR_LED, 255);
  delay(5000);
  ledcWrite(PIN_IR_LED, 0);
  Serial.println("    ...aus.\n");
  delay(1500);

  // ---- 1. Langsam heller werden ------------------------------------------
  Serial.println("--- TEST 2: langsam heller werden (Dimmen) ---");
  for (int h = 0; h <= 255; h += 5) {
    ledcWrite(PIN_IR_LED, h);
    if (h % 25 == 0) Serial.printf("   Helligkeit %3d von 255\n", h);
    delay(80);
  }

  // ---- 2. Voll an -------------------------------------------------------
  Serial.println("VOLL AN (255) - so hell wird es nie im Betrieb!");
  delay(2000);

  // ---- 3. Wieder dunkler ------------------------------------------------
  Serial.println("Werde wieder dunkler...");
  for (int h = 255; h >= 0; h -= 5) {
    ledcWrite(PIN_IR_LED, h);
    delay(80);
  }

  // ---- 4. Der Wert, den wir wirklich benutzen ---------------------------
  Serial.println("\nSo hell ist es im echten Betrieb: 75 von 255 (etwa 30 %).");
  Serial.println("Warum nicht mehr? Damit es im Nistkasten nicht warm wird");
  Serial.println("und der Akku laenger haelt.\n");
  ledcWrite(PIN_IR_LED, 75);
  delay(5000);

  // ---- 5. Blinken -------------------------------------------------------
  Serial.println("Und jetzt Blinken - zum Spass.");
  for (int i = 0; i < 6; i++) {
    ledcWrite(PIN_IR_LED, 200); delay(150);
    ledcWrite(PIN_IR_LED, 0);   delay(150);
  }

  // ---- 6. Auswertung ----------------------------------------------------
  Serial.println("\n----------------------------------------------------");
  Serial.println("AUSWERTUNG - was hast du auf dem Handydisplay gesehen?");
  Serial.println("");
  Serial.println("  TEST 1 hell, TEST 2 wird sichtbar heller/dunkler");
  Serial.println("     -> alles gut, weiter mit Schritt 5.");
  Serial.println("");
  Serial.println("  TEST 1 hell, aber TEST 2 dimmt kaum/ruckelig");
  Serial.println("     -> PWM_FREQUENZ ist zu hoch fuer dein Modul.");
  Serial.println("        Auf 500 stellen und nochmal.");
  Serial.println("");
  Serial.println("  Modul wird WARM oder riecht verschmort");
  Serial.println("     -> SOFORT Strom weg. Hast du nackte LEDs ohne");
  Serial.println("        Vorwiderstand angeschlossen? Das ist fast immer");
  Serial.println("        die Ursache. Siehe Wartung 7.3.");
  Serial.println("");
  Serial.println("  TEST 1 nur schwaches Glimmen oder gar nichts");
  Serial.println("     -> Der Reihe nach pruefen:");
  Serial.println("        1. Steckt das gelbe Kabel auf D0 und auf PWM/SIG?");
  Serial.println("        2. Steckt das schwarze Kabel der STEUERseite auf GND?");
  Serial.println("        3. LEDs verpolt? Plus und Minus tauschen.");
  Serial.println("        4. Variante A: Haengt der LED-Plus mit an der");
  Serial.println("           +-Klemme? Dieses Modul hat kein OUT+.");
  Serial.println("        5. Steht D4184/AOD4184 auf dem Chip - kein IRF520?");
  Serial.println("        Bleibt es dabei: Variante A schafft die 5 V nicht.");
  Serial.println("        Nimm das grosse Modul (Variante B), Schaltplan 3.6.");
  Serial.println("----------------------------------------------------\n");

  Serial.println("Durchlauf fertig, fange von vorne an.\n");
  delay(2000);
}
