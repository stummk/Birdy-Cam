// ============================================================================
//  SCHRITT 3 — Das erste Bild!
// ============================================================================
//  Ziel: Kamera anschließen und im Browser sehen. Das ist der Moment, in dem
//        aus Bastelei plötzlich eine Kamera wird.
//
//  Vorher:
//     1. USB-Kabel ABZIEHEN
//     2. Kameramodul an die Flachbandbuchse (siehe Schaltplan 3.9)
//     3. ANTENNE aufstecken! Das kleine Plättchen mit dem dünnen Kabel aus
//        der XIAO-Packung gehört auf den winzigen u.FL-Stecker oben links
//        auf der Platine. Ohne Antenne ist die Reichweite erbärmlich, und
//        keine Einstellung in diesem Sketch kann das ausgleichen.
//        Aufsetzen: eine Seite einhaken, dann die andere Seite hineindrücken
//        — niemals flach draufdrücken, dann geht der Stecker kaputt.
//     4. USB-Kabel wieder anstecken
//     5. Unten WLAN-Name und Passwort eintragen
//
//  ⚠️ Werkzeuge -> PSRAM -> "OPI PSRAM"  — sonst startet die Kamera nicht!
//
//  Danach: die IP-Adresse aus dem Seriellen Monitor im Browser aufrufen.
// ============================================================================

#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_camera.h>

// <<< HIER EINTRAGEN >>>
const char* WLAN_NAME     = "CHANGEME";
const char* WLAN_PASSWORT = "CHANGEME";

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

// ---------------------------------------------------------------------------
//  Wenn es nicht klappt: zeigen, was ueberhaupt zu hoeren ist. Das
//  unterscheidet die drei Faelle, die sonst gleich aussehen — falscher Name,
//  falsches Passwort, zu schwaches Signal.
// ---------------------------------------------------------------------------
void umgebungZeigen() {
  Serial.println("\nIch hoere mich mal um...");
  int n = WiFi.scanNetworks();

  if (n <= 0) {
    Serial.println("KEIN EINZIGES WLAN zu hoeren!");
    Serial.println("Das ist fast immer die Antenne: Steckt sie auf dem");
    Serial.println("u.FL-Stecker oben links auf der Platine?");
    return;
  }

  bool gefunden = false;
  for (int i = 0; i < n; i++) {
    bool unser = (WiFi.SSID(i) == WLAN_NAME);
    if (unser) gefunden = true;
    Serial.printf("  %-24s %4d dBm  Kanal %2d%s\n",
                  WiFi.SSID(i).c_str(), WiFi.RSSI(i), WiFi.channel(i),
                  unser ? "   <-- das ist deiner" : "");
  }
  Serial.println("  (-30 ist super, -70 geht so, ab -80 wird es nichts)");

  if (gefunden) {
    Serial.println("\nDein WLAN ist da - dann stimmt das PASSWORT nicht.");
  } else {
    Serial.printf("\n\"%s\" war nicht dabei. Moegliche Gruende:\n", WLAN_NAME);
    Serial.println("  - Name falsch geschrieben (Gross-/Kleinschreibung!)");
    Serial.println("  - Router funkt nur auf 5 GHz - der ESP32 kann nur 2,4");
    Serial.println("  - zu weit weg");
  }
  WiFi.scanDelete();
}

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
  WiFi.disconnect(true);
  delay(150);
  WiFi.mode(WIFI_STA);
  delay(100);

  // Kanaele 12 und 13 freischalten. Ab Werk kennt der ESP32 nur 1-11 — ein
  // Router auf Kanal 12 oder 13 ist fuer ihn dann schlicht unsichtbar,
  // obwohl das Handy danebenliegend vier Balken zeigt. In Deutschland sind
  // 12 und 13 erlaubt und werden von Routern auch benutzt.
  esp_wifi_set_country_code("DE", true);

  // VOLLE Sendeleistung — 19.5 dBm ist das Maximum des Chips.
  // (Hier stand einmal WIFI_POWER_15dBm. Das ist eine Drosselung und kostet
  //  sofort Reichweite. Fuer einen Nistkasten im Garten: nicht drosseln.)
  WiFi.setTxPower(WIFI_POWER_19_5dBm);

  // Nicht schlafen legen. Spart sonst ein bisschen Strom, macht den Stream
  // aber ruckelig und die Verbindung bei schwachem Empfang wacklig.
  WiFi.setSleep(false);

  // Auch aeltere Router mitnehmen: Ab Werk verlangt der ESP32 mindestens
  // WPA2 und ignoriert einen WPA/TKIP-Router einfach.
  WiFi.setMinSecurity(WIFI_AUTH_WPA_PSK);

  WiFi.begin(WLAN_NAME, WLAN_PASSWORT);

  Serial.printf("Verbinde mit \"%s\"", WLAN_NAME);
  uint32_t start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 20000) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  // Nicht ewig Punkte malen: Nach 20 Sekunden sagen wir, WAS los ist.
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Keine Verbindung.");
    umgebungZeigen();
    Serial.println("\nNeustart in 10 Sekunden.");
    delay(10000);
    ESP.restart();
  }

  int32_t rssi = WiFi.RSSI();
  Serial.printf("Verbunden! Empfang: %d dBm\n", rssi);
  if (rssi < -80) {
    Serial.println("ACHTUNG: sehr schwach. Der Stream wird abbrechen.");
    Serial.println("  1. Antenne aufgesteckt?");
    Serial.println("  2. Zum Ausprobieren naeher an den Router.");
  } else if (rssi < -70) {
    Serial.println("Grenzwertig - fuer ein Standbild reicht es, fuer");
    Serial.println("fluessigen Stream oft nicht.");
  }

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
