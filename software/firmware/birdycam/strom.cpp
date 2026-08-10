#include "strom.h"
#include "config.h"
#include "speicher.h"
#include <esp_sleep.h>

#define PWM_FREQUENZ   1000    // 1000 mal pro Sekunde ein/aus
#define PWM_BITS       8       // Auflösung: 0-255

static uint8_t irStand = 0;

void stromStart() {
  // PWM auf dem IR-Pin einrichten (Arduino-ESP32-Kern 3.x)
  ledcAttach(PIN_IR_LED, PWM_FREQUENZ, PWM_BITS);
  ledcWrite(PIN_IR_LED, 0);

  if (AKKU_MESSEN) {
    // 12 Bit Auflösung, Messbereich bis ~3,1 V
    analogReadResolution(12);
    analogSetPinAttenuation(PIN_AKKU, ADC_11db);
  }
  Serial.println("[Strom] Bereit.");
}

// ---------------------------------------------------------------------------
//  Akkuspannung messen
// ---------------------------------------------------------------------------
//  analogReadMilliVolts() ist besser als analogRead(), weil der ESP32 dabei
//  seine eigene, im Werk eingemessene Korrekturtabelle benutzt.
//  Wir messen 8 mal und nehmen den Mittelwert — einzelne Messungen zappeln.
// ---------------------------------------------------------------------------
float akkuVolt() {
  if (!AKKU_MESSEN) return 0.0f;

  uint32_t summe = 0;
  for (int i = 0; i < 8; i++) {
    summe += analogReadMilliVolts(PIN_AKKU);
    delayMicroseconds(200);
  }
  float amPin = (summe / 8.0f) / 1000.0f;     // Volt am Pin
  return amPin * BATT_KALIBRIERUNG;          // hochgerechnet auf Akkuspannung
}

// Grobe Umrechnung Spannung -> Prozent für einen LiPo mit einer Zelle.
// Die Kurve ist nicht linear, deshalb ein paar Stützpunkte.
uint8_t akkuProzent() {
  float v = akkuVolt();
  if (v <= 0.1f) return 0;
  struct { float v; uint8_t p; } kurve[] = {
    {3.30f,   0}, {3.50f,  10}, {3.65f,  25}, {3.75f,  45},
    {3.85f,  60}, {3.95f,  75}, {4.05f,  88}, {4.20f, 100}
  };
  if (v <= kurve[0].v) return 0;
  if (v >= kurve[7].v) return 100;
  for (int i = 0; i < 7; i++) {
    if (v < kurve[i + 1].v) {
      float anteil = (v - kurve[i].v) / (kurve[i + 1].v - kurve[i].v);
      return kurve[i].p + (uint8_t)(anteil * (kurve[i + 1].p - kurve[i].p));
    }
  }
  return 100;
}

// ---------------------------------------------------------------------------
//  Akku-Trend
// ---------------------------------------------------------------------------
//  Ein Amperemeter haben wir hier nicht (anders als bei der Pi-Variante mit
//  INA219). Ob die Sonne lädt, erkennen wir stattdessen am Verlauf: Wir
//  merken uns die Spannung von vor 10 Minuten und vergleichen.
//
//  10 mV Unterschied ist die Schwelle — darunter ist es Messrauschen.
// ---------------------------------------------------------------------------
static float    trendAlterWert = 0.0f;
static uint32_t trendAlterZeit = 0;
static int      trendRichtung  = 0;

void akkuTrendPflegen() {
  if (!AKKU_MESSEN) return;

  float jetzt = akkuVolt();
  if (jetzt < 0.5f) return;

  if (trendAlterWert == 0.0f) {          // allererste Messung
    trendAlterWert = jetzt;
    trendAlterZeit = millis();
    return;
  }

  if (millis() - trendAlterZeit < 600000UL) return;   // noch keine 10 Minuten

  float differenz = jetzt - trendAlterWert;
  if (differenz > 0.010f)       trendRichtung = 1;    // steigt -> lädt
  else if (differenz < -0.010f) trendRichtung = -1;   // fällt  -> entlädt
  else                          trendRichtung = 0;

  trendAlterWert = jetzt;
  trendAlterZeit = millis();
}

int akkuTrend() { return trendRichtung; }

// ---------------------------------------------------------------------------
//  IR-Licht
// ---------------------------------------------------------------------------
void irLicht(uint8_t helligkeit) {
  irStand = helligkeit;
  ledcWrite(PIN_IR_LED, helligkeit);
}

void irBlitzAn() {
  if (irStand == 0) {
    irLicht(IR_HELLIGKEIT);
    // Kurz warten, damit die Kamera-Automatik sich auf das neue Licht
    // einstellen kann. Ohne das ist das erste Bild fast schwarz.
    delay(IR_BLITZ_VORLAUF_MS);
  }
}

void irBlitzAus() { irLicht(0); }
bool irIstAn()    { return irStand > 0; }

// ---------------------------------------------------------------------------
//  Akkuschutz
// ---------------------------------------------------------------------------
//  Ein LiPo, der unter ~3,3 V entladen wird, verliert dauerhaft Kapazität.
//  Deshalb: lieber die Kamera abschalten als den Akku ruinieren.
//  Im Tiefschlaf braucht das Board fast nichts, und das Solarpanel kann
//  den Akku in Ruhe wieder aufladen.
// ---------------------------------------------------------------------------
void akkuSchutzPruefen() {
  if (!AKKU_MESSEN) return;

  float v = akkuVolt();
  if (v > 0.5f && v < AKKU_NOTAUS_VOLT) {
    Serial.printf("[Strom] NOTAUS! Akku nur %.2f V. Schlafe %d Minuten.\n",
                  v, NOTAUS_SCHLAF_MINUTEN);
    statistikSpeichern();          // Zahlen retten, bevor es dunkel wird
    irLicht(0);

    esp_sleep_enable_timer_wakeup((uint64_t)NOTAUS_SCHLAF_MINUTEN * 60ULL * 1000000ULL);
    esp_deep_sleep_start();        // ab hier geht es erst nach dem Aufwachen weiter,
                                   // und zwar wieder ganz von vorne bei setup()
  }
}
