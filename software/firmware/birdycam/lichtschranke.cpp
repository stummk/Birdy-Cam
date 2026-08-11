#include "lichtschranke.h"
#include "config.h"
#include "speicher.h"

// ---------------------------------------------------------------------------
//  Diese Variablen werden vom Interrupt verändert. Deshalb "volatile":
//  Das sagt dem Übersetzer "verlass dich nicht darauf, dass sich das nicht
//  hinter deinem Rücken ändert". Ohne volatile würde er den Wert in ein
//  Register legen und die Änderung nie bemerken.
// ---------------------------------------------------------------------------
static volatile uint32_t istUnterbrochenSeit = 0;   // millis(), 0 = frei
static volatile uint32_t letzteDauerMs       = 0;   // fertig gemessen
static volatile bool     neuesEreignis       = false;

static bool     aktiv          = false;
static uint32_t vogelDrinSeit  = 0;                 // millis(), 0 = keiner drin
static uint32_t durchfluege    = 0;
static uint32_t ignoriert      = 0;

// ---------------------------------------------------------------------------
//  Der Interrupt. Er muss SEHR kurz sein — deshalb rechnet er nur die
//  Dauer aus und setzt eine Flagge. Alles andere macht die Hauptschleife.
//  IRAM_ATTR heißt: "leg diesen Code in den schnellen Speicher."
// ---------------------------------------------------------------------------
static void IRAM_ATTR lichtschrankeISR() {
  bool gebrochen = (digitalRead(PIN_LICHTSCHRANKE) == LOW);
  if (LICHTSCHRANKE_INVERTIERT) gebrochen = !gebrochen;

  uint32_t jetzt = millis();

  if (gebrochen) {
    if (istUnterbrochenSeit == 0) istUnterbrochenSeit = jetzt;
  } else {
    if (istUnterbrochenSeit != 0) {
      letzteDauerMs = jetzt - istUnterbrochenSeit;
      istUnterbrochenSeit = 0;
      neuesEreignis = true;
    }
  }
}

// ---------------------------------------------------------------------------
bool lichtschrankeStart() {
  if (!LICHTSCHRANKE_AN) {
    Serial.println("[Licht] Lichtschranke ist in config.h abgeschaltet.");
    return false;
  }

  pinMode(PIN_LICHTSCHRANKE, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_LICHTSCHRANKE),
                  lichtschrankeISR, CHANGE);
  aktiv = true;

  bool frei = strahlIstFrei();
  Serial.printf("[Licht] Lichtschranke bereit an GPIO%d (Strahl ist %s)\n",
                PIN_LICHTSCHRANKE, frei ? "frei" : "GEBROCHEN");
  if (!frei) {
    Serial.println("[Licht] Der Strahl ist beim Start schon gebrochen!");
    Serial.println("[Licht] Sender/Empfaenger justieren, oder");
    Serial.println("[Licht] LICHTSCHRANKE_INVERTIERT umstellen.");
  }
  return true;
}

// ---------------------------------------------------------------------------
bool strahlIstFrei() {
  bool gebrochen = (digitalRead(PIN_LICHTSCHRANKE) == LOW);
  if (LICHTSCHRANKE_INVERTIERT) gebrochen = !gebrochen;
  return !gebrochen;
}

bool vogelIstDrin() { return vogelDrinSeit != 0; }

uint32_t vogelDrinSeitSekunden() {
  if (vogelDrinSeit == 0) return 0;
  return (millis() - vogelDrinSeit) / 1000;
}

uint32_t lichtschrankeDurchfluege() { return durchfluege; }
uint32_t lichtschrankeIgnoriert()   { return ignoriert; }

// ---------------------------------------------------------------------------
bool lichtschrankePruefen() {
  if (!aktiv || !neuesEreignis) return false;

  neuesEreignis = false;
  uint32_t dauer = letzteDauerMs;

  // --- Filter: war das überhaupt ein Vogel? ---
  if (dauer < MIN_UNTERBRECHUNG_MS) {
    ignoriert++;
    return false;                       // Insekt, Zittern
  }
  if (dauer > MAX_UNTERBRECHUNG_MS) {
    ignoriert++;
    Serial.printf("[Licht] Zu lang (%u ms) — sitzt etwas im Loch?\n", dauer);
    return false;
  }

  durchfluege++;

  // --- Ein- oder Ausflug? ---
  if (vogelDrinSeit == 0) {
    vogelDrinSeit = millis();
    besuchZaehlen();
    Serial.printf("[Licht] EINFLUG (%u ms Unterbrechung)\n", dauer);
  } else {
    uint32_t aufenthalt = (millis() - vogelDrinSeit) / 1000;
    vogelDrinSeit = 0;
    aufenthaltZaehlen(aufenthalt);      // zählt auch die Tagessumme mit
    Serial.printf("[Licht] AUSFLUG — war %u s drin\n", aufenthalt);
  }

  return true;                          // Kamera: bitte aufnehmen
}
