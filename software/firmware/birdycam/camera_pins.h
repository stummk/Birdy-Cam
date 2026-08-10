// ============================================================================
//  Pinbelegung der Kamera am XIAO ESP32-S3 Sense
// ============================================================================
//  Diese Zahlen sind vom Board fest vorgegeben — sie stehen hier nur, damit
//  die Kamera-Bibliothek weiß, wo sie suchen muss.
//
//  ⚠️ Hier nichts ändern. Wirklich nichts.
// ============================================================================

#pragma once

#define PWDN_GPIO_NUM     -1    // hat dieses Board nicht
#define RESET_GPIO_NUM    -1    // hat dieses Board nicht
#define XCLK_GPIO_NUM     10
#define SIOD_GPIO_NUM     40    // I2C Daten (Kamera-Einstellungen)
#define SIOC_GPIO_NUM     39    // I2C Takt

#define Y9_GPIO_NUM       48    // die 8 Bilddatenleitungen
#define Y8_GPIO_NUM       11
#define Y7_GPIO_NUM       12
#define Y6_GPIO_NUM       14
#define Y5_GPIO_NUM       16
#define Y4_GPIO_NUM       18
#define Y3_GPIO_NUM       17
#define Y2_GPIO_NUM       15

#define VSYNC_GPIO_NUM    38    // "neues Bild fängt an"
#define HREF_GPIO_NUM     47    // "neue Zeile fängt an"
#define PCLK_GPIO_NUM     13    // Pixeltakt
