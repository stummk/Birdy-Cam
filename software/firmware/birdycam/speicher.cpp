#include "speicher.h"
#include "config.h"
#include <SPI.h>
#include <time.h>

Statistik stats;

static bool     sdOk        = false;
static uint32_t letztesSpeichern = 0;
static bool     schmutzig   = false;   // "es hat sich was geändert"
static uint32_t letzteArchivStunde = 99;

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
//
//  Neue Werte werden immer HINTEN angehängt. Dann kann eine alte stats.txt
//  weiter gelesen werden: fehlt eine Zeile, liefert readStringUntil "" und
//  toInt() gibt 0.
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
  stats.letzteAufenthaltS = f.readStringUntil('\n').toInt();

  // Ab hier die Werte für die Tagesstatistik (/tage.csv)
  stats.clipsHeute        = f.readStringUntil('\n').toInt();
  stats.fotosHeute        = f.readStringUntil('\n').toInt();
  stats.audioHeute        = f.readStringUntil('\n').toInt();
  stats.sekundenDrinHeute = f.readStringUntil('\n').toInt();
  stats.akkuMinMv         = f.readStringUntil('\n').toInt();
  stats.akkuMaxMv         = f.readStringUntil('\n').toInt();
  stats.tagDatum          = f.readStringUntil('\n').toInt();

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
  f.printf("%u\n%u\n%u\n%u\n%u\n%u\n%u\n",
           stats.clipsHeute, stats.fotosHeute, stats.audioHeute,
           stats.sekundenDrinHeute, (uint32_t)stats.akkuMinMv,
           (uint32_t)stats.akkuMaxMv, stats.tagDatum);

  f.close();
  schmutzig = false;
}

// ---------------------------------------------------------------------------
//  Tagesarchiv /tage.csv
// ---------------------------------------------------------------------------
//  Eine Zeile je Tag, alle Zeilen gleich lang. Geschrieben wird sie
//  mehrfach: einmal pro Stunde für den laufenden Tag (damit ein Stromausfall
//  nie einen ganzen Tag kostet) und einmal endgültig um Mitternacht.
//
//  Der Trick, der das erlaubt: Weil alle Zeilen gleich lang sind, können wir
//  die LETZTE Zeile lesen und — wenn sie vom selben Tag ist — genau an ihre
//  Stelle zurückspringen und sie überschreiben. Ohne feste Zeilenlänge
//  müsste man dafür die ganze Datei neu schreiben.
// ---------------------------------------------------------------------------
static const char TAGE_KOPF[] =
  "datum;besuche;erster;letzter;drin_s;clips;fotos;audio;akku_min;akku_max;"
  "h00;h01;h02;h03;h04;h05;h06;h07;h08;h09;h10;h11;h12;h13;h14;h15;h16;h17;"
  "h18;h19;h20;h21;h22;h23\n";

// "HH:MM" oder "--:--" — immer genau 5 Zeichen, sonst wäre die Zeile nicht
// mehr gleich lang.
static void zeitFeld(uint32_t unixzeit, bool gueltig, char* aus6) {
  if (!gueltig || unixzeit < 100000) { memcpy(aus6, "--:--", 6); return; }
  time_t t = (time_t)unixzeit;
  struct tm* z = localtime(&t);
  snprintf(aus6, 6, "%02u:%02u", (unsigned)z->tm_hour, (unsigned)z->tm_min);
}

static bool tagArchivSchreiben(uint32_t datum) {
  if (!TAGE_CSV_AN || !sdOk || datum < 20000000UL) return false;

  char e[6], l[6];
  bool hatBesuche = (stats.besucheHeute > 0);
  zeitFeld(stats.ersterHeute,   hatBesuche, e);
  zeitFeld(stats.letzterBesuch, hatBesuche, l);

  char zeile[TAGE_ZEILE_BYTES + 16];
  int n = snprintf(zeile, sizeof(zeile),
                   "%04u-%02u-%02u;%04u;%s;%s;%06u;%04u;%04u;%04u;%.2f;%.2f",
                   (unsigned)(datum / 10000),
                   (unsigned)((datum / 100) % 100),
                   (unsigned)(datum % 100),
                   (unsigned)min<uint32_t>(stats.besucheHeute, 9999),
                   e, l,
                   (unsigned)min<uint32_t>(stats.sekundenDrinHeute, 999999),
                   (unsigned)min<uint32_t>(stats.clipsHeute, 9999),
                   (unsigned)min<uint32_t>(stats.fotosHeute, 9999),
                   (unsigned)min<uint32_t>(stats.audioHeute, 9999),
                   stats.akkuMinMv / 1000.0f, stats.akkuMaxMv / 1000.0f);
  for (int i = 0; i < 24; i++)
    n += snprintf(zeile + n, sizeof(zeile) - n, ";%03u",
                  (unsigned)min<uint32_t>(stats.stunden[i], 999));
  n += snprintf(zeile + n, sizeof(zeile) - n, "\n");

  if (n != TAGE_ZEILE_BYTES) {
    // Sollte nie passieren — aber wenn doch, dann lieber laut sein, denn
    // dann stimmt das Zurückspringen nicht mehr.
    static bool gewarnt = false;
    if (!gewarnt) {
      gewarnt = true;
      Serial.printf("[Tage] WARNUNG: Zeile ist %d statt %d Bytes lang.\n",
                    n, TAGE_ZEILE_BYTES);
    }
  }

  // Gibt es die Datei schon, und ist die letzte Zeile von HEUTE?
  bool ersetzen = false;
  uint32_t schreibAb = 0;
  if (SD.exists(TAGE_DATEI)) {
    File p = SD.open(TAGE_DATEI, FILE_READ);
    if (p) {
      uint32_t gr = p.size();
      if (gr >= TAGE_ZEILE_BYTES) {
        p.seek(gr - TAGE_ZEILE_BYTES);
        char anfang[11] = {0};
        p.readBytes(anfang, 10);
        if (strncmp(anfang, zeile, 10) == 0) {   // gleiches Datum
          ersetzen  = true;
          schreibAb = gr - TAGE_ZEILE_BYTES;
        }
      }
      p.close();
    }
  }

  File f;
  if (ersetzen) {
    f = SD.open(TAGE_DATEI, "r+");           // öffnen zum Ändern
    if (f) f.seek(schreibAb);
  } else {
    bool neu = !SD.exists(TAGE_DATEI);
    f = SD.open(TAGE_DATEI, FILE_APPEND);
    if (f && neu) f.print(TAGE_KOPF);        // Spaltennamen für Excel
  }
  if (!f) {
    Serial.println("[Tage] Kann /tage.csv nicht schreiben.");
    return false;
  }
  f.print(zeile);
  f.close();
  return true;
}

bool tagJetztSichern() { return tagArchivSchreiben(stats.tagDatum); }

// Alles, was "heute" bedeutet, auf null stellen.
static void tagZuruecksetzen() {
  stats.besucheHeute      = 0;
  stats.ersterHeute       = 0;
  stats.clipsHeute        = 0;
  stats.fotosHeute        = 0;
  stats.audioHeute        = 0;
  stats.sekundenDrinHeute = 0;
  stats.akkuMinMv         = 0;
  stats.akkuMaxMv         = 0;
  for (int i = 0; i < 24; i++) stats.stunden[i] = 0;
}

// ---------------------------------------------------------------------------
void statistikPflegen() {
  // Höchstens einmal pro Minute schreiben — das schont die Karte enorm.
  if (schmutzig && millis() - letztesSpeichern > 60000) {
    statistikSpeichern();
    letztesSpeichern = millis();
  }

  time_t jetzt = time(nullptr);
  if (jetzt < 100000) return;              // Uhrzeit noch nicht gestellt
  struct tm* t = localtime(&jetzt);

  uint32_t heute = (uint32_t)(t->tm_year + 1900) * 10000UL
                 + (uint32_t)(t->tm_mon + 1) * 100UL
                 + (uint32_t)t->tm_mday;

  if (stats.tagDatum == 0) {
    // Erster Start mit gestellter Uhr: Tag übernehmen, nichts archivieren.
    //
    // Kommt die stats.txt von einer älteren Firmware, hatten die Stundenwerte
    // dort eine andere Bedeutung — sie zählten über ALLE Tage zusammen, nicht
    // pro Tag. Einmal leeren, sonst stünde in der ersten Archivzeile Unsinn.
    if (stats.besucheGesamt > 0) {
      for (int i = 0; i < 24; i++) stats.stunden[i] = 0;
      Serial.println("[Tage] Umstellung: Stundenwerte gelten ab jetzt je Tag.");
    }
    stats.tagDatum  = heute;
    stats.tagImJahr = t->tm_mday;
    schmutzig = true;
  } else if (stats.tagDatum != heute) {
    // Mitternacht (oder die Kamera war ein paar Tage aus): den Tag, der zu
    // Ende ging, endgültig ins Archiv schreiben und dann bei 0 anfangen.
    tagArchivSchreiben(stats.tagDatum);
    Serial.printf("[Tage] %u archiviert: %u Besuche. Neuer Tag.\n",
                  stats.tagDatum, stats.besucheHeute);
    tagZuruecksetzen();
    stats.tagDatum  = heute;
    stats.tagImJahr = t->tm_mday;
    statistikSpeichern();                  // sofort, nicht erst in einer Minute
    letztesSpeichern   = millis();
    letzteArchivStunde = t->tm_hour;
  }

  // Einmal pro Stunde den laufenden Tag mitschreiben. 156 Bytes — dafür ist
  // nach einem Stromausfall höchstens die letzte Stunde verloren und nicht
  // der ganze Tag.
  if (TAGE_CSV_AN && (uint32_t)t->tm_hour != letzteArchivStunde) {
    letzteArchivStunde = t->tm_hour;
    tagArchivSchreiben(stats.tagDatum);
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

void aufenthaltZaehlen(uint32_t sekunden) {
  stats.letzteAufenthaltS  = sekunden;
  stats.sekundenDrinHeute += sekunden;
  schmutzig = true;
}

void akkuHeuteMerken(float volt) {
  if (volt < 0.5f) return;                 // kein Sensor angeschlossen
  uint16_t mv = (uint16_t)(volt * 1000.0f + 0.5f);
  if (stats.akkuMaxMv == 0 || mv > stats.akkuMaxMv) { stats.akkuMaxMv = mv; schmutzig = true; }
  if (stats.akkuMinMv == 0 || mv < stats.akkuMinMv) { stats.akkuMinMv = mv; schmutzig = true; }
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
  j += ",\"clipsHeute\":"    + String(stats.clipsHeute);
  j += ",\"letzterBesuch\":" + String(stats.letzterBesuch);
  j += ",\"ersterHeute\":"   + String(stats.ersterHeute);
  j += ",\"letzteDauer\":"   + String(stats.letzteAufenthaltS);
  j += ",\"drinHeuteS\":"    + String(stats.sekundenDrinHeute);
  j += ",\"akkuMinHeute\":"  + String(stats.akkuMinMv / 1000.0f, 2);
  j += ",\"akkuMaxHeute\":"  + String(stats.akkuMaxMv / 1000.0f, 2);
  j += ",\"belegung\":"      + String(kartenBelegungProzent());
  j += ",\"stunden\":[";
  for (int i = 0; i < 24; i++) {
    if (i) j += ",";
    j += String(stats.stunden[i]);
  }
  j += "]}";
  return j;
}

// ---------------------------------------------------------------------------
//  Die letzten Tage aus /tage.csv als JSON
// ---------------------------------------------------------------------------
//  Hier zahlt sich die feste Zeilenlänge aus: Wir springen direkt an die
//  Stelle, wo die gewünschten Tage anfangen, statt die ganze Datei zu lesen.
//  Eine Zeile mehr wird gelesen und weggeworfen ("Resync") — dann stehen
//  wir garantiert am Anfang einer Zeile, selbst wenn eine Zeile mal anders
//  lang wäre.
// ---------------------------------------------------------------------------
String tageAlsJson(uint16_t anzahl) {
  if (anzahl == 0)  anzahl = 1;
  if (anzahl > 60)  anzahl = 60;          // Arbeitsspeicher-Bremse

  String j = "{\"tage\":[";
  uint32_t summe[24] = {0};
  uint16_t tage = 0;

  File f = (sdOk && SD.exists(TAGE_DATEI)) ? SD.open(TAGE_DATEI, FILE_READ) : File();
  if (f) {
    uint32_t groesse = f.size();
    uint32_t brauche = (uint32_t)(anzahl + 1) * TAGE_ZEILE_BYTES;
    if (groesse > brauche) {
      f.seek(groesse - brauche);
      f.readStringUntil('\n');            // angebrochene Zeile wegwerfen
    }

    char zeile[TAGE_ZEILE_BYTES + 16];
    while (f.available()) {
      int len = f.readBytesUntil('\n', zeile, sizeof(zeile) - 1);
      if (len <= 0) break;
      zeile[len] = 0;
      if (len && zeile[len - 1] == '\r') zeile[len - 1] = 0;
      if (zeile[0] < '0' || zeile[0] > '9') continue;   // Kopfzeile

      char* feld[34];
      uint8_t anz = 0;
      char* p = strtok(zeile, ";");
      while (p && anz < 34) { feld[anz++] = p; p = strtok(nullptr, ";"); }
      if (anz < 10) continue;

      if (tage) j += ",";
      j += "{\"datum\":\"";   j += feld[0];
      j += "\",\"besuche\":";  j += String(atol(feld[1]));
      j += ",\"erster\":\"";   j += feld[2];
      j += "\",\"letzter\":\"";j += feld[3];
      j += "\",\"drinS\":";    j += String(atol(feld[4]));
      j += ",\"clips\":";      j += String(atol(feld[5]));
      j += ",\"fotos\":";      j += String(atol(feld[6]));
      j += ",\"audio\":";      j += String(atol(feld[7]));
      j += ",\"akkuMin\":";    j += String(atof(feld[8]), 2);
      j += ",\"akkuMax\":";    j += String(atof(feld[9]), 2);
      j += "}";

      for (uint8_t i = 0; i < 24 && (10 + i) < anz; i++)
        summe[i] += atol(feld[10 + i]);
      tage++;
    }
    f.close();
  }

  j += "],\"tageAnzahl\":" + String(tage);
  j += ",\"stundenSchnitt\":[";
  for (int i = 0; i < 24; i++) {
    if (i) j += ",";
    j += String(tage ? (summe[i] / (float)tage) : 0.0f, 1);
  }
  j += "]}";
  return j;
}
