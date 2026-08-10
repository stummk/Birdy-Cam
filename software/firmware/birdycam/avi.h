// ============================================================================
//  avi.h — Videoclips als AVI-Datei schreiben
// ============================================================================
//  Der ESP32 kann kein "echtes" Video (H.264) — dafür fehlt ihm die Kraft.
//  Er kann aber sehr schnell Einzelfotos machen. Ein AVI im Format "MJPEG"
//  ist genau das: viele JPEG-Fotos in einer Datei, mit einem Deckblatt vorne
//  dran, das sagt "spiel das mit 8 Bildern pro Sekunde ab".
//
//  Solche Dateien öffnet der VLC Player sofort, Windows meist auch.
// ============================================================================

#pragma once
#include <Arduino.h>
#include <FS.h>

// Neuen Clip anfangen. Gibt false zurück, wenn die Datei nicht aufgeht.
bool aviStart(const String& pfad, uint16_t breite, uint16_t hoehe);

// Ein JPEG-Bild anhängen.
bool aviFrame(const uint8_t* daten, size_t laenge);

// Clip abschließen: Deckblatt korrigieren und Datei schließen.
// bilderProSekunde: die tatsächlich erreichte Bildrate.
void aviEnde(float bilderProSekunde);

// Läuft gerade eine Aufnahme?
bool aviLaeuft();

// Wie viele Bilder sind schon drin?
uint32_t aviBildAnzahl();

// --- Selbstdiagnose: hält die SD-Karte mit? --------------------------------
// Bei Full HD ist die Schreibgeschwindigkeit der Karte der Engpass.
// Diese zwei Werte zeigen auf der Website, ob es reicht.

// Wie viele Bilder konnten NICHT geschrieben werden (Karte zu langsam/voll)?
uint32_t aviVerworfen();

// Tatsächlich erreichte Schreibrate in MB/s, gemessen über alle Clips.
float aviSchreibrateMBs();
