#include "audio.h"
#include "config.h"
#include <ESP_I2S.h>
#include <SD.h>

// ---------------------------------------------------------------------------
//  Der Ringpuffer — das Herzstück dieses Moduls
//
//  Ein Ring ist ein Tonband, das sich in sich selbst beißt: Ist das Ende
//  erreicht, schreibt der Schreiber vorne weiter und überschreibt das
//  Älteste. Der Schreiber ist der Ton-Kern, die Leser sind Clip und Stream.
//
//  Zwei Kleinigkeiten, die das Ganze erst richtig machen:
//
//  1. Die Ringgröße ist eine ZWEIERPOTENZ (65536 Proben = 4,1 s bei 16 kHz).
//     Dann ist "Stelle im Ring" einfach `zaehler & 65535`, und der Zähler
//     darf bedenkenlos überlaufen: 2^32 ist ein Vielfaches von 65536, nach
//     dem Überlauf passt es also wieder genau. Mit z. B. 60000 Proben würde
//     beim Überlauf alles um ein paar Plätze verrutschen — ein Fehler, den
//     man erst nach 74 Stunden Laufzeit hört.
//
//  2. Geschrieben wird nur vom Ton-Kern, gelesen nur von der Hauptschleife.
//     32-Bit-Zahlen liest und schreibt der ESP32 in einem Stück, deshalb
//     brauchen wir keine Sperre ("Mutex"): Der Leser sieht immer entweder
//     den alten oder den neuen Zählerstand, nie etwas dazwischen. Wichtig
//     ist nur die Reihenfolge — erst die Proben in den Ring, DANN den
//     Zähler weiterdrehen.
// ---------------------------------------------------------------------------

#define RING_PROBEN     65536u                 // Zweierpotenz! ~4,1 s
#define RING_MASKE      (RING_PROBEN - 1u)
#define BLOCK_PROBEN    512                    // so viel holen wir am Stück

// Nur für die Gesangs-WAV (GESANG_AUFNEHMEN) — kostet sonst keinen Speicher.
#define VORLAUF_PROBEN  (TON_ABTASTRATE * AUDIO_VORLAUF_MS / 1000)
#define MAX_PROBEN      (TON_ABTASTRATE * AUDIO_MAX_MS   / 1000)

static I2SClass I2S;

static int16_t*          ring           = nullptr;
static volatile uint32_t schreibZaehler = 0;   // insgesamt geschriebene Proben
static uint32_t          lesePos [TON_ABNEHMER_ANZAHL] = {0};
static uint32_t          verloren[TON_ABNEHMER_ANZAHL] = {0};
static bool              laeuft         = false;
static volatile uint16_t lautstaerke    = 0;

// --- Gesangsaufnahme (optional) --------------------------------------------
static int16_t*  aufnahme     = nullptr;
static uint32_t  aufnahmeLen  = 0;
enum AudioZustand { RUHE, AUFNEHMEN, FERTIG };
static volatile AudioZustand zustand = RUHE;
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
//  Lauter machen. Was über die Grenze geht, wird abgeschnitten ("geclippt")
//  statt umzuklappen — umklappen würde aus einem lauten Ton ein Krachen
//  machen, das viel schlimmer klingt als abgeschnittene Spitzen.
// ---------------------------------------------------------------------------
static void verstaerken(int16_t* proben, size_t anzahl) {
  if (TON_VERSTAERKUNG <= 1) return;
  for (size_t i = 0; i < anzahl; i++) {
    int32_t v = (int32_t)proben[i] * TON_VERSTAERKUNG;
    if (v >  32767) v =  32767;
    if (v < -32768) v = -32768;
    proben[i] = (int16_t)v;
  }
}

// ---------------------------------------------------------------------------
//  Die Gesangserkennung (nur wenn GESANG_AUFNEHMEN true ist)
// ---------------------------------------------------------------------------
static void gesangPruefen(const int16_t* block, size_t proben, uint16_t laut) {
  if (!aufnahme) return;

  if (zustand == RUHE) {
    if (laut <= GESANG_SCHWELLE) return;

    // Die letzten 2 Sekunden aus dem Ring nach vorne kopieren — sonst
    // fehlt der Anfang jedes Rufs.
    uint32_t w      = schreibZaehler;
    uint32_t haben  = (w < RING_PROBEN) ? w : RING_PROBEN;
    uint32_t nehme  = min<uint32_t>(VORLAUF_PROBEN, haben);
    for (uint32_t i = 0; i < nehme; i++)
      aufnahme[i] = ring[(w - nehme + i) & RING_MASKE];

    aufnahmeLen = nehme;
    stilleSeit  = millis();
    zustand     = AUFNEHMEN;
    Serial.printf("[Audio] Gesang erkannt (Lautstaerke %u)\n", laut);
    return;
  }

  if (zustand == AUFNEHMEN) {
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
                    aufnahmeLen / (float)TON_ABTASTRATE);
    }
  }
  // Zustand FERTIG: warten, bis die Hauptschleife die Datei geschrieben hat
}

// ---------------------------------------------------------------------------
//  Die Hintergrund-Aufgabe: läuft für immer auf Kern 0
// ---------------------------------------------------------------------------
static void tonAufgabe(void* p) {
  int16_t block[BLOCK_PROBEN];

  for (;;) {
    size_t bytes  = I2S.readBytes((char*)block, sizeof(block));
    size_t proben = bytes / sizeof(int16_t);
    if (proben == 0) { vTaskDelay(pdMS_TO_TICKS(5)); continue; }

    // Erst messen (roh!), dann verstärken. So bleibt GESANG_SCHWELLE
    // unabhängig von TON_VERSTAERKUNG — sonst müsste man beim Ändern der
    // Verstärkung die Schwelle mit umrechnen.
    uint16_t laut = lautstaerkeMessen(block, proben);
    lautstaerke = laut;
    verstaerken(block, proben);

    uint32_t w = schreibZaehler;
    for (size_t i = 0; i < proben; i++)
      ring[(w + i) & RING_MASKE] = block[i];
    schreibZaehler = w + proben;        // erst JETZT für die Leser sichtbar

    if (GESANG_AUFNEHMEN) gesangPruefen(block, proben, laut);
  }
}

// ---------------------------------------------------------------------------
bool audioStart() {
  if (!AUDIO_AN) {
    Serial.println("[Audio] Ton ist in config.h abgeschaltet.");
    return false;
  }

  ring = (int16_t*)ps_malloc(RING_PROBEN * sizeof(int16_t));
  if (!ring) {
    Serial.println("[Audio] Zu wenig PSRAM fuer den Tonring (128 KB).");
    return false;
  }
  memset(ring, 0, RING_PROBEN * sizeof(int16_t));

  if (GESANG_AUFNEHMEN) {
    aufnahme = (int16_t*)ps_malloc(MAX_PROBEN * sizeof(int16_t));
    if (!aufnahme)
      Serial.println("[Audio] Zu wenig PSRAM fuer die Gesangsaufnahme.");
  }

  I2S.setPinsPdmRx(PIN_MIC_CLK, PIN_MIC_DATA);
  if (!I2S.begin(I2S_MODE_PDM_RX, TON_ABTASTRATE,
                 I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO)) {
    Serial.println("[Audio] Mikrofon startet nicht.");
    free(ring);
    ring = nullptr;
    return false;
  }

  // Kern 0 macht den Ton, Kern 1 (die normale Hauptschleife) macht das Bild
  xTaskCreatePinnedToCore(tonAufgabe, "ton", 4096, nullptr, 1, nullptr, 0);

  laeuft = true;
  Serial.printf("[Audio] Mikrofon laeuft: %u Hz, Verstaerkung %dx%s%s\n",
                (unsigned)TON_ABTASTRATE, (int)TON_VERSTAERKUNG,
                TON_IN_CLIPS  ? ", Ton in Clips" : "",
                TON_IM_STREAM ? ", Ton im Stream" : "");
  return true;
}

bool     audioLaeuft()      { return laeuft; }
uint16_t audioLautstaerke() { return lautstaerke; }
uint32_t audioAbtastrate()  { return TON_ABTASTRATE; }

// ---------------------------------------------------------------------------
//  Ton abholen
// ---------------------------------------------------------------------------
void tonAufsetzen(uint8_t abnehmer) {
  if (abnehmer >= TON_ABNEHMER_ANZAHL) return;
  lesePos[abnehmer] = schreibZaehler;
}

size_t tonAbholen(uint8_t abnehmer, int16_t* ziel, size_t maxProben) {
  if (!laeuft || !ring || abnehmer >= TON_ABNEHMER_ANZAHL) return 0;

  uint32_t w  = schreibZaehler;          // ein Blick, ein Wert
  uint32_t r  = lesePos[abnehmer];
  uint32_t da = w - r;                   // funktioniert auch über den Überlauf

  // War der Abnehmer zu langsam, ist das Älteste schon überschrieben.
  // Dann springen wir ans hinterste noch gültige Ende und merken uns,
  // wie viel verloren ging (steht auf der Website).
  if (da > RING_PROBEN) {
    verloren[abnehmer] += da - RING_PROBEN;
    r  = w - RING_PROBEN;
    da = RING_PROBEN;
  }

  size_t n = (da < maxProben) ? da : maxProben;
  for (size_t i = 0; i < n; i++) ziel[i] = ring[(r + i) & RING_MASKE];
  lesePos[abnehmer] = r + n;
  return n;
}

void tonVorlaufAbholen(int16_t* ziel, size_t proben) {
  if (!ziel || proben == 0) return;
  if (!laeuft || !ring) { memset(ziel, 0, proben * sizeof(int16_t)); return; }

  uint32_t w     = schreibZaehler;
  uint32_t haben = (w < RING_PROBEN) ? w : RING_PROBEN;
  size_t   nehme = (haben < proben) ? haben : proben;
  size_t   stille = proben - nehme;

  // Vorne mit Stille auffüllen, damit die Tonspur genauso lang wird wie
  // der Bild-Vorlauf. Sonst würden Bild und Ton auseinanderlaufen.
  if (stille) memset(ziel, 0, stille * sizeof(int16_t));
  for (size_t i = 0; i < nehme; i++)
    ziel[stille + i] = ring[(w - nehme + i) & RING_MASKE];

  lesePos[TON_ABNEHMER_CLIP] = w;
}

uint32_t tonVerloren(uint8_t abnehmer) {
  if (abnehmer >= TON_ABNEHMER_ANZAHL) return 0;
  return verloren[abnehmer];
}

// ---------------------------------------------------------------------------
//  Gesangsaufnahme als WAV-Datei speichern.
//  Eine WAV-Datei ist ganz simpel: 44 Bytes Kopf, dann die Messwerte roh.
// ---------------------------------------------------------------------------
bool audioFertig()   { return zustand == FERTIG; }
bool audioNimmtAuf() { return zustand == AUFNEHMEN; }

bool audioSpeichern(const String& pfad) {
  if (zustand != FERTIG) return false;

  uint32_t datenBytes = aufnahmeLen * sizeof(int16_t);
  uint8_t kopf[44];
  uint32_t byteRate = TON_ABTASTRATE * 2;   // 1 Kanal * 2 Bytes

  memcpy(kopf + 0,  "RIFF", 4);
  *(uint32_t*)(kopf + 4)  = 36 + datenBytes;
  memcpy(kopf + 8,  "WAVE", 4);
  memcpy(kopf + 12, "fmt ", 4);
  *(uint32_t*)(kopf + 16) = 16;         // Länge des fmt-Blocks
  *(uint16_t*)(kopf + 20) = 1;          // 1 = PCM, also unkomprimiert
  *(uint16_t*)(kopf + 22) = 1;          // Mono
  *(uint32_t*)(kopf + 24) = TON_ABTASTRATE;
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

  aufnahmeLen = 0;
  zustand     = RUHE;                   // wieder zuhören
  return true;
}
