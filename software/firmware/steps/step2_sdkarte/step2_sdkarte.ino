// ============================================================================
//  SCHRITT 2 — Die Speicherkarte
// ============================================================================
//  Ziel: Auf die SD-Karte schreiben und wieder lesen.
//        Das ist der Test, ob unsere Aufnahmen später überhaupt landen können.
//
//  Vorher: microSD-Karte in den Schlitz an der Unterseite schieben,
//          bis sie leise klickt. Sie muss als FAT32 formatiert sein
//          (das sind fast alle Karten von Werk aus).
//
//  Was du sehen solltest:
//     - die Größe deiner Karte
//     - "Test bestanden!"
//     - eine Liste der Dateien auf der Karte
// ============================================================================

#include "FS.h"
#include "SD.h"
#include "SPI.h"

#define SD_CS_PIN 21     // so ist die Karte auf diesem Board angeschlossen

void dateienZeigen(const char* ordner, int tiefe) {
  File dir = SD.open(ordner);
  if (!dir) return;

  File f;
  while ((f = dir.openNextFile())) {
    for (int i = 0; i < tiefe; i++) Serial.print("   ");
    if (f.isDirectory()) {
      Serial.printf("[Ordner] %s\n", f.name());
      if (tiefe < 2) dateienZeigen(f.path(), tiefe + 1);
    } else {
      Serial.printf("%-24s %6u Bytes\n", f.name(), (unsigned)f.size());
    }
    f.close();
  }
  dir.close();
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n=== Schritt 2: Speicherkarte ===\n");

  // Die SD-Karte hängt am SPI-Bus: SCK=7, MISO=8, MOSI=9, CS=21
  SPI.begin(7, 8, 9, SD_CS_PIN);

  if (!SD.begin(SD_CS_PIN, SPI, 20000000)) {
    Serial.println("FEHLER: Keine Karte gefunden.");
    Serial.println("Pruefe:");
    Serial.println("  1. Steckt die Karte richtig drin? (bis es klickt)");
    Serial.println("  2. Ist sie FAT32 formatiert?");
    Serial.println("  3. Ist sie hoechstens 32 GB gross?");
    return;
  }

  // Was für eine Karte ist es?
  const char* art = "unbekannt";
  switch (SD.cardType()) {
    case CARD_MMC:  art = "MMC";     break;
    case CARD_SD:   art = "SDSC";    break;
    case CARD_SDHC: art = "SDHC";    break;
    case CARD_NONE: art = "keine";   break;
    default: break;
  }
  Serial.printf("Kartentyp:  %s\n", art);
  Serial.printf("Groesse:    %llu MB\n", SD.cardSize() / (1024ULL * 1024ULL));
  Serial.printf("Belegt:     %llu MB\n", SD.usedBytes() / (1024ULL * 1024ULL));
  Serial.println();

  // ---- Schreibtest -------------------------------------------------------
  Serial.print("Schreibe Testdatei... ");
  File f = SD.open("/birdy_test.txt", FILE_WRITE);
  if (!f) { Serial.println("FEHLGESCHLAGEN (schreibgeschuetzt?)"); return; }
  f.println("Hallo vom BirdyCam!");
  f.println("Wenn du das lesen kannst, funktioniert die Karte.");
  f.close();
  Serial.println("ok");

  // ---- Lesetest ----------------------------------------------------------
  Serial.print("Lese Testdatei...    ");
  f = SD.open("/birdy_test.txt", FILE_READ);
  if (!f) { Serial.println("FEHLGESCHLAGEN"); return; }
  Serial.println("ok");
  Serial.println("--- Inhalt der Datei ---");
  while (f.available()) Serial.write(f.read());
  f.close();
  Serial.println("------------------------");

  // ---- Geschwindigkeit messen -------------------------------------------
  // Interessant, weil unsere Videoclips schnell geschrieben werden müssen.
  Serial.print("\nMesse Schreibgeschwindigkeit... ");
  uint8_t* block = (uint8_t*)malloc(4096);
  memset(block, 0xAB, 4096);
  f = SD.open("/birdy_speed.bin", FILE_WRITE);
  uint32_t start = millis();
  for (int i = 0; i < 256; i++) f.write(block, 4096);   // 1 MB
  f.close();
  uint32_t dauer = millis() - start;
  free(block);
  SD.remove("/birdy_speed.bin");
  Serial.printf("%.2f MB/s\n", 1000.0 / dauer);
  Serial.println("(alles ueber 0,5 MB/s reicht uns voellig)");

  Serial.println("\n>>> Test bestanden! <<<\n");
  Serial.println("Dateien auf der Karte:");
  dateienZeigen("/", 0);
}

void loop() {
  delay(10000);
}
