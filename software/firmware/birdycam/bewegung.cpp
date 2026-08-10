#include "bewegung.h"
#include "config.h"
#include <img_converters.h>

static uint8_t* rgbPuffer   = nullptr;  // verkleinertes Bild in Farbe
static uint8_t* grauJetzt   = nullptr;  // Helligkeit dieses Bildes
static uint8_t* grauVorher  = nullptr;  // Helligkeit des letzten Bildes
static uint16_t kBreite     = 0;        // Größe des verkleinerten Bildes
static uint16_t kHoehe      = 0;
static bool     habeVorher  = false;

bool bewegungStart() {
  // Die Puffer legen wir erst an, wenn wir das erste Bild sehen —
  // dann wissen wir, wie groß es ist.
  habeVorher = false;
  return true;
}

void bewegungZuruecksetzen() { habeVorher = false; }

// ---------------------------------------------------------------------------
static bool pufferAnlegen(uint16_t breite, uint16_t hoehe) {
  kBreite = breite / 8;      // wir verkleinern um Faktor 8
  kHoehe  = hoehe  / 8;
  size_t anzahl = (size_t)kBreite * kHoehe;

  rgbPuffer  = (uint8_t*)ps_malloc(anzahl * 2);   // RGB565 = 2 Bytes je Punkt
  grauJetzt  = (uint8_t*)ps_malloc(anzahl);
  grauVorher = (uint8_t*)ps_malloc(anzahl);

  if (!rgbPuffer || !grauJetzt || !grauVorher) {
    Serial.println("[Bewegung] Zu wenig Speicher!");
    return false;
  }
  Serial.printf("[Bewegung] Vergleichsbild: %u x %u Punkte\n", kBreite, kHoehe);
  return true;
}

// ---------------------------------------------------------------------------
BewegungErgebnis bewegungPruefen(camera_fb_t* bild) {
  BewegungErgebnis erg;
  if (!bild || bild->format != PIXFORMAT_JPEG) return erg;

  if (!rgbPuffer && !pufferAnlegen(bild->width, bild->height)) return erg;

  size_t anzahl = (size_t)kBreite * kHoehe;

  // Schritt 1+2: JPEG auspacken und dabei auf 1/8 verkleinern.
  // Das macht die Kamera-Bibliothek für uns und ist überraschend schnell.
  if (!jpg2rgb565(bild->buf, bild->len, rgbPuffer, JPG_SCALE_8)) {
    return erg;   // kaputtes JPEG — einfach überspringen
  }

  // Aus Farbe wird Helligkeit. Grün zählt am stärksten, weil unser Auge
  // (und der Sensor) für Grün am empfindlichsten ist.
  uint32_t summe = 0;
  for (size_t i = 0; i < anzahl; i++) {
    uint16_t px = rgbPuffer[i * 2] | (rgbPuffer[i * 2 + 1] << 8);
    uint8_t r = (px >> 11) & 0x1F;
    uint8_t g = (px >> 5)  & 0x3F;
    uint8_t b =  px        & 0x1F;
    // auf 0..255 hochrechnen und gewichtet mischen
    uint8_t grau = (uint8_t)(((r << 3) * 30 + (g << 2) * 59 + (b << 3) * 11) / 100);
    grauJetzt[i] = grau;
    summe += grau;
  }
  erg.helligkeit = (uint8_t)(summe / anzahl);

  // Schritt 3+4: mit dem letzten Bild vergleichen
  if (habeVorher) {
    uint16_t geaendert = 0;
    for (size_t i = 0; i < anzahl; i++) {
      int diff = (int)grauJetzt[i] - (int)grauVorher[i];
      if (diff < 0) diff = -diff;
      if (diff > BEWEGUNG_SCHWELLE) geaendert++;
    }
    erg.punkte   = geaendert;
    erg.bewegung = (geaendert >= BEWEGUNG_MIN_PUNKTE);
    erg.gueltig  = true;
  }

  // Dieses Bild wird zum Vergleichsbild fürs nächste Mal
  memcpy(grauVorher, grauJetzt, anzahl);
  habeVorher = true;

  return erg;
}
