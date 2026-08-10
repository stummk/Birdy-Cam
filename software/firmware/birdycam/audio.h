// ============================================================================
//  audio.h — Vogelgesang hören und aufnehmen
// ============================================================================
//  Das Mikrofon läuft die ganze Zeit mit und horcht. Es merkt sich immer die
//  letzten 2 Sekunden in einem "Gedächtnis". Wird es plötzlich laut, dann
//  speichert es die 2 Sekunden VON VORHER mit ab — sonst würde der Anfang
//  vom Vogelruf immer fehlen.
//
//  ⚠️ Was das NICHT kann: sagen, WELCHER Vogel singt. Dafür bräuchte man
//  BirdNET, und das braucht einen Raspberry Pi. Siehe Machbarkeit 1.4.
//
//  Damit die Kamera nicht ins Stottern kommt, läuft das Zuhören auf dem
//  zweiten Prozessorkern — der ESP32-S3 hat zwei. Einer für Bild, einer
//  für Ton.
// ============================================================================

#pragma once
#include <Arduino.h>

// Einmal beim Start aufrufen. Startet Mikrofon und Hintergrund-Aufgabe.
bool audioStart();

// true, wenn eine Aufnahme fertig im Speicher liegt und weggeschrieben
// werden möchte. Wird in der Hauptschleife abgefragt.
bool audioFertig();

// Die fertige Aufnahme als WAV-Datei speichern und Platz wieder freigeben.
bool audioSpeichern(const String& pfad);

// Aktueller Lautstärkewert — praktisch, um die Schwelle einzustellen.
uint16_t audioLautstaerke();

// Läuft gerade eine Aufnahme?
bool audioNimmtAuf();
