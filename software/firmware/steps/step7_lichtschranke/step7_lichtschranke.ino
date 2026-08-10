// ============================================================================
//  SCHRITT 7 — Die Lichtschranke
// ============================================================================
//  Ziel: Den Infrarot-Strahl im Einflugloch justieren und sehen, wie aus
//        zwei einfachen Ereignissen eine echte Messung wird.
//
//  Vorher verkabeln (siehe Variante-ESP32-Doku):
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
// ============================================================================

#define PIN_LICHTSCHRANKE   3       // = Pin D2
#define INVERTIERT          false
#define MIN_UNTERBRECHUNG   30      // kürzer = Insekt, wird ignoriert
#define MAX_UNTERBRECHUNG   2000    // länger  = Blatt im Loch

volatile uint32_t unterbrochenSeit = 0;
volatile uint32_t letzteDauer      = 0;
volatile bool     neuesEreignis    = false;

uint32_t vogelDrinSeit = 0;
uint32_t durchfluege   = 0;
uint32_t ignoriert     = 0;

// Der Interrupt: wird sofort aufgerufen, wenn sich der Pin ändert.
// Er muss ganz kurz sein — deshalb rechnet er nur und setzt eine Flagge.
void IRAM_ATTR ereignis() {
  bool gebrochen = (digitalRead(PIN_LICHTSCHRANKE) == LOW);
  if (INVERTIERT) gebrochen = !gebrochen;

  uint32_t jetzt = millis();
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
  Serial.println("\n=== Schritt 7: Lichtschranke ===\n");

  pinMode(PIN_LICHTSCHRANKE, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_LICHTSCHRANKE), ereignis, CHANGE);

  Serial.printf("Strahl ist gerade: %s\n\n", strahlFrei() ? "FREI" : "GEBROCHEN");
  if (!strahlFrei()) {
    Serial.println(">>> Der Strahl ist schon gebrochen!");
    Serial.println(">>> Justiere Sender und Empfaenger, oder stelle");
    Serial.println(">>> oben INVERTIERT auf true.\n");
  }
  Serial.println("Fahre mit dem Finger durch den Strahl. Zweimal!\n");
}

uint32_t letzteAnzeige = 0;
bool     alterZustand  = true;

void loop() {
  // --- Justierhilfe: jede Änderung sofort melden ---
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
      Serial.printf("Zu kurz (%u ms) — ignoriert. Insekt? Zittern?\n", dauer);
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
