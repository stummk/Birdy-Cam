// ============================================================================
//  avi.h — Videoclips als AVI-Datei schreiben, mit Bild UND Ton
// ============================================================================
//  Der ESP32 kann kein "echtes" Video (H.264) — dafür fehlt ihm die Kraft.
//  Er kann aber sehr schnell Einzelfotos machen. Ein AVI im Format "MJPEG"
//  ist genau das: viele JPEG-Fotos in einer Datei, mit einem Deckblatt vorne
//  dran, das sagt "spiel das mit 8 Bildern pro Sekunde ab".
//
//  Eine AVI-Datei darf mehrere SPUREN haben. Wir schreiben zwei:
//
//      Spur 0 "00dc" = die JPEG-Bilder      (MJPG)
//      Spur 1 "01wb" = der Ton, unkomprimiert (PCM, wie in einer WAV-Datei)
//
//  Sie werden abwechselnd in die Datei geschrieben — ein Bild, der Ton dazu,
//  das nächste Bild. Das nennt sich "interleaved", und daher kommt das I in
//  AVI (Audio Video Interleave).
//
//  Solche Dateien öffnet der VLC Player sofort, Windows meist auch.
// ============================================================================

#pragma once
#include <Arduino.h>
#include <FS.h>

// Neuen Clip anfangen. Gibt false zurück, wenn die Datei nicht aufgeht.
//   tonRate = Abtastrate der Tonspur in Hz, oder 0 für "Clip ohne Ton".
bool aviStart(const String& pfad, uint16_t breite, uint16_t hoehe,
              uint32_t tonRate = 0);

// Ein JPEG-Bild anhängen.
bool aviFrame(const uint8_t* daten, size_t laenge);

// Ein Stück Ton anhängen (16 Bit, Mono, so viele Proben wie angegeben).
// Ohne Tonspur tut das nichts.
bool aviTon(const int16_t* proben, size_t anzahl);

// Hat dieser Clip eine Tonspur?
bool aviHatTon();

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
