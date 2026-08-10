#include "audio.h"
#include "config.h"
#include <ESP_I2S.h>
#include <SD.h>

#define ABTASTRATE      16000            // 16000 Messungen pro Sekunde
#define BLOCK_PROBEN    512              // so viel holen wir auf einmal
#define VORLAUF_PROBEN  (ABTASTRATE * AUDIO_VORLAUF_MS / 1000)
#define MAX_PROBEN      (ABTASTRATE * AUDIO_MAX_MS   / 1000)

static I2SClass I2S;

// Das "Gedächtnis" für die letzten 2 Sekunden (ein Ringpuffer)
static int16_t*  vorlauf      = nullptr;
static uint32_t  vorlaufPos   = 0;
static bool      vorlaufVoll  = false;

// Die eigentliche Aufnahme
static int16_t*  aufnahme     = nullptr;
static uint32_t  aufnahmeLen  = 0;

// Zustände der Zustandsmaschine
enum AudioZustand { RUHE, AUFNEHMEN, FERTIG };
static volatile AudioZustand zustand = RUHE;
static volatile uint16_t     lautstaerke = 0;
static uint32_t  stilleSeit  = 0;

// ---------------------------------------------------------------------------
//  Lautstärke im Vogel-Frequenzbereich schätzen.
//  Trick: Wir rechnen die Differenz zwischen aufeinanderfolgenden Messwerten.
//  Tiefe Töne (Verkehr, Wind) ändern sich langsam -> kleine Differenzen.
//  Hohe Töne (Vogelgesang, 2-8 kHz) ändern sich schnell -> große Differenzen.
//  Das ist ein sehr einfacher Hochpassfilter, und er reicht hier völlig.
// ---------------------------------------------------------------------------
static uint16_t lautstaerkeMessen(const int16_t* proben, size_t anzahl) {
  if (anzahl < 2) return 0;
  uint64_t summe = 0;
  for (size_t i = 1; i < anzahl; i++) {
    int32_t d = (int32_t)proben[i] - (int32_t)proben[i - 1];
    summe += (uint64_t)((int64_t)d * d);
  }
  return (uint16_t)min<uint64_t>(65535, sqrt((double)(summe / (anzahl - 1))));
}

// ---------------------------------------------------------------------------
//  Die Hintergrund-Aufgabe: läuft für immer auf Kern 0
// ---------------------------------------------------------------------------
static void audioAufgabe(void* p) {
  int16_t block[BLOCK_PROBEN];

  for (;;) {
    size_t bytes = I2S.readBytes((char*)block, BLOCK_PROBEN * sizeof(int16_t));
    size_t proben = bytes / sizeof(int16_t);
    if (proben == 0) { vTaskDelay(pdMS_TO_TICKS(5)); continue; }

    uint16_t laut = lautstaerkeMessen(block, proben);
    lautstaerke = laut;

    if (zustand == RUHE) {
      // Ins Gedächtnis schreiben (überschreibt das Älteste)
      for (size_t i = 0; i < proben; i++) {
        vorlauf[vorlaufPos] = block[i];
        vorlaufPos = (vorlaufPos + 1) % VORLAUF_PROBEN;
        if (vorlaufPos == 0) vorlaufVoll = true;
      }

      // Laut geworden? Dann Aufnahme starten.
      if (laut > GESANG_SCHWELLE) {
        // Erst das Gedächtnis in die Aufnahme kopieren — in der richtigen
        // Reihenfolge: ab der aktuellen Position bis zum Ende, dann von vorn.
        aufnahmeLen = 0;
        if (vorlaufVoll) {
          uint32_t rest = VORLAUF_PROBEN - vorlaufPos;
          memcpy(aufnahme, vorlauf + vorlaufPos, rest * sizeof(int16_t));
          memcpy(aufnahme + rest, vorlauf, vorlaufPos * sizeof(int16_t));
          aufnahmeLen = VORLAUF_PROBEN;
        } else {
          memcpy(aufnahme, vorlauf, vorlaufPos * sizeof(int16_t));
          aufnahmeLen = vorlaufPos;
        }
        stilleSeit = millis();
        zustand = AUFNEHMEN;
        Serial.printf("[Audio] Gesang erkannt (Lautstaerke %u)\n", laut);
      }

    } else if (zustand == AUFNEHMEN) {
      // Weiter anhängen, solange Platz ist
      uint32_t frei = MAX_PROBEN - aufnahmeLen;
      uint32_t nimm = min<uint32_t>(frei, proben);
      memcpy(aufnahme + aufnahmeLen, block, nimm * sizeof(int16_t));
      aufnahmeLen += nimm;

      if (laut > GESANG_SCHWELLE) stilleSeit = millis();

      bool zuLang   = (aufnahmeLen >= MAX_PROBEN);
      bool ruhigGut = (millis() - stilleSeit > AUDIO_STILLE_MS);
      if (zuLang || ruhigGut) {
        zustand = FERTIG;
        Serial.printf("[Audio] Aufnahme fertig: %.1f s\n",
                      aufnahmeLen / (float)ABTASTRATE);
      }
    }
    // Zustand FERTIG: warten, bis die Hauptschleife die Datei geschrieben hat
  }
}

// ---------------------------------------------------------------------------
bool audioStart() {
  if (!AUDIO_AN) return false;

  vorlauf  = (int16_t*)ps_malloc(VORLAUF_PROBEN * sizeof(int16_t));
  aufnahme = (int16_t*)ps_malloc(MAX_PROBEN     * sizeof(int16_t));
  if (!vorlauf || !aufnahme) {
    Serial.println("[Audio] Zu wenig PSRAM fuer die Tonpuffer.");
    return false;
  }
  memset(vorlauf, 0, VORLAUF_PROBEN * sizeof(int16_t));

  I2S.setPinsPdmRx(PIN_MIC_CLK, PIN_MIC_DATA);
  if (!I2S.begin(I2S_MODE_PDM_RX, ABTASTRATE,
                 I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO)) {
    Serial.println("[Audio] Mikrofon startet nicht.");
    return false;
  }

  // Kern 0 macht den Ton, Kern 1 (die normale Hauptschleife) macht das Bild
  xTaskCreatePinnedToCore(audioAufgabe, "audio", 4096, nullptr, 1, nullptr, 0);

  Serial.println("[Audio] Mikrofon laeuft.");
  return true;
}

bool audioFertig()     { return zustand == FERTIG; }
bool audioNimmtAuf()   { return zustand == AUFNEHMEN; }
uint16_t audioLautstaerke() { return lautstaerke; }

// ---------------------------------------------------------------------------
//  Als WAV-Datei speichern.
//  Eine WAV-Datei ist ganz simpel: 44 Bytes Kopf, dann die Messwerte roh.
// ---------------------------------------------------------------------------
bool audioSpeichern(const String& pfad) {
  if (zustand != FERTIG) return false;

  uint32_t datenBytes = aufnahmeLen * sizeof(int16_t);
  uint8_t kopf[44];
  uint32_t byteRate = ABTASTRATE * 2;   // 1 Kanal * 2 Bytes

  memcpy(kopf + 0,  "RIFF", 4);
  *(uint32_t*)(kopf + 4)  = 36 + datenBytes;
  memcpy(kopf + 8,  "WAVE", 4);
  memcpy(kopf + 12, "fmt ", 4);
  *(uint32_t*)(kopf + 16) = 16;         // Länge des fmt-Blocks
  *(uint16_t*)(kopf + 20) = 1;          // 1 = PCM, also unkomprimiert
  *(uint16_t*)(kopf + 22) = 1;          // Mono
  *(uint32_t*)(kopf + 24) = ABTASTRATE;
  *(uint32_t*)(kopf + 28) = byteRate;
  *(uint16_t*)(kopf + 32) = 2;          // Bytes pro Messwert
  *(uint16_t*)(kopf + 34) = 16;         // Bits pro Messwert
  memcpy(kopf + 36, "data", 4);
  *(uint32_t*)(kopf + 40) = datenBytes;

  File f = SD.open(pfad.c_str(), "w");
  if (!f) {
    Serial.printf("[Audio] Kann %s nicht anlegen.\n", pfad.c_str());
    zustand = RUHE;                     // trotzdem freigeben, sonst hängt es
    return false;
  }
  f.write(kopf, 44);
  f.write((uint8_t*)aufnahme, datenBytes);
  f.close();

  Serial.printf("[Audio] %s gespeichert (%u KB)\n", pfad.c_str(), datenBytes / 1024);

  // Aufräumen und wieder zuhören
  aufnahmeLen = 0;
  vorlaufPos  = 0;
  vorlaufVoll = false;
  zustand     = RUHE;
  return true;
}
