#include "netzwerk.h"
#include "config.h"

#include <WiFi.h>
#include <ESPmDNS.h>
#include <DNSServer.h>
#include <time.h>
#include <sys/time.h>

static bool      eigenesWlanAn   = false;
static bool      routerVerbunden = false;
static uint32_t  neuverbindungen = 0;
static uint32_t  letztePruefung  = 0;
static bool      zeitGesetzt     = false;

// Der DNS-Server sorgt dafür, dass im eigenen WLAN JEDE eingegebene Adresse
// auf der Kamera landet ("Captive Portal"). Viele Handys öffnen die Seite
// dann sogar von selbst.
static DNSServer dnsServer;
static bool      dnsLaeuft = false;

static const IPAddress AP_IP(192, 168, 4, 1);
static const IPAddress AP_MASKE(255, 255, 255, 0);

// ---------------------------------------------------------------------------
//  Router-Betrieb
// ---------------------------------------------------------------------------
static bool routerVerbinden(uint16_t timeoutS) {
  Serial.printf("[Netz] Verbinde mit Router \"%s\"", WLAN_NAME);

  WiFi.mode(WIFI_STA);
  // Modem-Sleep: Das WLAN darf zwischen zwei Funkkontakten dösen.
  // Spart deutlich Strom und ist der Grund, warum Routerbetrieb
  // sparsamer ist als ein eigenes WLAN.
  WiFi.setSleep(true);
  WiFi.setHostname(GERAETE_NAME);
  WiFi.begin(WLAN_NAME, WLAN_PASSWORT);

  uint32_t start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < timeoutS * 1000UL) {
    delay(400);
    Serial.print(".");
  }
  Serial.println();

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[Netz] Router nicht erreichbar.");
    return false;
  }

  routerVerbunden = true;
  eigenesWlanAn = false;
  Serial.printf("[Netz] Verbunden. IP: %s  (%d dBm)\n",
                WiFi.localIP().toString().c_str(), WiFi.RSSI());
  return true;
}

// ---------------------------------------------------------------------------
//  Eigenes WLAN
// ---------------------------------------------------------------------------
static bool eigenesWlanStarten() {
  Serial.printf("[Netz] Mache eigenes WLAN auf: \"%s\"\n", AP_NAME);

  const char* pw = AP_PASSWORT;
  bool offen = (pw == nullptr) || (strlen(pw) == 0);
  if (!offen && strlen(pw) < 8) {
    Serial.println("[Netz] WARNUNG: AP_PASSWORT ist kuerzer als 8 Zeichen!");
    Serial.println("[Netz] WLAN wird OHNE Passwort geoeffnet. Bitte in");
    Serial.println("[Netz] config.h ein laengeres Passwort eintragen.");
    offen = true;
  }

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(AP_IP, AP_IP, AP_MASKE);

  bool ok = WiFi.softAP(AP_NAME, offen ? nullptr : pw, AP_KANAL,
                        false /* nicht versteckt */, 4 /* max. Geraete */);
  if (!ok) {
    Serial.println("[Netz] Eigenes WLAN konnte nicht gestartet werden.");
    return false;
  }

  eigenesWlanAn = true;
  routerVerbunden = false;

  // Captive Portal: alle DNS-Anfragen auf uns umleiten
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsLaeuft = dnsServer.start(53, "*", AP_IP);

  Serial.println("[Netz] ========================================");
  Serial.printf ("[Netz]  WLAN-Name:  %s\n", AP_NAME);
  Serial.printf ("[Netz]  Passwort:   %s\n", offen ? "(offen)" : pw);
  Serial.printf ("[Netz]  Dann im Browser:  http://%s/\n",
                 AP_IP.toString().c_str());
  Serial.println("[Netz] ========================================");
  if (!dnsLaeuft) Serial.println("[Netz] (DNS-Umleitung nicht aktiv)");
  return true;
}

static void eigenesWlanStoppen() {
  if (!eigenesWlanAn) return;
  if (dnsLaeuft) { dnsServer.stop(); dnsLaeuft = false; }
  WiFi.softAPdisconnect(true);
  eigenesWlanAn = false;
  Serial.println("[Netz] Eigenes WLAN abgeschaltet (Nachtruhe).");
}

// ---------------------------------------------------------------------------
//  Uhrzeit
// ---------------------------------------------------------------------------
static void zeitVomServerHolen() {
  configTzTime(ZEITZONE, NTP_SERVER);
  Serial.print("[Zeit] Hole Uhrzeit vom Zeitserver");
  for (int i = 0; i < 20 && time(nullptr) < 1000000000; i++) {
    delay(300);
    Serial.print(".");
  }
  Serial.println();

  time_t jetzt = time(nullptr);
  if (jetzt > 1000000000) {
    zeitGesetzt = true;
    Serial.printf("[Zeit] %s", ctime(&jetzt));
  } else {
    Serial.println("[Zeit] Keine Uhrzeit bekommen.");
  }
}

bool zeitIstGesetzt() { return zeitGesetzt; }

bool zeitSetzen(uint32_t unixZeit) {
  // Plausibilitätsprüfung: irgendwann zwischen 2024 und 2050.
  // Ohne das könnte ein kaputter Browser die Statistik ruinieren.
  if (unixZeit < 1700000000UL || unixZeit > 2500000000UL) {
    Serial.printf("[Zeit] Unplausibler Wert abgelehnt: %u\n", unixZeit);
    return false;
  }

  struct timeval tv = { .tv_sec = (time_t)unixZeit, .tv_usec = 0 };
  settimeofday(&tv, nullptr);
  setenv("TZ", ZEITZONE, 1);
  tzset();

  zeitGesetzt = true;
  time_t jetzt = time(nullptr);
  Serial.printf("[Zeit] Vom Browser gestellt: %s", ctime(&jetzt));
  return true;
}

// ---------------------------------------------------------------------------
//  Start
// ---------------------------------------------------------------------------
void netzwerkStart() {
  bool ok = false;

  if (NETZ_MODUS == NETZ_ROUTER) {
    ok = routerVerbinden(ROUTER_TIMEOUT_S);
    if (!ok) {
      Serial.println("[Netz] Modus ROUTER: keine Verbindung.");
      Serial.println("[Netz] Die Kamera nimmt trotzdem auf — nur ohne Website.");
      Serial.println("[Netz] Tipp: NETZ_MODUS auf NETZ_AUTO stellen, dann");
      Serial.println("[Netz] macht sie in diesem Fall ihr eigenes WLAN auf.");
    }

  } else if (NETZ_MODUS == NETZ_EIGENES) {
    ok = eigenesWlanStarten();

  } else {   // NETZ_AUTO
    ok = routerVerbinden(ROUTER_TIMEOUT_S);
    if (!ok) {
      Serial.println("[Netz] Kein Router -> schalte auf eigenes WLAN um.");
      ok = eigenesWlanStarten();
    }
  }

  if (!ok) return;

  // Name im Netzwerk (funktioniert in beiden Betriebsarten)
  if (MDNS.begin(GERAETE_NAME)) {
    MDNS.addService("http", "tcp", 80);
    Serial.printf("[Netz] Auch erreichbar als http://%s.local/\n", GERAETE_NAME);
  }

  // Uhrzeit: aus dem Internet nur im Routerbetrieb möglich
  if (routerVerbunden) {
    zeitVomServerHolen();
  } else {
    Serial.println("[Zeit] Eigenes WLAN = kein Internet = keine Uhrzeit.");
    Serial.println("[Zeit] Der erste Besucher der Website stellt sie.");
  }
}

// ---------------------------------------------------------------------------
//  Pflege
// ---------------------------------------------------------------------------
void netzwerkPflegen(bool istNacht) {
  // Captive-Portal-DNS muss oft bedient werden, nicht nur alle 15 s
  if (dnsLaeuft) dnsServer.processNextRequest();

  if (millis() - letztePruefung < 15000) return;
  letztePruefung = millis();

  // --- Eigenes WLAN: nachts abschalten? ---
  if (eigenesWlanAn && AP_NACHTS_AUS) {
    if (istNacht && WiFi.softAPgetStationNum() == 0) {
      eigenesWlanStoppen();
      return;
    }
  }
  // ... und morgens wieder an
  if (!eigenesWlanAn && !routerVerbunden && AP_NACHTS_AUS && !istNacht) {
    eigenesWlanStarten();
    return;
  }

  // --- Routerbetrieb: Verbindung überwachen ---
  //  Ohne das wäre die Kamera nach dem ersten Router-Neustart dauerhaft
  //  offline: Sie würde weiter aufnehmen, aber niemand käme mehr dran.
  if (!eigenesWlanAn && NETZ_MODUS != NETZ_EIGENES) {
    if (WiFi.status() != WL_CONNECTED) {
      routerVerbunden = false;
      neuverbindungen++;
      Serial.printf("[Netz] Router weg (%u. Mal) — verbinde neu…\n",
                    neuverbindungen);
      WiFi.disconnect();
      WiFi.begin(WLAN_NAME, WLAN_PASSWORT);
      // Absichtlich NICHT hier warten: Die Kamera soll weiter aufnehmen.
      // Beim nächsten Durchlauf schauen wir wieder nach.

      // Im AUTO-Modus nach 10 vergeblichen Versuchen (~2,5 Minuten)
      // aufgeben und eigenes WLAN aufmachen.
      if (NETZ_MODUS == NETZ_AUTO && neuverbindungen % 10 == 0) {
        Serial.println("[Netz] Router bleibt weg -> eigenes WLAN.");
        eigenesWlanStarten();
      }
    } else if (!routerVerbunden) {
      routerVerbunden = true;
      Serial.printf("[Netz] Router wieder da. IP: %s\n",
                    WiFi.localIP().toString().c_str());
      if (!zeitGesetzt) zeitVomServerHolen();
    }
  }
}

// ---------------------------------------------------------------------------
//  Auskünfte
// ---------------------------------------------------------------------------
bool netzwerkIstEigenes() { return eigenesWlanAn; }

bool netzwerkBereit() {
  return eigenesWlanAn || (WiFi.status() == WL_CONNECTED);
}

String netzwerkAdresse() {
  if (eigenesWlanAn) return WiFi.softAPIP().toString();
  if (WiFi.status() == WL_CONNECTED) return WiFi.localIP().toString();
  return String("-");
}

String netzwerkModusText() {
  if (eigenesWlanAn) {
    int n = WiFi.softAPgetStationNum();
    return String("eigenes WLAN (") + n + (n == 1 ? " Gerät)" : " Geräte)");
  }
  if (WiFi.status() == WL_CONNECTED)
    return String("Router (") + WiFi.RSSI() + " dBm)";
  return String("keine Verbindung");
}

int netzwerkClients() {
  return eigenesWlanAn ? WiFi.softAPgetStationNum() : 0;
}

uint32_t netzwerkNeuverbindungen() { return neuverbindungen; }
