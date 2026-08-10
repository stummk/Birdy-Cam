#include "systeminfo.h"
#include "config.h"
#include "speicher.h"
#include "strom.h"
#include "avi.h"
#include "netzwerk.h"

#include <WiFi.h>
#include <SD.h>
#include <esp_system.h>
#include <esp_timer.h>

static uint32_t kameraFehler   = 0;
static uint32_t minFreierHeap  = 0xFFFFFFFF;
static const char* neustartGrund = "unbekannt";

// ---------------------------------------------------------------------------
//  Neustartgrund im Klartext. Sehr nützlich zur Fehlersuche:
//  "Brownout" heißt Spannungseinbruch, "Watchdog" heißt das Programm hing.
// ---------------------------------------------------------------------------
static const char* grundText(esp_reset_reason_t r) {
  switch (r) {
    case ESP_RST_POWERON:  return "Einschalten";
    case ESP_RST_SW:       return "Software-Neustart";
    case ESP_RST_PANIC:    return "Absturz";
    case ESP_RST_INT_WDT:  return "Watchdog (Interrupt)";
    case ESP_RST_TASK_WDT: return "Watchdog (Aufgabe)";
    case ESP_RST_WDT:      return "Watchdog";
    case ESP_RST_DEEPSLEEP:return "Aufwachen aus Tiefschlaf";
    case ESP_RST_BROWNOUT: return "Spannungseinbruch";
    case ESP_RST_EXT:      return "Reset-Knopf";
    default:               return "unbekannt";
  }
}

// ---------------------------------------------------------------------------
//  Chip-Temperatur.
//  Der ESP32-S3 hat einen Temperatursensor eingebaut. Falls dein
//  Arduino-Kern die Funktion nicht kennt (sehr alte Version), liefert
//  dieser Wrapper 0 zurück und der Rest läuft trotzdem.
// ---------------------------------------------------------------------------
static float chipTemperatur() {
#if defined(CONFIG_IDF_TARGET_ESP32S3) || defined(SOC_TEMP_SENSOR_SUPPORTED)
  return temperatureRead();
#else
  return 0.0f;
#endif
}

void systemStart() {
  neustartGrund = grundText(esp_reset_reason());
  minFreierHeap = ESP.getFreeHeap();
  Serial.printf("[System] Letzter Start: %s\n", neustartGrund);
  Serial.printf("[System] Chip: %s, %d Kerne, %d MHz\n",
                ESP.getChipModel(), ESP.getChipCores(), ESP.getCpuFreqMHz());
  Serial.printf("[System] Heap: %u KB, PSRAM: %u KB\n",
                ESP.getFreeHeap() / 1024, ESP.getFreePsram() / 1024);
}

uint32_t systemLaufzeitS() {
  return (uint32_t)(esp_timer_get_time() / 1000000ULL);
}

void systemKameraFehlerPlus()        { kameraFehler++; }
uint32_t systemKameraFehler()        { return kameraFehler; }
uint32_t systemWlanNeuverbindungen() { return netzwerkNeuverbindungen(); }
const char* systemNeustartGrund()    { return neustartGrund; }

// ---------------------------------------------------------------------------
//  Speicher beobachten
// ---------------------------------------------------------------------------
//  Um das Netzwerk kümmert sich netzwerk.cpp — dort steckt die Logik für
//  Routerbetrieb, eigenes WLAN und Neuverbindung.
// ---------------------------------------------------------------------------
void systemPflegen() {
  uint32_t frei = ESP.getFreeHeap();
  if (frei < minFreierHeap) minFreierHeap = frei;

  // Der kleinste je gesehene Wert ist die wichtigste Zahl zur Beurteilung:
  // Sinkt er über Tage immer weiter, gibt es ein Speicherleck.
}

// ---------------------------------------------------------------------------
//  Bewertung
// ---------------------------------------------------------------------------
uint8_t systemGesundheit(String& warnungen) {
  uint8_t stufe = 0;
  warnungen = "[";
  bool erstes = true;

  auto melde = [&](uint8_t neueStufe, const char* text) {
    if (neueStufe > stufe) stufe = neueStufe;
    if (!erstes) warnungen += ",";
    erstes = false;
    warnungen += "\"";
    warnungen += text;
    warnungen += "\"";
  };

  // --- Akku ---
  float v = akkuVolt();
  if (AKKU_MESSEN && v > 0.5f) {
    if (v < AKKU_NOTAUS_VOLT + 0.10f)
      melde(2, "Akku fast leer — Kamera legt sich bald schlafen");
    else if (v < 3.60f)
      melde(1, "Akku niedrig — bei Dauerregen Powerbank anstecken");
  } else if (AKKU_MESSEN) {
    melde(1, "Kein Akku-Messwert — Spannungssensor angeschlossen?");
  }

  // --- Speicherkarte ---
  if (!speicherBereit())
    melde(2, "Keine SD-Karte — es werden keine Clips gespeichert");
  else if (aviVerworfen() > 0)
    melde(1, "SD-Karte zu langsam — BILD_QUALITAET erhoehen");

  // --- Arbeitsspeicher ---
  if (ESP.getFreeHeap() < 25000)
    melde(2, "Arbeitsspeicher fast voll — Neustart droht");
  else if (minFreierHeap < 35000)
    melde(1, "Arbeitsspeicher war schon knapp");

  // --- Netzwerk ---
  //  Achtung: Im eigenen WLAN (Access Point) ist WiFi.status() nicht
  //  WL_CONNECTED — das ist normal und kein Fehler. Deshalb fragen wir
  //  netzwerkBereit() und nicht direkt die WiFi-Bibliothek.
  if (!netzwerkBereit())
    melde(1, "Keine Verbindung — nimmt weiter auf, aber offline");
  else if (!netzwerkIstEigenes() && WiFi.RSSI() < -80)
    melde(1, "WLAN-Signal schwach — Stream wird ruckeln");

  // --- Uhrzeit ---
  if (!zeitIstGesetzt())
    melde(1, "Uhrzeit fehlt — Website einmal aufrufen, dann wird sie gestellt");

  // --- Temperatur ---
  float t = chipTemperatur();
  if (t > 80.0f)      melde(2, "Chip sehr heiss — Gehaeuse in die Sonne gestellt?");
  else if (t > 65.0f) melde(1, "Chip warm");

  // --- Zuverlässigkeit ---
  if (kameraFehler > 50)
    melde(1, "Viele Kamerafehler — Flachbandkabel pruefen");
  if (strcmp(neustartGrund, "Spannungseinbruch") == 0)
    melde(1, "Letzter Neustart durch Spannungseinbruch — Akku/Kabel pruefen");
  if (strcmp(neustartGrund, "Absturz") == 0)
    melde(1, "Letzter Neustart durch Absturz");

  if (erstes) {
    warnungen += "\"Alles in Ordnung\"";
  }
  warnungen += "]";
  return stufe;
}

// ---------------------------------------------------------------------------
String systemAlsJson() {
  String warnungen;
  uint8_t stufe = systemGesundheit(warnungen);

  String j;
  j += "\"gesundheit\":" + String(stufe);
  j += ",\"meldungen\":" + warnungen;
  j += ",\"laufzeitS\":" + String(systemLaufzeitS());
  j += ",\"neustartGrund\":\"" + String(neustartGrund) + "\"";

  // Speicher
  j += ",\"heapFreiKB\":"  + String(ESP.getFreeHeap() / 1024);
  j += ",\"heapMinKB\":"   + String(minFreierHeap / 1024);
  j += ",\"heapGesamtKB\":" + String(ESP.getHeapSize() / 1024);
  j += ",\"psramFreiKB\":" + String(ESP.getFreePsram() / 1024);
  j += ",\"psramGesamtKB\":" + String(ESP.getPsramSize() / 1024);

  // Netzwerk
  bool eigenes = netzwerkIstEigenes();
  j += ",\"wlan\":" + String(netzwerkBereit() ? "true" : "false");
  j += ",\"eigenesWlan\":" + String(eigenes ? "true" : "false");
  j += ",\"netzModus\":\"" + netzwerkModusText() + "\"";
  j += ",\"rssi\":" + String((!eigenes && WiFi.status() == WL_CONNECTED)
                             ? WiFi.RSSI() : 0);
  j += ",\"clients\":" + String(netzwerkClients());
  j += ",\"ip\":\"" + netzwerkAdresse() + "\"";
  j += ",\"wlanNeuverbindungen\":" + String(netzwerkNeuverbindungen());
  j += ",\"zeitGesetzt\":" + String(zeitIstGesetzt() ? "true" : "false");

  // Chip
  j += ",\"temperaturC\":" + String(chipTemperatur(), 1);
  j += ",\"chip\":\"" + String(ESP.getChipModel()) + "\"";
  j += ",\"takt\":" + String(ESP.getCpuFreqMHz());
  j += ",\"sdkVersion\":\"" + String(ESP.getSdkVersion()) + "\"";

  // Karte
  j += ",\"kartenGroesseMB\":" + String((uint32_t)(SD.cardSize() / (1024ULL * 1024ULL)));
  j += ",\"kartenBelegtMB\":"  + String((uint32_t)(SD.usedBytes() / (1024ULL * 1024ULL)));

  // Zuverlässigkeit
  j += ",\"kameraFehler\":" + String(kameraFehler);

  return j;
}
