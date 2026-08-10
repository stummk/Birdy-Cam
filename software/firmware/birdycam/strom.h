// ============================================================================
//  strom.h — Akku, IR-Licht und Stromsparen
// ============================================================================
//  Drei Aufgaben:
//   1. Nachschauen, wie voll der Akku ist.
//   2. Die IR-LEDs an- und ausschalten und dimmen (PWM).
//   3. Die Kamera schlafen legen, wenn der Akku fast leer ist —
//      sonst nimmt der Akku dauerhaft Schaden.
// ============================================================================

#pragma once
#include <Arduino.h>

// Einmal beim Start aufrufen.
void stromStart();

// Akkuspannung in Volt. Gibt 0 zurück, wenn AKKU_MESSEN aus ist.
float akkuVolt();

// Ladezustand grob in Prozent (aus der Spannung geschätzt).
uint8_t akkuProzent();

// Wohin geht es? +1 = steigt (die Sonne lädt), -1 = fällt, 0 = stabil.
// Der ESP32 misst nur Spannung, keinen Strom — also erkennen wir das
// Laden am Trend über 10 Minuten statt an einem Amperemeter.
int akkuTrend();

// Regelmäßig aufrufen (z. B. alle 30 s), damit der Trend gepflegt wird.
void akkuTrendPflegen();

// IR-Licht ein/aus. helligkeit 0-255, 0 = aus.
void irLicht(uint8_t helligkeit);

// IR-Licht kurz vorglühen lassen, damit das Bild gleichmäßig belichtet ist.
void irBlitzAn();
void irBlitzAus();

// Ist das IR-Licht gerade an?
bool irIstAn();

// Prüft den Akku. Ist er zu leer, legt sich die Kamera schlafen
// (kommt aus dieser Funktion dann nicht mehr zurück).
void akkuSchutzPruefen();
