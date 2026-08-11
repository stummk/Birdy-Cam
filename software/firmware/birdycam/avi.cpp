#include "avi.h"
#include <SD.h>

// ---------------------------------------------------------------------------
//  Aufbau einer AVI-Datei
//
//  OHNE Ton ist das "Deckblatt" genau 224 Bytes lang:
//
//    Byte   0  'RIFF'  + Gesamtgröße + 'AVI '
//    Byte  12  'LIST'  hdrl   ("Kopfdaten")
//    Byte  24  'avih'         (Angaben zum ganzen Film)
//    Byte  88  'LIST'  strl   ("Spur-Daten", Spur 0)
//    Byte 100  'strh'         (Angaben zur Videospur)
//    Byte 164  'strf'         (Bildformat: MJPG)
//    Byte 212  'LIST'  movi   (ab hier kommen die Bilder)
//    Byte 224  erstes Bild
//
//  MIT Ton kommt eine zweite Spur dazu, und das Deckblatt wird 326 Bytes:
//
//    Byte 212  'LIST'  strl   ("Spur-Daten", Spur 1)
//    Byte 224  'strh'         (Angaben zur Tonspur)
//    Byte 288  'strf'         (Tonformat: PCM, 16 Bit, Mono)
//    Byte 314  'LIST'  movi
//    Byte 326  erstes Bild
//
//  Danach wechseln sich die Häppchen ab:
//    '00dc' Bild ... '01wb' Ton ... '00dc' Bild ... '01wb' Ton ...
//
//  Am Ende: 'idx1' — ein Inhaltsverzeichnis, damit man spulen kann.
//
//  Ein paar Zahlen im Deckblatt kennen wir erst am ENDE (z. B. wie viele
//  Bilder es geworden sind). Deshalb schreiben wir erst Platzhalter und
//  springen zum Schluss zurück und tragen die richtigen Werte ein.
// ---------------------------------------------------------------------------

#define KOPF_OHNE_TON  224
#define KOPF_MIT_TON   326

#define MAX_BILDER     900     // Sicherheitsgrenze für ein Clip
#define MAX_EINTRAEGE  2400    // Inhaltsverzeichnis: Bild- UND Tonhäppchen

static File      datei;
static bool      laeuft      = false;
static uint32_t  bildAnzahl  = 0;
static uint32_t  moviBytes   = 0;   // Größe aller Daten inkl. Häppchen-Köpfe
static uint32_t  maxBildByte = 0;
static uint8_t*  idxPuffer   = nullptr;   // 16 Bytes pro Häppchen
static uint32_t  idxAnzahl   = 0;

// --- Tonspur ---------------------------------------------------------------
static uint32_t  tonRateHz    = 0;   // 0 = dieser Clip hat keinen Ton
static uint32_t  tonProben    = 0;   // wie viele Messwerte insgesamt
static uint32_t  maxTonByte   = 0;
static uint16_t  kopfGroesse  = KOPF_OHNE_TON;
static uint16_t  offMoviGr    = 216; // wo die Größe des movi-Blocks steht

// --- Selbstdiagnose der SD-Karte ------------------------------------------
static uint32_t  verworfen     = 0;       // Häppchen, die nicht draufpassten
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
bool aviStart(const String& pfad, uint16_t breite, uint16_t hoehe,
              uint32_t tonRate) {
  if (laeuft) aviEnde(8.0f);

  datei = SD.open(pfad.c_str(), "w+");      // w+ = neu anlegen, lesen+schreiben
  if (!datei) {
    Serial.printf("[AVI] Kann %s nicht anlegen.\n", pfad.c_str());
    return false;
  }

  if (!idxPuffer) {
    idxPuffer = (uint8_t*)ps_malloc(MAX_EINTRAEGE * 16);
    if (!idxPuffer) {
      Serial.println("[AVI] Kein PSRAM fuer das Inhaltsverzeichnis.");
      datei.close();
      return false;
    }
  }

  tonRateHz   = tonRate;
  kopfGroesse = tonRate ? KOPF_MIT_TON : KOPF_OHNE_TON;
  offMoviGr   = tonRate ? 318 : 216;

  // Deckblatt mit Platzhaltern bauen
  uint8_t kopf[KOPF_MIT_TON];
  memset(kopf, 0, sizeof(kopf));

  setzeTag(kopf + 0,  "RIFF");
  setzeLE32(kopf + 4, 0);                  // Gesamtgröße -> später
  setzeTag(kopf + 8,  "AVI ");

  setzeTag(kopf + 12, "LIST");
  setzeLE32(kopf + 16, tonRate ? 294 : 192);   // Größe des hdrl-Blocks
  setzeTag(kopf + 20, "hdrl");

  setzeTag(kopf + 24, "avih");
  setzeLE32(kopf + 28, 56);
  setzeLE32(kopf + 32, 125000);            // Mikrosekunden pro Bild -> später
  setzeLE32(kopf + 36, 0);                 // max. Bytes pro Sekunde
  setzeLE32(kopf + 40, 0);
  setzeLE32(kopf + 44, 0x10);              // Flag: hat ein Inhaltsverzeichnis
  setzeLE32(kopf + 48, 0);                 // Anzahl Bilder -> später
  setzeLE32(kopf + 52, 0);
  setzeLE32(kopf + 56, tonRate ? 2 : 1);   // Anzahl der Spuren
  setzeLE32(kopf + 60, 0);
  setzeLE32(kopf + 64, breite);
  setzeLE32(kopf + 68, hoehe);

  // ---- Spur 0: das Bild ---------------------------------------------------
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

  if (tonRate) {
    // ---- Spur 1: der Ton --------------------------------------------------
    //  PCM heißt: die Messwerte stehen roh in der Datei, genau wie in einer
    //  WAV-Datei. Keine Kompression, dafür kein Rechenaufwand — und der
    //  ESP32 hat für Tonkompression sowieso keine Luft.
    setzeTag(kopf + 212, "LIST");
    setzeLE32(kopf + 216, 94);
    setzeTag(kopf + 220, "strl");

    setzeTag(kopf + 224, "strh");
    setzeLE32(kopf + 228, 56);
    setzeTag(kopf + 232, "auds");          // Spur-Art: Ton
    setzeLE32(kopf + 236, 1);              // Format 1 = PCM
    setzeLE32(kopf + 240, 0);              // Flags
    setzeLE16(kopf + 244, 0);              // Priorität
    setzeLE16(kopf + 246, 0);              // Sprache
    setzeLE32(kopf + 248, 0);              // Vorlaufbilder
    setzeLE32(kopf + 252, 2);              // Zeit-Teiler = Bytes pro Messwert
    setzeLE32(kopf + 256, tonRate * 2);    // Bytes pro Sekunde
    setzeLE32(kopf + 260, 0);              // Start
    setzeLE32(kopf + 264, 0);              // Länge in Messwerten -> später
    setzeLE32(kopf + 268, 0);              // Puffergröße -> später
    setzeLE32(kopf + 272, 0xFFFFFFFF);     // Qualität: unbekannt
    setzeLE32(kopf + 276, 2);              // Größe eines Messwerts in Bytes
    setzeLE32(kopf + 280, 0);              // Bildausschnitt (für Ton egal)
    setzeLE32(kopf + 284, 0);

    setzeTag(kopf + 288, "strf");
    setzeLE32(kopf + 292, 18);             // WAVEFORMATEX ist 18 Bytes lang
    setzeLE16(kopf + 296, 1);              // 1 = PCM, unkomprimiert
    setzeLE16(kopf + 298, 1);              // Mono
    setzeLE32(kopf + 300, tonRate);        // Messungen pro Sekunde
    setzeLE32(kopf + 304, tonRate * 2);    // Bytes pro Sekunde
    setzeLE16(kopf + 308, 2);              // Bytes pro Messwert-Block
    setzeLE16(kopf + 310, 16);             // Bits pro Messwert
    setzeLE16(kopf + 312, 0);              // keine Zusatzangaben

    setzeTag(kopf + 314, "LIST");
    setzeLE32(kopf + 318, 0);              // Größe des movi-Blocks -> später
    setzeTag(kopf + 322, "movi");
  } else {
    setzeTag(kopf + 212, "LIST");
    setzeLE32(kopf + 216, 0);              // Größe des movi-Blocks -> später
    setzeTag(kopf + 220, "movi");
  }

  if (datei.write(kopf, kopfGroesse) != kopfGroesse) {
    Serial.println("[AVI] Deckblatt konnte nicht geschrieben werden.");
    datei.close();
    return false;
  }

  bildAnzahl  = 0;
  tonProben   = 0;
  moviBytes   = 4;      // die 4 Bytes von 'movi' zählen mit
  maxBildByte = 0;
  maxTonByte  = 0;
  idxAnzahl   = 0;
  laeuft      = true;
  return true;
}

// ---------------------------------------------------------------------------
//  Ein Häppchen schreiben: 4 Zeichen Name, Länge, Daten, ggf. ein Füllbyte.
//  Genau so funktionieren Bild und Ton — deshalb steht es nur einmal hier.
// ---------------------------------------------------------------------------
static bool haeppchenSchreiben(const char* tag, const uint8_t* daten,
                               size_t laenge) {
  uint8_t chunkKopf[8];
  setzeTag(chunkKopf, tag);
  setzeLE32(chunkKopf + 4, laenge);

  uint32_t versatz = moviBytes;      // Position fürs Inhaltsverzeichnis
  uint32_t t0 = micros();

  if (datei.write(chunkKopf, 8) != 8)       { verworfen++; return false; }
  if (datei.write(daten, laenge) != laenge) { verworfen++; return false; }

  // AVI verlangt gerade Byte-Anzahlen -> ggf. ein Füllbyte
  size_t pad = laenge & 1;
  if (pad) { uint8_t null = 0; datei.write(&null, 1); }

  // Für die Selbstdiagnose: wie schnell schreibt die Karte wirklich?
  schreibZeitUs += (micros() - t0);
  schreibBytes  += 8 + laenge + pad;

  // Inhaltsverzeichnis-Eintrag: Name, "ist Vollbild", Position, Länge
  if (idxAnzahl < MAX_EINTRAEGE) {
    uint8_t* e = idxPuffer + idxAnzahl * 16;
    setzeTag(e, tag);
    setzeLE32(e + 4, 0x10);          // AVIIF_KEYFRAME — jedes JPEG ist ganz
    setzeLE32(e + 8, versatz);
    setzeLE32(e + 12, laenge);
    idxAnzahl++;
  }

  moviBytes += 8 + laenge + pad;
  return true;
}

// ---------------------------------------------------------------------------
bool aviFrame(const uint8_t* daten, size_t laenge) {
  if (!laeuft || bildAnzahl >= MAX_BILDER) { verworfen++; return false; }
  if (!haeppchenSchreiben("00dc", daten, laenge)) return false;

  bildAnzahl++;
  if (laenge > maxBildByte) maxBildByte = laenge;
  return true;
}

// ---------------------------------------------------------------------------
bool aviTon(const int16_t* proben, size_t anzahl) {
  if (!laeuft || !tonRateHz || !proben || anzahl == 0) return false;

  size_t laenge = anzahl * sizeof(int16_t);
  if (!haeppchenSchreiben("01wb", (const uint8_t*)proben, laenge)) return false;

  tonProben += anzahl;
  if (laenge > maxTonByte) maxTonByte = laenge;
  return true;
}

bool aviHatTon() { return tonRateHz != 0; }

// ---------------------------------------------------------------------------
void aviEnde(float bilderProSekunde) {
  if (!laeuft) return;

  // 1) Inhaltsverzeichnis anhängen
  uint8_t idxKopf[8];
  setzeTag(idxKopf, "idx1");
  setzeLE32(idxKopf + 4, idxAnzahl * 16);
  datei.write(idxKopf, 8);
  datei.write(idxPuffer, idxAnzahl * 16);

  uint32_t gesamt = kopfGroesse + (moviBytes - 4) + 8 + idxAnzahl * 16;

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

  if (tonRateHz) {
    // Die Tonspur wird in Messwerten gezählt, nicht in Bytes.
    datei.seek(264);  setzeLE32(z, tonProben);       datei.write(z, 4);
    datei.seek(268);  setzeLE32(z, maxTonByte);      datei.write(z, 4);
  }

  datei.seek(offMoviGr); setzeLE32(z, moviBytes);    datei.write(z, 4);

  datei.close();    // <- ganz wichtig: sofort schließen, nicht offen lassen!
  laeuft = false;

  // Der Ton verrät nebenbei, ob Bild und Ton gleich lang geworden sind:
  // stimmen die zwei Sekundenzahlen nicht überein, laufen sie auseinander.
  if (tonRateHz) {
    Serial.printf("[AVI] Clip fertig: %u Bilder / %.1f s Bild, %.1f s Ton, "
                  "%u KB (SD schreibt %.2f MB/s%s)\n",
                  bildAnzahl, bildAnzahl / bilderProSekunde,
                  tonProben / (float)tonRateHz, gesamt / 1024,
                  aviSchreibrateMBs(),
                  verworfen ? ", ACHTUNG Bilder verworfen!" : "");
  } else {
    Serial.printf("[AVI] Clip fertig: %u Bilder, %.1f fps, %u KB "
                  "(SD schreibt %.2f MB/s%s)\n",
                  bildAnzahl, bilderProSekunde, gesamt / 1024,
                  aviSchreibrateMBs(),
                  verworfen ? ", ACHTUNG Bilder verworfen!" : "");
  }
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
