// ============================================================================
//  SCHRITT 6 — Zuhören
// ============================================================================
//  Ziel: Das Mikrofon ausprobieren, einen Lautstärkebalken bauen und eine
//        echte Tonaufnahme auf die SD-Karte schreiben.
//
//  Nichts zu verkabeln! Das Mikrofon ist auf dem Board schon fest verbaut.
//
//  Was du machen kannst:
//     - reinpusten, pfeifen, in die Hände klatschen -> der Balken zappelt
//     - den Wert "Lautstaerke" bei Stille ablesen und bei Pfeifen
//       -> daraus ergibt sich die GESANG_SCHWELLE für config.h!
//     - im Seriellen Monitor "r" eingeben und Enter -> 5 Sekunden Aufnahme
//     - Karte in den Computer stecken und test.wav anhören
//
//  ⚠️ Das Mikrofon auf dem XIAO ist SEHR leise. Eine rohe Aufnahme klingt
//     wie nichts. Deshalb wird für die WAV-Datei verstärkt (VERSTAERKUNG
//     unten). Die angezeigte Zahl bleibt absichtlich UNVERSTÄRKT — nur so
//     passt sie zu GESANG_SCHWELLE in config.h, denn die Gesangserkennung
//     in der Firmware rechnet ebenfalls mit den rohen Werten.
// ============================================================================

#include <ESP_I2S.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"

#define SD_CS_PIN     21
#define MIC_CLK       42     // fest verbaut
#define MIC_DATA      41     // fest verbaut
#define ABTASTRATE    16000  // 16000 Messungen pro Sekunde
#define BLOCK         512

// Verstärkung für die WAV-Datei. 1 = roh (viel zu leise), 16 ist erprobt.
// Knackt es in der Aufnahme, ist der Wert zu hoch -> 8 probieren.
// Denselben Wert traegst du spaeter in config.h bei TON_VERSTAERKUNG ein.
#define VERSTAERKUNG  16

// Bei welchem Lautstaerkewert der Balken voll ausschlaegt. Nur Optik —
// an der angezeigten Zahl aendert das nichts.
#define BALKEN_VOLL   1500

I2SClass I2S;
int16_t  block[BLOCK];
bool     sdDa = false;

// ---------------------------------------------------------------------------
//  Lautstärke schätzen — mit Betonung auf hohe Töne.
//  Trick: die Differenz zwischen benachbarten Messwerten. Tiefe Töne
//  (Brummen, Wind) aendern sich langsam, hohe Toene (Vogelgesang) schnell.
// ---------------------------------------------------------------------------
uint16_t lautstaerke(const int16_t* p, size_t n) {
  if (n < 2) return 0;
  uint64_t summe = 0;
  for (size_t i = 1; i < n; i++) {
    int32_t d = (int32_t)p[i] - (int32_t)p[i-1];
    summe += (uint64_t)((int64_t)d * d);
  }
  return (uint16_t)min<uint64_t>(65535, sqrt((double)(summe / (n - 1))));
}

// ---------------------------------------------------------------------------
//  Lauter machen. Was über die Grenze geht, wird abgeschnitten ("geclippt")
//  statt umzuklappen — umklappen würde aus einem lauten Ton ein Krachen
//  machen, das viel schlimmer klingt als abgeschnittene Spitzen.
// ---------------------------------------------------------------------------
void verstaerken(int16_t* p, size_t n) {
  if (VERSTAERKUNG <= 1) return;
  for (size_t i = 0; i < n; i++) {
    int32_t v = (int32_t)p[i] * VERSTAERKUNG;
    if (v >  32767) v =  32767;
    if (v < -32768) v = -32768;
    p[i] = (int16_t)v;
  }
}

void balken(uint16_t wert) {
  int laenge = map(min<uint16_t>(wert, BALKEN_VOLL), 0, BALKEN_VOLL, 0, 40);
  Serial.print("[");
  for (int i = 0; i < 40; i++) Serial.print(i < laenge ? '#' : ' ');
  Serial.printf("] %5u\n", wert);
}

// ---------------------------------------------------------------------------
//  5 Sekunden aufnehmen und als WAV speichern.
//  Eine WAV-Datei ist einfach: 44 Bytes Kopfdaten, dann die Messwerte roh.
// ---------------------------------------------------------------------------
void aufnehmen() {
  if (!sdDa) { Serial.println("Keine SD-Karte - kann nicht aufnehmen."); return; }

  const uint32_t sekunden   = 5;
  const uint32_t datenBytes = ABTASTRATE * 2 * sekunden;

  Serial.println("\n>>> AUFNAHME LAEUFT - sag was! <<<");

  File f = SD.open("/test.wav", FILE_WRITE);
  if (!f) { Serial.println("Kann /test.wav nicht anlegen."); return; }

  // --- WAV-Kopf ---
  uint8_t k[44];
  memcpy(k + 0, "RIFF", 4);   *(uint32_t*)(k + 4)  = 36 + datenBytes;
  memcpy(k + 8, "WAVE", 4);
  memcpy(k + 12,"fmt ", 4);   *(uint32_t*)(k + 16) = 16;
  *(uint16_t*)(k + 20) = 1;                 // PCM = unkomprimiert
  *(uint16_t*)(k + 22) = 1;                 // Mono
  *(uint32_t*)(k + 24) = ABTASTRATE;
  *(uint32_t*)(k + 28) = ABTASTRATE * 2;
  *(uint16_t*)(k + 32) = 2;
  *(uint16_t*)(k + 34) = 16;                // 16 Bit
  memcpy(k + 36,"data", 4);   *(uint32_t*)(k + 40) = datenBytes;
  f.write(k, 44);

  uint32_t geschrieben = 0;
  while (geschrieben < datenBytes) {
    size_t bytes = I2S.readBytes((char*)block, BLOCK * sizeof(int16_t));
    if (bytes == 0) continue;

    // Nur die Datei wird verstaerkt — die Anzeige unten bleibt roh.
    verstaerken(block, bytes / sizeof(int16_t));

    f.write((uint8_t*)block, bytes);
    geschrieben += bytes;
    if (geschrieben % (ABTASTRATE * 2) < BLOCK * 2)
      Serial.printf("   %u von %u Sekunden\n", geschrieben / (ABTASTRATE * 2), sekunden);
  }
  f.close();

  Serial.println(">>> Fertig! /test.wav liegt auf der Karte. <<<");
  Serial.println("    Karte in den Computer stecken und anhoeren.\n");
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n=== Schritt 6: Mikrofon ===\n");

  // SD-Karte (optional, nur fuer die Aufnahme)
  SPI.begin(7, 8, 9, SD_CS_PIN);
  sdDa = SD.begin(SD_CS_PIN, SPI, 20000000);
  Serial.println(sdDa ? "SD-Karte: da" : "SD-Karte: fehlt (Balken geht trotzdem)");

  // Mikrofon starten
  I2S.setPinsPdmRx(MIC_CLK, MIC_DATA);
  if (!I2S.begin(I2S_MODE_PDM_RX, ABTASTRATE,
                 I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO)) {
    Serial.println("FEHLER: Mikrofon startet nicht.");
    return;
  }
  Serial.println("Mikrofon: laeuft\n");

  Serial.println("Pfeife, klatsche, rede - und schau dem Balken zu.");
  Serial.println("Tippe 'r' + Enter fuer eine 5-Sekunden-Aufnahme.\n");
  Serial.println("MERKE DIR ZWEI ZAHLEN (die angezeigte Zahl ist ROH,");
  Serial.println("also unverstaerkt - genau so braucht sie config.h):");
  Serial.println("  - der Wert bei Stille           (z. B. 200)");
  Serial.println("  - der Wert beim Pfeifen         (z. B. 1200)");
  Serial.println("Die GESANG_SCHWELLE in config.h liegt gut in der Mitte.\n");
  Serial.printf ("Die Aufnahme wird %dx verstaerkt - derselbe Wert gehoert\n",
                 VERSTAERKUNG);
  Serial.println("spaeter in config.h zu TON_VERSTAERKUNG.\n");
}

void loop() {
  if (Serial.available()) {
    char z = Serial.read();
    if (z == 'r' || z == 'R') aufnehmen();
  }

  size_t bytes = I2S.readBytes((char*)block, BLOCK * sizeof(int16_t));
  if (bytes > 0) {
    // Bewusst NICHT verstaerken: Der angezeigte Wert soll der rohe sein,
    // sonst passt er nicht zu GESANG_SCHWELLE in config.h. Damit der Balken
    // trotzdem sichtbar zappelt, ist stattdessen sein Vollausschlag klein
    // gewaehlt (BALKEN_VOLL).
    balken(lautstaerke(block, bytes / sizeof(int16_t)));
  }

  delay(100);
}
