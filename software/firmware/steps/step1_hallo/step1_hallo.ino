// ============================================================================
//  SCHRITT 1 — Hallo Welt
// ============================================================================
//  Ziel: Beweisen, dass der Computer mit dem Board reden kann.
//        Noch keine Kamera, noch keine SD-Karte. Nur: lebt es?
//
//  Board:      XIAO_ESP32S3
//  Hochladen:  Pfeil-Symbol oben links
//  Danach:     Werkzeuge -> Serieller Monitor, Geschwindigkeit 115200
//
//  Was du sehen solltest:
//     - die kleine LED auf dem Board blinkt
//     - im Seriellen Monitor erscheint jede Sekunde eine neue Zeile
// ============================================================================

void setup() {
  Serial.begin(115200);
  delay(1000);                    // dem USB-Anschluss Zeit geben

  pinMode(LED_BUILTIN, OUTPUT);

  Serial.println();
  Serial.println("========================================");
  Serial.println("  Hallo! Ich bin dein ESP32-S3.");
  Serial.println("========================================");

  // Der ESP32 kann uns erzählen, was in ihm steckt:
  Serial.printf("Prozessorkerne:   %d\n", ESP.getChipCores());
  Serial.printf("Taktfrequenz:     %d MHz\n", ESP.getCpuFreqMHz());
  Serial.printf("Arbeitsspeicher:  %d KB frei\n", ESP.getFreeHeap() / 1024);

  if (psramFound()) {
    Serial.printf("PSRAM (extra):    %d KB  -> super, das brauchen wir!\n",
                  ESP.getFreePsram() / 1024);
  } else {
    Serial.println("PSRAM:            FEHLT!");
    Serial.println(">>> Werkzeuge -> PSRAM -> \"OPI PSRAM\" einstellen");
    Serial.println(">>> und nochmal hochladen. Ohne PSRAM geht die Kamera nicht.");
  }
  Serial.println();
}

int zaehler = 0;

void loop() {
  zaehler++;

  // Achtung, kleine Falle: Auf dem XIAO leuchtet die LED bei LOW,
  // nicht bei HIGH. Sie ist "verkehrt herum" angeschlossen.
  digitalWrite(LED_BUILTIN, LOW);     // an
  delay(120);
  digitalWrite(LED_BUILTIN, HIGH);    // aus
  delay(880);

  Serial.printf("Ich lebe! Sekunde %d\n", zaehler);
}
