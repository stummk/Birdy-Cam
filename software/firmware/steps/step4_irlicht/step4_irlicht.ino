// ============================================================================
//  SCHRITT 4 — Unsichtbares Licht
// ============================================================================
//  Ziel: Die IR-LEDs schalten und dimmen. Und beweisen, dass sie leuchten,
//        obwohl man nichts sieht.
//
//  Vorher verkabeln (siehe Schaltplan 3.4):
//     D0 (GPIO1) -> MOSFET-Modul SIG
//     3V3        -> MOSFET-Modul VCC
//     GND        -> MOSFET-Modul GND
//     5V         -> MOSFET-Modul VIN+     (Lastseite, Schraubklemme)
//     GND        -> MOSFET-Modul VIN-
//     LEDs       -> MOSFET-Modul OUT
//
//  🔍 DAS EXPERIMENT:
//     Halte die FRONTKAMERA deines Handys auf die LEDs (nicht die Rückkamera —
//     die hat oft einen IR-Filter). Auf dem Handydisplay siehst du sie
//     schwach violett leuchten. Mit den Augen: nichts. Unsichtbares Licht!
// ============================================================================

#define PIN_IR_LED   1      // = Pin D0 auf dem Board

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n=== Schritt 4: IR-Licht ===\n");

  // PWM einrichten: 1000 Schaltvorgaenge pro Sekunde, Werte von 0 bis 255
  ledcAttach(PIN_IR_LED, 1000, 8);
  ledcWrite(PIN_IR_LED, 0);

  Serial.println("Halte jetzt die FRONTKAMERA deines Handys auf die LEDs.");
  Serial.println("Mit den Augen siehst du nichts - auf dem Display schon!\n");
  delay(3000);
}

void loop() {
  // ---- 1. Langsam heller werden ------------------------------------------
  Serial.println("Werde langsam heller...");
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

  Serial.println("\nDurchlauf fertig, fange von vorne an.\n");
  delay(2000);
}
