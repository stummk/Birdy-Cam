// ============================================================================
//  SCHRITT 5 — Wie voll ist der Akku?
// ============================================================================
//  Ziel: Die Akkuspannung messen — und den Messwert richtig einstellen
//        ("kalibrieren"). Das ist echte Messtechnik.
//
//  Vorher verkabeln (siehe Schaltplan 3.3):
//     Akku +  -> Spannungssensor VCC (Schraubklemme)
//     Akku -  -> Spannungssensor GND (Schraubklemme)
//     Sensor S   -> D1 (GPIO2)
//     Sensor -   -> GND am Board
//
//  ---------------------------------------------------------------------------
//  KALIBRIEREN — so geht's:
//
//   1. Sketch hochladen, Seriellen Monitor öffnen.
//   2. Die Zeile "Am Pin: x.xxx V" ablesen.
//   3. Die ECHTE Akkuspannung messen:
//        - mit einem Multimeter direkt am Akku, ODER
//        - ein frisch geladener LiPo hat fast genau 4,20 V
//   4. Rechnen:   FAKTOR = echte Spannung / Spannung am Pin
//      Beispiel:  4,20 V / 0,84 V = 5,00
//   5. Diesen Faktor unten bei FAKTOR eintragen und neu hochladen.
//   6. Denselben Wert in config.h bei BATT_KALIBRIERUNG eintragen.
//
//  Warum ist überhaupt ein Faktor nötig? Der ESP32 verträgt nur 3,3 V.
//  Das Sensormodul teilt die Spannung deshalb durch etwa 5, damit sie
//  ungefährlich wird. Wir rechnen sie wieder hoch.
//  ---------------------------------------------------------------------------
// ============================================================================

#define PIN_AKKU   2        // = Pin D1 auf dem Board
#define FAKTOR     5.00     // <<< HIER DEIN MESSWERT AUS SCHRITT 4 >>>

float messePin() {
  // 16 Messungen und Mittelwert — einzelne Messungen zappeln immer etwas
  uint32_t summe = 0;
  for (int i = 0; i < 16; i++) {
    summe += analogReadMilliVolts(PIN_AKKU);
    delayMicroseconds(300);
  }
  return (summe / 16.0f) / 1000.0f;
}

// Aus der Spannung den Ladezustand schätzen. Ein LiPo entlädt sich nicht
// gleichmäßig — darum ein paar Stützpunkte statt einer geraden Linie.
int prozent(float v) {
  struct { float v; int p; } kurve[] = {
    {3.30f,0},{3.50f,10},{3.65f,25},{3.75f,45},
    {3.85f,60},{3.95f,75},{4.05f,88},{4.20f,100}
  };
  if (v <= kurve[0].v) return 0;
  if (v >= kurve[7].v) return 100;
  for (int i = 0; i < 7; i++)
    if (v < kurve[i+1].v) {
      float a = (v - kurve[i].v) / (kurve[i+1].v - kurve[i].v);
      return kurve[i].p + (int)(a * (kurve[i+1].p - kurve[i].p));
    }
  return 100;
}

void balken(int p) {
  Serial.print("  [");
  for (int i = 0; i < 20; i++) Serial.print(i < p / 5 ? '#' : '.');
  Serial.printf("] %d %%\n", p);
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  analogReadResolution(12);
  analogSetPinAttenuation(PIN_AKKU, ADC_11db);

  Serial.println("\n=== Schritt 5: Akku messen ===\n");
  Serial.printf("Benutzter Faktor: %.2f\n", FAKTOR);
  Serial.println("(zum Kalibrieren die Anleitung oben im Sketch lesen)\n");
}

void loop() {
  float amPin = messePin();
  float akku  = amPin * FAKTOR;
  int   p     = prozent(akku);

  Serial.printf("Am Pin: %.3f V   ->   Akku: %.2f V\n", amPin, akku);
  balken(p);

  if (amPin < 0.05f) {
    Serial.println("  ! Fast 0 V. Ist der Sensor angeschlossen? Akku dran?");
  } else if (akku > 4.35f) {
    Serial.println("  ! Ueber 4,35 V - der Faktor ist zu gross eingestellt.");
  } else if (akku < 3.30f) {
    Serial.println("  ! ACHTUNG: Akku sehr leer. Aufladen!");
  } else if (akku < 3.60f) {
    Serial.println("  ~ Wird knapp, sollte bald geladen werden.");
  } else {
    Serial.println("  Alles gut.");
  }
  Serial.println();

  delay(3000);
}
