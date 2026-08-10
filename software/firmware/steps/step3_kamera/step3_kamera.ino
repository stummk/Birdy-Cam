// ============================================================================
//  SCHRITT 3 — Das erste Bild!
// ============================================================================
//  Ziel: Kamera anschließen und im Browser sehen. Das ist der Moment, in dem
//        aus Bastelei plötzlich eine Kamera wird.
//
//  Vorher:
//     1. USB-Kabel ABZIEHEN
//     2. Kameramodul an die Flachbandbuchse (siehe Schaltplan 3.5)
//     3. USB-Kabel wieder anstecken
//     4. Unten WLAN-Name und Passwort eintragen
//
//  ⚠️ Werkzeuge -> PSRAM -> "OPI PSRAM"  — sonst startet die Kamera nicht!
//
//  Danach: die IP-Adresse aus dem Seriellen Monitor im Browser aufrufen.
// ============================================================================

#include <WiFi.h>
#include <esp_camera.h>

// <<< HIER EINTRAGEN >>>
const char* WLAN_NAME     = "HierDeinWLANName";
const char* WLAN_PASSWORT = "HierDeinPasswort";

// Pinbelegung der Kamera auf dem XIAO ESP32-S3 Sense (nicht ändern)
#define XCLK_GPIO_NUM  10
#define SIOD_GPIO_NUM  40
#define SIOC_GPIO_NUM  39
#define Y9_GPIO_NUM    48
#define Y8_GPIO_NUM    11
#define Y7_GPIO_NUM    12
#define Y6_GPIO_NUM    14
#define Y5_GPIO_NUM    16
#define Y4_GPIO_NUM    18
#define Y3_GPIO_NUM    17
#define Y2_GPIO_NUM    15
#define VSYNC_GPIO_NUM 38
#define HREF_GPIO_NUM  47
#define PCLK_GPIO_NUM  13

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n=== Schritt 3: Kamera ===\n");

  if (!psramFound()) {
    Serial.println("FEHLER: Kein PSRAM. Werkzeuge -> PSRAM -> OPI PSRAM!");
    return;
  }

  // ---- Kamera einrichten -------------------------------------------------
  camera_config_t c;
  c.ledc_channel = LEDC_CHANNEL_0;
  c.ledc_timer   = LEDC_TIMER_0;
  c.pin_d0 = Y2_GPIO_NUM;  c.pin_d1 = Y3_GPIO_NUM;
  c.pin_d2 = Y4_GPIO_NUM;  c.pin_d3 = Y5_GPIO_NUM;
  c.pin_d4 = Y6_GPIO_NUM;  c.pin_d5 = Y7_GPIO_NUM;
  c.pin_d6 = Y8_GPIO_NUM;  c.pin_d7 = Y9_GPIO_NUM;
  c.pin_xclk = XCLK_GPIO_NUM;    c.pin_pclk  = PCLK_GPIO_NUM;
  c.pin_vsync = VSYNC_GPIO_NUM;  c.pin_href  = HREF_GPIO_NUM;
  c.pin_sccb_sda = SIOD_GPIO_NUM; c.pin_sccb_scl = SIOC_GPIO_NUM;
  c.pin_pwdn = -1;               c.pin_reset = -1;
  c.xclk_freq_hz = 20000000;     // bei Streifen im Bild: auf 10000000 aendern
  c.pixel_format = PIXFORMAT_JPEG;
  c.frame_size   = FRAMESIZE_VGA;
  c.jpeg_quality = 12;
  c.fb_count     = 2;
  c.fb_location  = CAMERA_FB_IN_PSRAM;
  c.grab_mode    = CAMERA_GRAB_LATEST;

  esp_err_t fehler = esp_camera_init(&c);
  if (fehler != ESP_OK) {
    Serial.printf("FEHLER: Kamera startet nicht (Code 0x%x)\n", fehler);
    Serial.println("Pruefe:");
    Serial.println("  1. Flachbandkabel richtig drin und Buegel zu?");
    Serial.println("  2. Kontakte in die richtige Richtung?");
    Serial.println("  3. PSRAM auf OPI PSRAM?");
    return;
  }
  Serial.println("Kamera laeuft!");

  // ---- WLAN --------------------------------------------------------------
  WiFi.begin(WLAN_NAME, WLAN_PASSWORT);
  Serial.print("Verbinde mit WLAN");
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println();

  server.begin();

  Serial.println("\n========================================");
  Serial.printf("  Oeffne im Browser:  http://%s/\n", WiFi.localIP().toString().c_str());
  Serial.println("========================================\n");
}

void loop() {
  WiFiClient client = server.available();
  if (!client) { delay(10); return; }

  // Wir lesen die Anfrage des Browsers, interessieren uns aber nur dafür,
  // OB er was will — nicht was genau.
  while (client.connected() && client.available()) client.read();

  // Wir antworten mit einem endlosen Strom von JPEG-Bildern.
  // Das Format heisst MJPEG, und jeder Browser versteht es von sich aus.
  client.print("HTTP/1.1 200 OK\r\n"
               "Content-Type: multipart/x-mixed-replace; boundary=bild\r\n\r\n");

  Serial.println("Zuschauer da! Stream laeuft...");
  int bilder = 0;
  uint32_t start = millis();

  while (client.connected()) {
    camera_fb_t* fb = esp_camera_fb_get();
    if (!fb) break;

    client.printf("--bild\r\nContent-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n", fb->len);
    client.write(fb->buf, fb->len);
    client.print("\r\n");

    esp_camera_fb_return(fb);      // ganz wichtig: Bild zurueckgeben!
    bilder++;

    if (bilder % 30 == 0) {
      Serial.printf("  %d Bilder, %.1f pro Sekunde\n",
                    bilder, bilder * 1000.0 / (millis() - start));
    }
  }

  client.stop();
  Serial.println("Zuschauer weg.\n");
}
