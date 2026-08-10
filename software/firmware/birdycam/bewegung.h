// ============================================================================
//  bewegung.h — Bewegungserkennung
// ============================================================================
//  Wie funktioniert das? Ganz einfach, und genau so, wie du es selbst machen
//  würdest, wenn du zwei "Finde-den-Fehler"-Bilder vergleichst:
//
//    1. Das große Bild wird auf ein winziges verkleinert (80 x 60 Punkte).
//    2. Farbe interessiert nicht, nur Helligkeit -> ein Graustufenbild.
//    3. Jeder Punkt wird mit dem gleichen Punkt vom letzten Bild verglichen.
//    4. Wenn genug Punkte anders geworden sind: da hat sich was bewegt!
//
//  Warum so klein? Weil 80 x 60 = 4800 Vergleiche in Millisekunden erledigt
//  sind. Bei 640 x 480 wären es 307.200 — das würde den Akku leersaugen.
// ============================================================================

#pragma once
#include <Arduino.h>
#include <esp_camera.h>

struct BewegungErgebnis {
  bool     bewegung   = false;  // hat sich was bewegt?
  uint16_t punkte     = 0;      // wie viele Punkte haben sich geändert
  uint8_t  helligkeit = 0;      // 0 = dunkel ... 255 = hell (für Tag/Nacht)
  bool     gueltig    = false;  // false beim allerersten Bild
};

// Einmal beim Start aufrufen.
bool bewegungStart();

// Für jedes Kamerabild aufrufen.
BewegungErgebnis bewegungPruefen(camera_fb_t* bild);

// Vergleichsbild verwerfen (z. B. nach dem Umschalten Tag/Nacht,
// sonst löst der Helligkeitssprung einen Fehlalarm aus).
void bewegungZuruecksetzen();
