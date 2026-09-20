// ============================================================================
//  SCHRITT 7 — Die Lichtschranke
// ============================================================================
//  Ziel: Den Infrarot-Strahl im Einflugloch justieren und sehen, wie aus
//        zwei einfachen Ereignissen eine echte Messung wird.
//
//  Vorher verkabeln (siehe Schaltplan 3.7):
//     Lichtschranke VCC  -> 3V3
//     Lichtschranke GND  -> GND
//     Lichtschranke OUT  -> D2 (GPIO3)
//
//  Was du machen sollst:
//     1. Sender und Empfänger so ausrichten, dass "frei" STABIL bleibt.
//     2. Mit dem Finger durchfahren -> muss sofort umschalten.
//     3. Zweimal durchfahren -> du siehst EINFLUG und AUSFLUG mit Dauer.
//
//  Wenn es genau umgekehrt ist (frei zeigt UNTERBROCHEN), dann unten
//  INVERTIERT auf true stellen.
//
//  SCHRITT 7 — Die Lichtschranke (Optimiert gegen Sensor-Flackern)
// ============================================================================

#define PIN_LICHTSCHRANKE   3       // = Pin D2
#define INVERTIERT          false
#define MIN_UNTERBRECHUNG   60     // Mindestdauer in ms für einen echten Durchflug
#define MAX_UNTERBRECHUNG   2000    // Länger = blockiert (z.B. Blatt)
#define DEBOUNCE_TIME       30      // Ignoriere Signalwechsel, die schneller als X ms sind

volatile uint32_t unterbrochenSeit = 0;
volatile uint32_t letzteDauer      = 0;
volatile bool     neuesEreignis    = false;
volatile uint32_t letzterWechsel   = 0; // Für die Entprellung

uint32_t vogelDrinSeit = 0;
uint32_t durchfluege   = 0;
uint32_t ignoriert     = 0;

void IRAM_ATTR ereignis() {
  uint32_t jetzt = millis();
  
  // Software-Entprellung: Ignoriere den Wechsel, wenn der letzte zu kurz her ist
  if (jetzt - letzterWechsel < DEBOUNCE_TIME) {
    return; 
  }
  letzterWechsel = jetzt;

  bool gebrochen = (digitalRead(PIN_LICHTSCHRANKE) == LOW);
  if (INVERTIERT) gebrochen = !gebrochen;

  if (gebrochen) {
    if (unterbrochenSeit == 0) unterbrochenSeit = jetzt;
  } else if (unterbrochenSeit != 0) {
    letzteDauer = jetzt - unterbrochenSeit;
    unterbrochenSeit = 0;
    neuesEreignis = true;
  }
}

bool strahlFrei() {
  bool gebrochen = (digitalRead(PIN_LICHTSCHRANKE) == LOW);
  if (INVERTIERT) gebrochen = !gebrochen;
  return !gebrochen;
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n=== Schritt 7: Lichtschranke (Entprellt) ===\n");

  pinMode(PIN_LICHTSCHRANKE, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_LICHTSCHRANKE), ereignis, CHANGE);

  Serial.printf("Strahl ist gerade: %s\n\n", strahlFrei() ? "FREI" : "GEBROCHEN");
  Serial.println("Fahre mit dem Finger durch den Strahl. Zweimal!\n");
}

uint32_t letzteAnzeige = 0;
bool     alterZustand  = true;

void loop() {
  // --- Justierhilfe ---
  bool frei = strahlFrei();
  if (frei != alterZustand) {
    alterZustand = frei;
    Serial.printf("   Strahl: %s\n", frei ? "frei" : "UNTERBROCHEN");
  }

  // --- Ausgewertetes Ereignis ---
  if (neuesEreignis) {
    neuesEreignis = false;
    uint32_t dauer = letzteDauer;

    if (dauer < MIN_UNTERBRECHUNG) {
      ignoriert++;
      Serial.printf("Zu kurz (%u ms) — ignoriert. Sensor-Zittern abgefangen!\n", dauer);
    } else if (dauer > MAX_UNTERBRECHUNG) {
      ignoriert++;
      Serial.printf("Zu lang (%u ms) — ignoriert. Sitzt etwas im Loch?\n", dauer);
    } else {
      durchfluege++;
      if (vogelDrinSeit == 0) {
        vogelDrinSeit = millis();
        Serial.printf("\n>>> EINFLUG  (Unterbrechung %u ms)\n", dauer);
        Serial.println("    Jetzt ist ein Vogel im Kasten.");
      } else {
        uint32_t drin = (millis() - vogelDrinSeit) / 1000;
        vogelDrinSeit = 0;
        Serial.printf(">>> AUSFLUG  — war %u Sekunden drin\n\n", drin);
      }
    }
  }

  // --- Alle 5 Sekunden eine Zusammenfassung ---
  if (millis() - letzteAnzeige > 5000) {
    letzteAnzeige = millis();
    Serial.printf("[Bilanz] %u Durchfluege, %u ignoriert, Kasten: %s\n",
                  durchfluege, ignoriert,
                  vogelDrinSeit ? "BESETZT" : "leer");
  }

  delay(10);
}
