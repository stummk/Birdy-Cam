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
static volatile uint32_t letzterWechsel      = 0;   // für die Entprellung

static bool     aktiv          = false;
static uint32_t vogelDrinSeit  = 0;                 // millis(), 0 = keiner drin
static uint32_t durchfluege    = 0;
static uint32_t ignoriert      = 0;
static uint32_t verpasst       = 0;                 // verlorene Ausflüge

// ---------------------------------------------------------------------------
//  Der Interrupt. Er muss SEHR kurz sein — deshalb rechnet er nur die
//  Dauer aus und setzt eine Flagge. Alles andere macht die Hauptschleife.
//  IRAM_ATTR heißt: "leg diesen Code in den schnellen Speicher."
// ---------------------------------------------------------------------------
static void IRAM_ATTR lichtschrankeISR() {
  uint32_t jetzt = millis();

  // Entprellen. Die einfachen Module schalten am Umschaltpunkt gern mehrfach
  // hintereinander — jeder dieser Zappler löst den Interrupt erneut aus. Wir
  // nehmen deshalb nur den ERSTEN Wechsel und sind danach kurz taub.
  // Ohne das wird aus einem Durchflug ein Dutzend Ereignisse.
  if (jetzt - letzterWechsel < LICHTSCHRANKE_ENTPRELL_MS) return;
  letzterWechsel = jetzt;

  bool gebrochen = (digitalRead(PIN_LICHTSCHRANKE) == LOW);
  if (LICHTSCHRANKE_INVERTIERT) gebrochen = !gebrochen;

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
  // Ohne eingeschaltete Lichtschranke hat der Pin nie ein pinMode() gesehen.
  // Ihn trotzdem zu lesen liefert einen zufälligen Pegel — also gar nicht erst
  // fragen und "frei" melden: Ohne Strahl unterbricht auch nichts.
  if (!aktiv) return true;

  bool gebrochen = (digitalRead(PIN_LICHTSCHRANKE) == LOW);
  if (LICHTSCHRANKE_INVERTIERT) gebrochen = !gebrochen;
  return !gebrochen;
}

bool vogelIstDrin() { return vogelDrinSeit != 0; }

uint32_t vogelDrinSeitSekunden() {
  if (vogelDrinSeit == 0) return 0;
  return (millis() - vogelDrinSeit) / 1000;
}

bool lichtschrankeAktiv() { return aktiv; }

uint32_t lichtschrankeDurchfluege() { return durchfluege; }
uint32_t lichtschrankeIgnoriert()   { return ignoriert; }

// ---------------------------------------------------------------------------
bool lichtschrankePruefen() {
  if (!aktiv) return false;

  // Hängengebliebene Unterbrechung aufräumen. Prellt das Modul heftig, kann
  // die abschließende Flanke ausgerechnet in die Entprellzeit fallen und
  // verlorengehen — dann stünde istUnterbrochenSeit für immer, und der
  // nächste echte Durchflug käme als absurd lange Unterbrechung an.
  // Ist der Strahl längst wieder frei, verwerfen wir die Messung.
  uint32_t seit = istUnterbrochenSeit;
  if (seit != 0 && millis() - seit > MAX_UNTERBRECHUNG_MS && strahlIstFrei()) {
    istUnterbrochenSeit = 0;
    ignoriert++;
  }

  // Notbremse für "Vogel drin". Ohne sie würde ein einziger verlorener
  // Ausflug die Zählung dauerhaft vertauschen: Jeder folgende Einflug käme
  // als Ausflug an, jeder Ausflug als Einflug.
  //
  // Die Aufenthaltsdauer wird dabei absichtlich NICHT mitgezählt — wir
  // wissen ja gerade nicht, wann der Vogel weg war. Eine erfundene Zahl
  // wäre schlimmer als eine fehlende. Der Besuch selbst bleibt gezählt,
  // der hat schließlich stattgefunden.
  if (VOGEL_MAX_DRIN_MINUTEN > 0 && vogelDrinSeit != 0 &&
      millis() - vogelDrinSeit > (uint32_t)VOGEL_MAX_DRIN_MINUTEN * 60000UL) {
    vogelDrinSeit = 0;
    verpasst++;
    Serial.printf("[Licht] Seit %u Minuten \"drin\" ohne Ausflug — den habe "
                  "ich verpasst.\n", (unsigned)VOGEL_MAX_DRIN_MINUTEN);
    Serial.printf("[Licht] Zaehlung zurueckgesetzt (%u. Mal). Haeuft sich "
                  "das, ist die\n", verpasst);
    Serial.println("[Licht] Lichtschranke dejustiert — Sketch 7 wiederholen.");
  }

  if (!neuesEreignis) return false;

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
