// ============================================================================
//  web.h — Website und Livestream
// ============================================================================
//  Der ESP32 ist ein kleiner Webserver. Zwei Türen:
//
//    Tür 80 (die normale): die Website, die Statistik, die Dateien
//    Tür 81 (extra):       der Livestream
//
//  Warum zwei? Weil ein Livestream nie aufhört. Er würde die normale Tür
//  blockieren, und dann könnte man die Website nicht mehr aufrufen,
//  solange man zuschaut.
// ============================================================================

#pragma once
#include <Arduino.h>
#include <esp_camera.h>

// Einmal beim Start aufrufen (nachdem das WLAN steht).
void webStart();

// In jeder Runde der Hauptschleife aufrufen: bearbeitet Anfragen.
void webBedienen();

// Ein frisches Kamerabild an den Livestream weitergeben (falls jemand
// zuschaut). Kostet nichts, wenn niemand da ist.
void webFrameSenden(camera_fb_t* bild);

// Schaut gerade jemand zu? Wenn ja, macht die Kamera mehr Bilder pro Sekunde.
bool webHatZuschauer();
