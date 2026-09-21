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
//  Die drei Zahlen unten heissen in config.h genauso (mit dem Zusatz
//  LICHTSCHRANKE_ bzw. _MS). Was du hier erprobst, traegst du dort ein.
// ============================================================================

#define PIN_LICHTSCHRANKE   3       // = Pin D2
#define INVERTIERT          false
#define MIN_UNTERBRECHUNG   60      // kürzer = Insekt oder Zittern, wird ignoriert
#define MAX_UNTERBRECHUNG   2000    // länger  = Blatt im Loch
#define ENTPRELL_MS         30      // so lange nach einem Wechsel: taub sein
#define MAX_DRIN_MINUTEN    720     // Notbremse, falls ein Ausflug verlorengeht

volatile uint32_t unterbrochenSeit = 0;
volatile uint32_t letzteDauer      = 0;
volatile bool     neuesEreignis    = false;
volatile uint32_t letzterWechsel   = 0; // Für die Entprellung

uint32_t vogelDrinSeit = 0;
uint32_t durchfluege   = 0;
uint32_t ignoriert     = 0;

void IRAM_ATTR ereignis() {
  uint32_t jetzt = millis();

  // Entprellen: Billige Module schalten am Umschaltpunkt mehrfach hin und
  // her. Wir nehmen nur den ERSTEN Wechsel und sind danach kurz taub —
  // sonst wird aus einem Durchflug ein Dutzend Ereignisse.
  if (jetzt - letzterWechsel < ENTPRELL_MS) return;
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

  // --- Hängengebliebene Unterbrechung aufräumen ---
  //  Prellt das Modul heftig, kann die abschliessende Flanke ausgerechnet in
  //  die Entprellzeit fallen und verlorengehen. Dann stuende unterbrochenSeit
  //  fuer immer. Ist der Strahl laengst wieder frei: verwerfen.
  if (unterbrochenSeit != 0 && millis() - unterbrochenSeit > MAX_UNTERBRECHUNG
      && strahlFrei()) {
    unterbrochenSeit = 0;
    ignoriert++;
    Serial.println("Unterbrechung haengengeblieben — verworfen.");
  }

  // --- Notbremse: "Vogel drin" kann nicht ewig gelten ---
  //  Geht die AUSFLUG-Unterbrechung verloren, waere ab da alles vertauscht:
  //  jeder Einflug wuerde als Ausflug gezaehlt. Zum Ausprobieren am
  //  Schreibtisch darfst du MAX_DRIN_MINUTEN ruhig auf 1 stellen.
  if (MAX_DRIN_MINUTEN > 0 && vogelDrinSeit != 0 &&
      millis() - vogelDrinSeit > (uint32_t)MAX_DRIN_MINUTEN * 60000UL) {
    vogelDrinSeit = 0;
    Serial.println("Ausflug verpasst - Zaehlung zurueckgesetzt.");
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
