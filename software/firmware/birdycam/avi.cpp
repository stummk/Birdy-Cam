#include "avi.h"
#include <SD.h>

// ---------------------------------------------------------------------------
//  Aufbau einer AVI-Datei (das "Deckblatt" ist immer genau 224 Bytes lang):
//
//    Byte   0  'RIFF'  + Gesamtgröße + 'AVI '
//    Byte  12  'LIST'  hdrl   ("Kopfdaten")
//    Byte  24  'avih'         (Angaben zum ganzen Film)
//    Byte  88  'LIST'  strl   ("Spur-Daten")
//    Byte 100  'strh'         (Angaben zur Videospur)
//    Byte 164  'strf'         (Bildformat: MJPG)
//    Byte 212  'LIST'  movi   (ab hier kommen die Bilder)
//    Byte 224  erstes Bild
//    ...
//    am Ende: 'idx1' — ein Inhaltsverzeichnis, damit man spulen kann
//
//  Ein paar Zahlen im Deckblatt kennen wir erst am ENDE (z.B. wie viele
//  Bilder es geworden sind). Deshalb schreiben wir erst Platzhalter und
//  springen zum Schluss zurück und tragen die richtigen Werte ein.
// ---------------------------------------------------------------------------

#define KOPF_GROESSE   224
#define MAX_BILDER     900     // Sicherheitsgrenze fürs Inhaltsverzeichnis

static File      datei;
static bool      laeuft      = false;
static uint32_t  bildAnzahl  = 0;
static uint32_t  moviBytes   = 0;   // Größe aller Bilddaten inkl. Chunk-Köpfe
static uint32_t  maxBildByte = 0;
static uint16_t  vBreite     = 0;
static uint16_t  vHoehe      = 0;
static uint8_t*  idxPuffer   = nullptr;   // 16 Bytes pro Bild
static uint32_t  idxAnzahl   = 0;

// --- Selbstdiagnose der SD-Karte ------------------------------------------
static uint32_t  verworfen     = 0;       // Bilder, die nicht draufpassten
static uint64_t  schreibBytes  = 0;       // insgesamt geschrieben
static uint64_t  schreibZeitUs = 0;       // wie lange das gedauert hat

// --- kleine Helfer: Zahlen "verkehrt herum" schreiben (Little Endian) ------
static void setzeLE32(uint8_t* p, uint32_t w) {
  p[0] = w & 0xFF; p[1] = (w >> 8) & 0xFF; p[2] = (w >> 16) & 0xFF; p[3] = (w >> 24) & 0xFF;
}
static void setzeLE16(uint8_t* p, uint16_t w) {
  p[0] = w & 0xFF; p[1] = (w >> 8) & 0xFF;
}
static void setzeTag(uint8_t* p, const char* tag) { memcpy(p, tag, 4); }

// ---------------------------------------------------------------------------
bool aviStart(const String& pfad, uint16_t breite, uint16_t hoehe) {
  if (laeuft) aviEnde(8.0f);

  datei = SD.open(pfad.c_str(), "w+");      // w+ = neu anlegen, lesen+schreiben
  if (!datei) {
    Serial.printf("[AVI] Kann %s nicht anlegen.\n", pfad.c_str());
    return false;
  }

  if (!idxPuffer) {
    idxPuffer = (uint8_t*)ps_malloc(MAX_BILDER * 16);
    if (!idxPuffer) {
      Serial.println("[AVI] Kein PSRAM fuer das Inhaltsverzeichnis.");
      datei.close();
      return false;
    }
  }

  vBreite = breite;
  vHoehe  = hoehe;

  // Deckblatt mit Platzhaltern bauen
  uint8_t kopf[KOPF_GROESSE];
  memset(kopf, 0, KOPF_GROESSE);

  setzeTag(kopf + 0,  "RIFF");
  setzeLE32(kopf + 4, 0);                  // Gesamtgröße -> später
  setzeTag(kopf + 8,  "AVI ");

  setzeTag(kopf + 12, "LIST");
  setzeLE32(kopf + 16, 192);               // Größe des hdrl-Blocks
  setzeTag(kopf + 20, "hdrl");

  setzeTag(kopf + 24, "avih");
  setzeLE32(kopf + 28, 56);
  setzeLE32(kopf + 32, 125000);            // Mikrosekunden pro Bild -> später
  setzeLE32(kopf + 36, 0);                 // max. Bytes pro Sekunde
  setzeLE32(kopf + 40, 0);
  setzeLE32(kopf + 44, 0x10);              // Flag: hat ein Inhaltsverzeichnis
  setzeLE32(kopf + 48, 0);                 // Anzahl Bilder -> später
  setzeLE32(kopf + 52, 0);
  setzeLE32(kopf + 56, 1);                 // eine Spur (nur Video)
  setzeLE32(kopf + 60, 0);
  setzeLE32(kopf + 64, breite);
  setzeLE32(kopf + 68, hoehe);

  setzeTag(kopf + 88, "LIST");
  setzeLE32(kopf + 92, 116);
  setzeTag(kopf + 96, "strl");

  setzeTag(kopf + 100, "strh");
  setzeLE32(kopf + 104, 56);
  setzeTag(kopf + 108, "vids");            // Spur-Art: Video
  setzeTag(kopf + 112, "MJPG");            // Format: Motion JPEG
  setzeLE32(kopf + 116, 0);
  setzeLE16(kopf + 120, 0);
  setzeLE16(kopf + 122, 0);
  setzeLE32(kopf + 124, 0);
  setzeLE32(kopf + 128, 1);                // Zeit-Teiler  -> später
  setzeLE32(kopf + 132, 8);                // Bilder/Sek.  -> später
  setzeLE32(kopf + 136, 0);
  setzeLE32(kopf + 140, 0);                // Länge in Bildern -> später
  setzeLE32(kopf + 144, 0);
  setzeLE32(kopf + 148, 0xFFFFFFFF);       // Qualität: unbekannt
  setzeLE32(kopf + 152, 0);
  setzeLE16(kopf + 156, 0);
  setzeLE16(kopf + 158, 0);
  setzeLE16(kopf + 160, breite);
  setzeLE16(kopf + 162, hoehe);

  setzeTag(kopf + 164, "strf");
  setzeLE32(kopf + 168, 40);
  setzeLE32(kopf + 172, 40);               // Größe dieser Struktur
  setzeLE32(kopf + 176, breite);
  setzeLE32(kopf + 180, hoehe);
  setzeLE16(kopf + 184, 1);                // Ebenen
  setzeLE16(kopf + 186, 24);               // Bits pro Pixel
  setzeTag(kopf + 188, "MJPG");
  setzeLE32(kopf + 192, breite * hoehe * 3);

  setzeTag(kopf + 212, "LIST");
  setzeLE32(kopf + 216, 0);                // Größe des movi-Blocks -> später
  setzeTag(kopf + 220, "movi");

  if (datei.write(kopf, KOPF_GROESSE) != KOPF_GROESSE) {
    Serial.println("[AVI] Deckblatt konnte nicht geschrieben werden.");
    datei.close();
    return false;
  }

  bildAnzahl  = 0;
  moviBytes   = 4;      // die 4 Bytes von 'movi' zählen mit
  maxBildByte = 0;
  idxAnzahl   = 0;
  laeuft      = true;
  return true;
}

// ---------------------------------------------------------------------------
bool aviFrame(const uint8_t* daten, size_t laenge) {
  if (!laeuft || bildAnzahl >= MAX_BILDER) { verworfen++; return false; }

  // Jedes Bild bekommt einen kleinen Kopf: '00dc' + Länge
  uint8_t chunkKopf[8];
  setzeTag(chunkKopf, "00dc");
  setzeLE32(chunkKopf + 4, laenge);

  // Position dieses Bildes merken (fürs Inhaltsverzeichnis am Ende)
  uint32_t versatz = moviBytes;

  uint32_t t0 = micros();

  if (datei.write(chunkKopf, 8) != 8)      { verworfen++; return false; }
  if (datei.write(daten, laenge) != laenge) { verworfen++; return false; }

  // AVI verlangt gerade Byte-Anzahlen -> ggf. ein Füllbyte
  size_t pad = laenge & 1;
  if (pad) { uint8_t null = 0; datei.write(&null, 1); }

  // Für die Selbstdiagnose: wie schnell schreibt die Karte wirklich?
  schreibZeitUs += (micros() - t0);
  schreibBytes  += 8 + laenge + pad;

  // Inhaltsverzeichnis-Eintrag: Tag, "ist Vollbild", Position, Länge
  uint8_t* e = idxPuffer + idxAnzahl * 16;
  setzeTag(e, "00dc");
  setzeLE32(e + 4, 0x10);          // AVIIF_KEYFRAME — jedes JPEG ist ein Vollbild
  setzeLE32(e + 8, versatz);
  setzeLE32(e + 12, laenge);
  idxAnzahl++;

  moviBytes += 8 + laenge + pad;
  bildAnzahl++;
  if (laenge > maxBildByte) maxBildByte = laenge;
  return true;
}

// ---------------------------------------------------------------------------
void aviEnde(float bilderProSekunde) {
  if (!laeuft) return;

  // 1) Inhaltsverzeichnis anhängen
  uint8_t idxKopf[8];
  setzeTag(idxKopf, "idx1");
  setzeLE32(idxKopf + 4, idxAnzahl * 16);
  datei.write(idxKopf, 8);
  datei.write(idxPuffer, idxAnzahl * 16);

  uint32_t gesamt = KOPF_GROESSE + (moviBytes - 4) + 8 + idxAnzahl * 16;

  if (bilderProSekunde < 1.0f) bilderProSekunde = 1.0f;
  uint32_t usProBild = (uint32_t)(1000000.0f / bilderProSekunde);

  // 2) Zurückspringen und die Platzhalter durch echte Werte ersetzen
  uint8_t z[4];

  datei.seek(4);    setzeLE32(z, gesamt - 8);        datei.write(z, 4);
  datei.seek(32);   setzeLE32(z, usProBild);         datei.write(z, 4);
  datei.seek(48);   setzeLE32(z, bildAnzahl);        datei.write(z, 4);
  datei.seek(60);   setzeLE32(z, maxBildByte);       datei.write(z, 4);
  datei.seek(128);  setzeLE32(z, 1000);              datei.write(z, 4);   // Teiler
  datei.seek(132);  setzeLE32(z, (uint32_t)(bilderProSekunde * 1000.0f)); datei.write(z, 4);
  datei.seek(140);  setzeLE32(z, bildAnzahl);        datei.write(z, 4);
  datei.seek(144);  setzeLE32(z, maxBildByte);       datei.write(z, 4);
  datei.seek(216);  setzeLE32(z, moviBytes);         datei.write(z, 4);

  datei.close();    // <- ganz wichtig: sofort schließen, nicht offen lassen!
  laeuft = false;

  Serial.printf("[AVI] Clip fertig: %u Bilder, %.1f fps, %u KB "
                "(SD schreibt %.2f MB/s%s)\n",
                bildAnzahl, bilderProSekunde, gesamt / 1024,
                aviSchreibrateMBs(),
                verworfen ? ", ACHTUNG Bilder verworfen!" : "");
}

bool     aviLaeuft()     { return laeuft; }
uint32_t aviBildAnzahl() { return bildAnzahl; }
uint32_t aviVerworfen()  { return verworfen; }

float aviSchreibrateMBs() {
  if (schreibZeitUs == 0) return 0.0f;
  // Bytes pro Mikrosekunde -> MB/s ist zufällig derselbe Zahlenwert,
  // weil 1 Byte/µs genau 1 MB/s entspricht (1e6 Bytes pro Sekunde).
  return (float)schreibBytes / (float)schreibZeitUs;
}
