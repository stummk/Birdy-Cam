#include "speicher.h"
#include "config.h"
#include <SPI.h>
#include <time.h>

Statistik stats;

static bool     sdOk        = false;
static uint32_t letztesSpeichern = 0;
static bool     schmutzig   = false;   // "es hat sich was geändert"

// ---------------------------------------------------------------------------
//  SD-Karte starten
// ---------------------------------------------------------------------------
bool speicherStart() {
  // Die SD-Karte des XIAO Sense hängt an SPI: SCK=7, MISO=8, MOSI=9, CS=21
  SPI.begin(7, 8, 9, PIN_SD_CS);

  // Von schnell nach langsam probieren. Je schneller die Karte angesprochen
  // wird, desto weniger Bilder fallen bei Full HD aus dem Clip heraus.
  // Manche Karten oder längere Leitungen kommen bei 40 MHz nicht mit —
  // deshalb die Rückfallebenen.
  const uint32_t takte[] = { SD_TAKT_START_HZ, 20000000, 10000000 };
  uint32_t genutzterTakt = 0;

  for (uint8_t i = 0; i < 3; i++) {
    if (SD.begin(PIN_SD_CS, SPI, takte[i])) {
      genutzterTakt = takte[i];
      break;
    }
    SD.end();
    delay(50);
  }

  if (genutzterTakt == 0) {
    Serial.println("[SD] Keine Karte gefunden!");
    Serial.println("[SD] Pruefe: Karte drin (bis es klickt)? FAT32? Max. 32 GB?");
    sdOk = false;
    return false;
  }
  Serial.printf("[SD] Takt: %u MHz\n", genutzterTakt / 1000000);

  if (SD.cardType() == CARD_NONE) {
    Serial.println("[SD] Kartenschacht leer.");
    sdOk = false;
    return false;
  }

  Serial.printf("[SD] Karte da: %llu MB\n", SD.cardSize() / (1024ULL * 1024ULL));

  // Die drei Ordner anlegen, falls es sie noch nicht gibt
  if (!SD.exists("/clips"))  SD.mkdir("/clips");
  if (!SD.exists("/fotos"))  SD.mkdir("/fotos");
  if (!SD.exists("/audio"))  SD.mkdir("/audio");

  sdOk = true;
  statistikLaden();
  return true;
}

bool speicherBereit() { return sdOk; }

// ---------------------------------------------------------------------------
//  Ringspeicher
// ---------------------------------------------------------------------------
//  Wichtig: Wir benutzen IMMER die gleichen Dateinamen und überschreiben sie.
//  Dadurch wächst der Ordner nie, und die Karte muss keine neuen
//  Verzeichniseinträge anlegen. Das ist schonender als löschen + neu anlegen.
// ---------------------------------------------------------------------------
static String ringName(const char* ordner, const char* prefix,
                       const char* endung, uint16_t& zaehler, uint16_t maximum) {
  char puffer[48];
  snprintf(puffer, sizeof(puffer), "/%s/%s_%03u.%s", ordner, prefix, zaehler, endung);
  zaehler = (zaehler + 1) % maximum;
  schmutzig = true;
  return String(puffer);
}

String naechsterClipName()  { return ringName("clips", "clip",  "avi", stats.ringClip,  RING_CLIPS); }
String naechsterFotoName()  { return ringName("fotos", "foto",  "jpg", stats.ringFoto,  RING_FOTOS); }
String naechsterAudioName() { return ringName("audio", "audio", "wav", stats.ringAudio, RING_AUDIO); }

// ---------------------------------------------------------------------------
//  Statistik laden und speichern
// ---------------------------------------------------------------------------
//  Wir speichern das ganz simpel als Textzeilen — kein JSON-Parser nötig,
//  das spart Speicherplatz im Programm und ist leicht zu lesen.
// ---------------------------------------------------------------------------
void statistikLaden() {
  if (!sdOk || !SD.exists("/stats.txt")) {
    Serial.println("[Stats] Keine alte Statistik, fange bei 0 an.");
    return;
  }
  File f = SD.open("/stats.txt", FILE_READ);
  if (!f) return;

  stats.besucheGesamt = f.readStringUntil('\n').toInt();
  stats.besucheHeute  = f.readStringUntil('\n').toInt();
  stats.clipsGesamt   = f.readStringUntil('\n').toInt();
  stats.fotosGesamt   = f.readStringUntil('\n').toInt();
  stats.audioGesamt   = f.readStringUntil('\n').toInt();
  stats.letzterBesuch = f.readStringUntil('\n').toInt();
  stats.ersterHeute   = f.readStringUntil('\n').toInt();
  stats.ringClip      = f.readStringUntil('\n').toInt();
  stats.ringFoto      = f.readStringUntil('\n').toInt();
  stats.ringAudio     = f.readStringUntil('\n').toInt();
  stats.tagImJahr     = f.readStringUntil('\n').toInt();
  for (int i = 0; i < 24; i++) stats.stunden[i] = f.readStringUntil('\n').toInt();
  // Zuletzt angehängt, damit ältere stats.txt weiterhin lesbar bleiben:
  // fehlt die Zeile, liefert readStringUntil "" und toInt() gibt 0.
  stats.letzteAufenthaltS = f.readStringUntil('\n').toInt();

  f.close();
  Serial.printf("[Stats] Geladen: %u Besuche insgesamt.\n", stats.besucheGesamt);
}

void statistikSpeichern() {
  if (!sdOk) return;
  File f = SD.open("/stats.txt", FILE_WRITE);   // "w" = neu schreiben
  if (!f) return;

  f.printf("%u\n%u\n%u\n%u\n%u\n%u\n%u\n%u\n%u\n%u\n%u\n",
           stats.besucheGesamt, stats.besucheHeute, stats.clipsGesamt,
           stats.fotosGesamt, stats.audioGesamt, stats.letzterBesuch,
           stats.ersterHeute, (uint32_t)stats.ringClip, (uint32_t)stats.ringFoto,
           (uint32_t)stats.ringAudio, (uint32_t)stats.tagImJahr);
  for (int i = 0; i < 24; i++) f.printf("%u\n", stats.stunden[i]);
  f.printf("%u\n", stats.letzteAufenthaltS);

  f.close();
  schmutzig = false;
}

void statistikPflegen() {
  // Höchstens einmal pro Minute schreiben — das schont die Karte enorm.
  if (schmutzig && millis() - letztesSpeichern > 60000) {
    statistikSpeichern();
    letztesSpeichern = millis();
  }

  // Um Mitternacht "heute" zurücksetzen
  time_t jetzt = time(nullptr);
  if (jetzt < 100000) return;              // Uhrzeit noch nicht von NTP geholt
  struct tm* t = localtime(&jetzt);
  if (stats.tagImJahr != t->tm_mday) {
    stats.tagImJahr    = t->tm_mday;
    stats.besucheHeute = 0;
    stats.ersterHeute  = 0;
    schmutzig = true;
    Serial.println("[Stats] Neuer Tag, Tageszähler zurückgesetzt.");
  }
}

void besuchZaehlen() {
  time_t jetzt = time(nullptr);
  stats.besucheGesamt++;
  stats.besucheHeute++;
  stats.letzterBesuch = jetzt;
  if (stats.ersterHeute == 0) stats.ersterHeute = jetzt;

  if (jetzt > 100000) {
    struct tm* t = localtime(&jetzt);
    stats.stunden[t->tm_hour]++;
  }
  schmutzig = true;
}

uint8_t kartenBelegungProzent() {
  if (!sdOk) return 0;
  uint64_t gesamt = SD.totalBytes();
  if (gesamt == 0) return 0;
  return (uint8_t)((SD.usedBytes() * 100ULL) / gesamt);
}

// ---------------------------------------------------------------------------
//  JSON für die Website — von Hand gebaut, spart eine ganze Bibliothek
// ---------------------------------------------------------------------------
String statistikAlsJson() {
  String j = "{";
  j += "\"besucheGesamt\":" + String(stats.besucheGesamt);
  j += ",\"besucheHeute\":"  + String(stats.besucheHeute);
  j += ",\"clips\":"         + String(stats.clipsGesamt);
  j += ",\"fotos\":"         + String(stats.fotosGesamt);
  j += ",\"audio\":"         + String(stats.audioGesamt);
  j += ",\"letzterBesuch\":" + String(stats.letzterBesuch);
  j += ",\"ersterHeute\":"   + String(stats.ersterHeute);
  j += ",\"letzteDauer\":"   + String(stats.letzteAufenthaltS);
  j += ",\"belegung\":"      + String(kartenBelegungProzent());
  j += ",\"stunden\":[";
  for (int i = 0; i < 24; i++) {
    if (i) j += ",";
    j += String(stats.stunden[i]);
  }
  j += "]}";
  return j;
}
