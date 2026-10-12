// ============================================================================
//                    🐦  B I R D Y C A M  🐦
//        Nistkasten-Kamera mit Nachtsicht, Livestream und Website
// ============================================================================
//
//  Board in der Arduino IDE:  "ESP32S3 Dev Module"  (nicht "XIAO_ESP32S3" -
//                             damit wird das Board nicht erkannt)
//  Wichtig unter Werkzeuge:   PSRAM = "OPI PSRAM"  <- sonst geht die Kamera nicht!
//                             Flash Size = "8MB (64Mb)", Partition Scheme =
//                             "8M with spiffs (3MB APP/1.5MB SPIFFS)"
//
//  Alle Einstellungen stehen in config.h — dort trägst du dein WLAN ein.
//
//  So läuft das Programm ab:
//
//     setup()  = einmal beim Einschalten: alles einschalten und prüfen
//     loop()   = immer wieder, viele Male pro Sekunde:
//                  1. Bild holen
//                  2. mit dem letzten Bild vergleichen
//                  3. wenn Bewegung: Clip aufnehmen + Foto speichern
//                  4. wenn jemand zuschaut: Bild in den Livestream schicken
//                  5. nachschauen, ob der Akku noch mag
//
// ============================================================================

#include "config.h"
#include "camera_pins.h"
#include "speicher.h"
#include "bewegung.h"
#include "avi.h"
#include "audio.h"
#include "strom.h"
#include "web.h"
#include "lichtschranke.h"
#include "systeminfo.h"
#include "netzwerk.h"

#include <esp_camera.h>
#include <time.h>

#if CONFIG_BT_ENABLED
#include <esp_bt.h>
#endif

// ---- Diese Werte liest auch die Website (siehe web.cpp) -------------------
bool    istNacht          = false;
float   aktuelleFps       = 0.0f;
// Mittlere Bildhelligkeit 0-255, frisch aus der Bewegungserkennung. Steht auf
// der Website unter "Technische Details" — mit dieser Zahl stellt man
// IR_HELLIGKEIT ein, statt nach Gefühl zu raten (siehe config.h).
uint8_t aktuelleHelligkeit = 0;

// ---- Vorlauf-Gedächtnis: die letzten Bilder VOR der Bewegung --------------
//  Größe und Anzahl stehen in config.h, weil sie zur Auflösung passen müssen.
struct VorlaufBild { uint8_t* daten = nullptr; size_t laenge = 0; };
static VorlaufBild vorlaufRing[VORLAUF_MAX_BILDER];
static uint8_t     vorlaufSchreib = 0;
static uint8_t     vorlaufAnzahl  = 0;

// ---- Ton für die Clips ----------------------------------------------------
//  Ein Zwischeneimer, groß genug für den ganzen Ton-Vorlauf. Damit wird
//  der Ton aus dem Ring in audio.cpp abgeholt und in die AVI-Datei gelegt.
#define TON_VORLAUF_PROBEN  ((uint32_t)TON_ABTASTRATE * TON_VORLAUF_MS / 1000)
#define TON_BLOCK_PROBEN    4096u           // ~256 ms bei 16 kHz
// Der Eimer muss für beides reichen: den ganzen Vorlauf und einen Block.
#define TON_PUFFER_PROBEN   (TON_VORLAUF_PROBEN > TON_BLOCK_PROBEN \
                             ? TON_VORLAUF_PROBEN : TON_BLOCK_PROBEN)
static int16_t* tonPuffer = nullptr;
static bool     clipHatTon = false;
static uint32_t clipVorlaufMs = 0;   // Dauer des Vorlaufs im laufenden Clip

// ---- Zustand der Aufnahme -------------------------------------------------
static bool     nehmeAuf         = false;
static uint32_t aufnahmeStart    = 0;
static uint32_t letzteBewegung   = 0;
static uint32_t letzterClipEnde  = 0;
static uint32_t clipBilder       = 0;

// ---- Zeitmessung ---------------------------------------------------------
static uint32_t fpsZaehler   = 0;
static uint32_t fpsFenster   = 0;
static uint32_t letzterAkkuCheck = 0;
static uint32_t letzteLichtprobe = 0;

// Zuletzt gemessene Akkuspannung. Gepuffert, weil akkuVolt() acht Messungen
// macht — das will man nicht in jeder Runde der Hauptschleife.
static float    akkuJetzt    = 0.0f;

// ---------------------------------------------------------------------------
//  Darf das IR-Licht gerade durchgehend brennen?
//
//  Dauerlicht gibt das deutlich bessere Nachtbild (siehe config.h), kostet
//  aber rund 4 Wh am Tag. Damit das keine Regenwoche killt, fällt die Kamera
//  unter IR_DAUERLICHT_AB_V von allein auf Blitzbetrieb zurück und spart
//  damit ~90 % davon. Steigt der Akku wieder, kommt das Dauerlicht zurück.
// ---------------------------------------------------------------------------
static bool irDauerlichtJetzt() {
  if (!IR_DAUERLICHT) return false;
  if (!AKKU_MESSEN || akkuJetzt < 0.5f) return true;   // kein Messwert, kein Sparen

  // Etwas Abstand zwischen Aus- und Einschaltpunkt ("Hysterese"), sonst
  // flackert es rund um die Schwelle im 30-Sekunden-Takt hin und her.
  static bool an = true;
  if      ( an && akkuJetzt <  IR_DAUERLICHT_AB_V)          an = false;
  else if (!an && akkuJetzt >  IR_DAUERLICHT_AB_V + 0.05f)  an = true;
  return an;
}

// ============================================================================
//  KAMERA STARTEN
// ============================================================================
// ---------------------------------------------------------------------------
//  Tag- oder Nachtprofil in den Sensor schreiben.
//
//  IR-Licht ist etwas völlig anderes als Tageslicht, und der Sensor muss das
//  wissen. Zwei Dinge sind dabei entscheidend:
//
//   1. GRAUSTUFEN. Unter reinem Infrarot gibt es keine echten Farben mehr.
//      Der Weißabgleich versucht trotzdem, welche herzustellen — daher der
//      Grünstich und das grüne Farbrauschen. Mit Graustufen und abgeschaltetem
//      Weißabgleich ist beides schlagartig weg.
//
//   2. VERSTÄRKUNG DECKELN. Verstärkung ist das Rauschen. Sie macht aus wenig
//      Licht ein helles, aber griesiges Bild. Die Obergrenze gehört deshalb
//      nachts nach UNTEN, nicht nach oben — und das fehlende Licht kommt
//      besser aus IR_HELLIGKEIT.
// ---------------------------------------------------------------------------
static void kameraProfil(bool nacht, bool melden) {
  sensor_t* s = esp_camera_sensor_get();
  if (!s) return;

  if (!nacht) {
    s->set_special_effect(s, 0);              // Farbe
    s->set_whitebal   (s, 1);                 // Weißabgleich an
    s->set_awb_gain   (s, 1);
    s->set_gainceiling(s, GAINCEILING_16X);
    s->set_exposure_ctrl(s, 1);               // Belichtungsautomatik an
    s->set_gain_ctrl  (s, 1);                 // Verstärkungsautomatik an
    s->set_ae_level   (s, 0);
    s->set_brightness (s, 1);                 // im Kasten ist es dunkel
    s->set_contrast   (s, 0);
    if (melden) Serial.println("[Kamera] Tagprofil: Farbe, Automatik an.");
    return;
  }

  s->set_special_effect(s, NACHT_GRAUSTUFEN ? 2 : 0);   // 2 = Graustufen
  s->set_whitebal   (s, NACHT_GRAUSTUFEN ? 0 : 1);
  s->set_awb_gain   (s, NACHT_GRAUSTUFEN ? 0 : 1);
  s->set_gainceiling(s, NACHT_GAINCEILING);
  s->set_brightness (s, 1);
  s->set_contrast   (s, 1);                   // der IR-Kontrast ist flau

  if (IR_HINTER_DEM_FENSTER) {
    // Die LEDs leuchten durch dasselbe Fenster wie die Linse. Der Schleier
    // vom Deckel ist heller als das, was vom Nest zurückkommt — die Automatik
    // würde dagegen abblenden und den Nistraum schwarz machen. Also feste
    // Werte: Der Schleier bleibt, verdunkelt aber nichts mehr. Siehe config.h.
    s->set_exposure_ctrl(s, 0);
    s->set_aec_value  (s, NACHT_BELICHTUNG);
    s->set_gain_ctrl  (s, 0);
    s->set_agc_gain   (s, NACHT_VERSTAERKUNG);
    if (melden)
      Serial.printf("[Kamera] Nachtprofil: %s, FESTE Belichtung %d / "
                    "Verstaerkung %d (LEDs hinter dem Fenster).\n",
                    NACHT_GRAUSTUFEN ? "Graustufen" : "Farbe",
                    (int)NACHT_BELICHTUNG, (int)NACHT_VERSTAERKUNG);
  } else {
    s->set_exposure_ctrl(s, 1);
    s->set_gain_ctrl  (s, 1);
    s->set_ae_level   (s, 1);                 // eine Stufe heller belichten
    if (melden)
      Serial.printf("[Kamera] Nachtprofil: %s, Automatik an.\n",
                    NACHT_GRAUSTUFEN ? "Graustufen" : "Farbe");
  }
}

// ---------------------------------------------------------------------------
//  Bilder wegwerfen, bis die Kamera das neue Licht wirklich gesehen hat.
//
//  Die Kamera nimmt durchgehend auf und legt fertige Bilder in zwei Puffer.
//  Ein frisch abgeholtes Bild ist deshalb ein paar Hundertstel ALT — es
//  entstand, bevor die IR-LEDs angingen. Ohne dieses Leerlaufen beleuchtet
//  der Blitz nur Bilder, die nie jemand zu sehen bekommt, und man wundert
//  sich über ein schwarzes Nachtbild bei leuchtenden LEDs.
// ---------------------------------------------------------------------------
static void kameraLeerlaufen(uint8_t bilder) {
  for (uint8_t i = 0; i < bilder; i++) {
    camera_fb_t* weg = esp_camera_fb_get();
    if (!weg) return;
    esp_camera_fb_return(weg);
  }
}

static bool kameraStarten() {
  // ⚠️ Erst nullen, dann füllen. In camera_config_t stecken Felder, die wir
  //    unten nicht anfassen (sccb_i2c_port, conv_mode) — uninitialisiert
  //    stünde dort Stack-Müll, und der Treiber würde ihn glauben.
  camera_config_t c = {};
  c.sccb_i2c_port = -1;              // -1 = der Treiber macht seinen eigenen
  c.ledc_channel = LEDC_CHANNEL_1;   // Kanal 0 gehört den IR-LEDs
  c.ledc_timer   = LEDC_TIMER_1;
  c.pin_d0 = Y2_GPIO_NUM;  c.pin_d1 = Y3_GPIO_NUM;
  c.pin_d2 = Y4_GPIO_NUM;  c.pin_d3 = Y5_GPIO_NUM;
  c.pin_d4 = Y6_GPIO_NUM;  c.pin_d5 = Y7_GPIO_NUM;
  c.pin_d6 = Y8_GPIO_NUM;  c.pin_d7 = Y9_GPIO_NUM;
  c.pin_xclk  = XCLK_GPIO_NUM;   c.pin_pclk  = PCLK_GPIO_NUM;
  c.pin_vsync = VSYNC_GPIO_NUM;  c.pin_href  = HREF_GPIO_NUM;
  c.pin_sccb_sda = SIOD_GPIO_NUM; c.pin_sccb_scl = SIOC_GPIO_NUM;
  c.pin_pwdn  = PWDN_GPIO_NUM;   c.pin_reset = RESET_GPIO_NUM;
  c.xclk_freq_hz = XCLK_MHZ * 1000000;
  c.pixel_format = PIXFORMAT_JPEG;
  c.frame_size   = BILD_GROESSE;
  c.jpeg_quality = BILD_QUALITAET;
  c.fb_count     = 2;                        // zwei Bildpuffer = flüssiger
  c.fb_location  = CAMERA_FB_IN_PSRAM;
  c.grab_mode    = CAMERA_GRAB_LATEST;       // immer das neueste Bild nehmen

  esp_err_t f = esp_camera_init(&c);
  if (f != ESP_OK) {
    Serial.printf("[Kamera] Start fehlgeschlagen, Fehler 0x%x\n", f);
    Serial.println("[Kamera] Pruefe: Flachbandkabel richtig drin? PSRAM aktiviert?");
    return false;
  }

  sensor_t* s = esp_camera_sensor_get();
  if (s) {
    if (BILD_SPIEGELN) s->set_hmirror(s, 1);
    if (BILD_DREHEN)   s->set_vflip(s, 1);
  }
  // Alles Weitere steht im Tag-/Nachtprofil — es wird bei jedem Umschalten
  // neu geschrieben, nicht nur einmal beim Start.
  kameraProfil(false, true);

  Serial.println("[Kamera] Laeuft.");
  return true;
}

// ============================================================================
//  VORLAUF-GEDÄCHTNIS
// ============================================================================
static void vorlaufAnlegen() {
  for (int i = 0; i < VORLAUF_MAX_BILDER; i++) {
    vorlaufRing[i].daten = (uint8_t*)ps_malloc(VORLAUF_SLOT_BYTES);
    if (!vorlaufRing[i].daten) {
      Serial.printf("[Vorlauf] Nur Platz fuer %d Bilder.\n", i);
      break;
    }
  }
}

static void vorlaufMerken(camera_fb_t* bild) {
  VorlaufBild& slot = vorlaufRing[vorlaufSchreib];
  if (!slot.daten) return;

  if (bild->len > VORLAUF_SLOT_BYTES) {
    // Passiert, wenn die Bilder größer sind als erwartet. Dann fehlt der
    // Vorlauf — deshalb einmal deutlich warnen statt still zu schlucken.
    static bool schonGewarnt = false;
    if (!schonGewarnt) {
      schonGewarnt = true;
      Serial.printf("[Vorlauf] WARNUNG: Bild ist %u Bytes, Platz nur %u!\n",
                    bild->len, (unsigned)VORLAUF_SLOT_BYTES);
      Serial.println("[Vorlauf] Der Vorlauf funktioniert so NICHT.");
      Serial.println("[Vorlauf] Abhilfe: BILD_QUALITAET erhoehen (z.B. 16),");
      Serial.println("[Vorlauf] oder BILD_GROESSE auf FRAMESIZE_SVGA stellen.");
    }
    return;
  }
  memcpy(slot.daten, bild->buf, bild->len);
  slot.laenge = bild->len;
  vorlaufSchreib = (vorlaufSchreib + 1) % VORLAUF_MAX_BILDER;
  if (vorlaufAnzahl < VORLAUF_MAX_BILDER) vorlaufAnzahl++;
}

// Alle gemerkten Bilder in den frisch gestarteten Clip schreiben,
// beginnend beim ältesten.
static void vorlaufAusschuetten() {
  uint8_t start = (vorlaufSchreib + VORLAUF_MAX_BILDER - vorlaufAnzahl) % VORLAUF_MAX_BILDER;
  for (uint8_t i = 0; i < vorlaufAnzahl; i++) {
    VorlaufBild& s = vorlaufRing[(start + i) % VORLAUF_MAX_BILDER];
    if (s.daten && s.laenge) { aviFrame(s.daten, s.laenge); clipBilder++; }
  }
}

// ============================================================================
//  TON IN DEN CLIP SCHREIBEN
// ============================================================================
//  Zweimal gebraucht: einmal für den Vorlauf beim Start, und danach in jeder
//  Runde für den Ton, der seit dem letzten Bild angefallen ist. Deshalb hier
//  als eigene kleine Funktion.
// ============================================================================
// Wie lange hat das Aufnehmen des Vorlaufs gedauert? Das sind die Bilder, die
// VOR dem Auslöser entstanden sind — ihre Zeit zählt zur Spielzeit des Clips.
static uint32_t vorlaufDauerMs() {
  float fps = (aktuelleFps > 1.0f) ? aktuelleFps : 8.0f;
  return (uint32_t)(vorlaufAnzahl * 1000.0f / fps);
}

static void tonVorlaufInClip(uint32_t vorlaufMs) {
  if (!clipHatTon || !tonPuffer) return;

  // Genau so lang wie der Bild-Vorlauf — sonst wäre die Tonspur länger als
  // die Bildspur, und beide liefen auseinander. Mehr als TON_VORLAUF_MS passt
  // nicht in den Eimer.
  if (vorlaufMs > TON_VORLAUF_MS) vorlaufMs = TON_VORLAUF_MS;

  uint32_t proben = (uint32_t)TON_ABTASTRATE * vorlaufMs / 1000;
  if (proben == 0) { tonAufsetzen(TON_ABNEHMER_CLIP); return; }
  if (proben > TON_VORLAUF_PROBEN) proben = TON_VORLAUF_PROBEN;

  tonVorlaufAbholen(tonPuffer, proben);

  // In Häppchen schreiben statt in einem Klotz — so bleibt die Datei auch
  // am Anfang schön abwechselnd aus Bild und Ton aufgebaut.
  for (uint32_t ab = 0; ab < proben; ab += TON_BLOCK_PROBEN) {
    uint32_t n = min<uint32_t>(TON_BLOCK_PROBEN, proben - ab);
    aviTon(tonPuffer + ab, n);
  }
}

static void tonNachschieben() {
  if (!clipHatTon || !tonPuffer) return;
  size_t n;
  while ((n = tonAbholen(TON_ABNEHMER_CLIP, tonPuffer, TON_BLOCK_PROBEN)) > 0) {
    aviTon(tonPuffer, n);
    if (n < TON_BLOCK_PROBEN) break;      // mehr ist gerade nicht da
  }
}

// ============================================================================
//  EIN FOTO SPEICHERN
// ============================================================================
static void fotoSpeichern(camera_fb_t* bild) {
  if (!speicherBereit()) return;
  String pfad = naechsterFotoName();
  File f = SD.open(pfad.c_str(), "w");
  if (!f) return;
  f.write(bild->buf, bild->len);
  f.close();
  stats.fotosGesamt++;
  stats.fotosHeute++;
}

// ============================================================================
//  AUFNAHME STARTEN / BEENDEN
// ============================================================================
static void aufnahmeStarten(camera_fb_t* bild) {
  if (!speicherBereit()) return;
  if (millis() - letzterClipEnde < CLIP_PAUSE_SEKUNDEN * 1000UL) return;

  String pfad = naechsterClipName();

  // Ton nur, wenn er eingeschaltet ist UND das Mikrofon wirklich läuft.
  clipHatTon = TON_IN_CLIPS && audioLaeuft() && tonPuffer;
  uint32_t tonRate = clipHatTon ? audioAbtastrate() : 0;

  if (!aviStart(pfad, bild->width, bild->height, tonRate)) {
    clipHatTon = false;
    return;
  }

  clipBilder    = 0;
  nehmeAuf      = true;
  aufnahmeStart = millis();

  clipVorlaufMs = vorlaufDauerMs();   // vor dem Ausschütten merken
  vorlaufAusschuetten();       // die Sekunden VOR dem Anflug mitnehmen
  tonVorlaufInClip(clipVorlaufMs);   // und den Ton dazu

  // Gezählt wird nur von der Lichtschranke — die ist genauer.
  // Ohne Lichtschranke müssen wir hier zählen, sonst gäbe es keine Statistik.
  if (!LICHTSCHRANKE_AN) besuchZaehlen();

  fotoSpeichern(bild);         // ein Standbild für die Galerie
  stats.clipsGesamt++;
  stats.clipsHeute++;

  Serial.printf("[Clip] Start: %s%s (Besuch Nr. %u heute)\n",
                pfad.c_str(), clipHatTon ? " (mit Ton)" : "",
                stats.besucheHeute);
}

static void aufnahmeBeenden() {
  if (!nehmeAuf) return;
  tonNachschieben();           // den letzten Rest Ton noch mitnehmen

  // Die Vorlaufzeit gehört zur Spielzeit dazu: Diese Bilder sind VOR
  // aufnahmeStart entstanden. Ohne sie käme eine zu hohe Bildrate heraus,
  // der Clip würde zu schnell abgespielt — und die Tonspur wäre länger als
  // die Bildspur.
  float sek = (millis() - aufnahmeStart + clipVorlaufMs) / 1000.0f;
  float fps = (sek > 0.2f) ? (clipBilder / sek) : 8.0f;
  aviEnde(fps);
  nehmeAuf        = false;
  clipHatTon      = false;
  letzterClipEnde = millis();
}

// ---------------------------------------------------------------------------
//  Bluetooth bleibt aus. Immer.
//
//  Der Nistkasten spricht ausschließlich über WLAN. Bluetooth wird von dieser
//  Firmware nirgends benutzt — wir schalten es trotzdem ausdrücklich ab,
//  statt uns darauf zu verlassen, dass es niemand anwirft. Das hat zwei
//  handfeste Gründe:
//
//    1. Der Funkteil gibt rund 60 KB Arbeitsspeicher frei, den die Kamera
//       gut gebrauchen kann.
//    2. Bluetooth und WLAN teilen sich beim ESP32 dieselbe Antenne. Ein
//       nebenher laufender BLE-Stack kostet Strom und stört den Livestream.
//
//  esp_bt_controller_mem_release() ist endgültig: Nach diesem Aufruf lässt
//  sich Bluetooth bis zum nächsten Neustart nicht mehr einschalten. Genau
//  das wollen wir.
// ---------------------------------------------------------------------------
static void bluetoothAbschalten() {
#if CONFIG_BT_ENABLED
  esp_bt_controller_disable();
  if (esp_bt_controller_mem_release(ESP_BT_MODE_BLE) == ESP_OK)
    Serial.println("[System] Bluetooth aus, Speicher freigegeben.");
  else
    Serial.println("[System] Bluetooth aus.");
#else
  Serial.println("[System] Bluetooth ist schon im Board-Paket abgeschaltet.");
#endif
}

// ============================================================================
//  SETUP
// ============================================================================
void setup() {
  Serial.begin(115200);
  delay(600);
  Serial.println("\n\n=====  BirdyCam startet  =====");

  bluetoothAbschalten();          // vor allem anderen, solange RAM frei ist

  if (!psramFound()) {
    Serial.println("!! KEIN PSRAM !!  In der Arduino IDE unter Werkzeuge");
    Serial.println("   PSRAM auf \"OPI PSRAM\" stellen und neu hochladen.");
  } else {
    Serial.printf("[System] PSRAM: %u KB frei\n", ESP.getFreePsram() / 1024);
  }

  stromStart();
  irLicht(0);
  akkuJetzt = akkuVolt();         // damit irDauerlichtJetzt() sofort Bescheid weiß

  if (!kameraStarten()) {
    Serial.println("[System] Ohne Kamera geht nichts. Neustart in 10 s.");
    delay(10000);
    ESP.restart();
  }

  if (!speicherStart())
    Serial.println("[System] Keine SD-Karte — Livestream geht, Aufnehmen nicht.");

  bewegungStart();
  vorlaufAnlegen();
  lichtschrankeStart();
  netzwerkStart();                // Router oder eigenes WLAN, siehe config.h
  systemStart();

  // Der Ton muss VOR dem Webserver starten: der macht die Ton-Tür (82) nur
  // auf, wenn das Mikrofon wirklich läuft.
  if (audioStart() && TON_IN_CLIPS) {
    // Der Eimer für den Ton-Vorlauf: 2,5 s bei 16 kHz sind 80 KB. Der liegt
    // im PSRAM, weil der normale Arbeitsspeicher dafür zu knapp ist.
    tonPuffer = (int16_t*)ps_malloc(TON_PUFFER_PROBEN * sizeof(int16_t));
    if (!tonPuffer)
      Serial.println("[Ton] Kein PSRAM fuer den Clip-Ton — Clips bleiben stumm.");
  }

  webStart();

  fpsFenster = millis();
  Serial.println("=====  BirdyCam ist bereit  =====\n");
}

// ============================================================================
//  LOOP — die Hauptschleife
// ============================================================================
void loop() {
  webBedienen();

  netzwerkPflegen(istNacht);      // Verbindung überwachen, Captive-Portal-DNS
  systemPflegen();                // Speicher beobachten

  // --- Akku alle 30 Sekunden prüfen ---------------------------------------
  if (millis() - letzterAkkuCheck > 30000) {
    letzterAkkuCheck = millis();
    akkuTrendPflegen();           // steigt der Akku oder fällt er?
    akkuJetzt = akkuVolt();       // gepuffert für irDauerlichtJetzt()
    akkuHeuteMerken(akkuJetzt);   // Minimum/Maximum für die Tagesstatistik
    akkuSchutzPruefen();          // legt sich ggf. schlafen und kommt nicht zurück
  }

  // --- Fertige Gesangsaufnahme wegschreiben -------------------------------
  if (audioFertig() && speicherBereit()) {
    if (audioSpeichern(naechsterAudioName())) {
      stats.audioGesamt++;
      stats.audioHeute++;
    }
  }

  statistikPflegen();

  // --- Nachts langsamer arbeiten (Strom sparen) ---------------------------
  //  Wird gerade aufgenommen oder schaut jemand zu, wird nicht gebummelt.
  bool betrieb = nehmeAuf || webHatZuschauer();
  if (istNacht && !betrieb) {
    delay(NACHT_PAUSE_MS);
    webBedienen();                // zwischendurch trotzdem erreichbar bleiben
  }

  // --- Lichtprobe: Ist es draußen inzwischen hell geworden? ---------------
  //  Die Tag/Nacht-Entscheidung darf NIE an einem Bild hängen, das vom
  //  eigenen IR-Licht beleuchtet ist — sonst beißt sich die Katze in den
  //  Schwanz: Licht an, Bild hell, "ist ja Tag", Licht aus, Bild schwarz,
  //  "ist ja Nacht", Licht an ... und das im Sekundentakt.
  //
  //  Deshalb geht das Licht einmal je NACHT_PROBE_SEKUNDEN kurz aus, und nur
  //  dieses eine Bild zählt für die Entscheidung.
  //
  //  Während einer Aufnahme nicht: Das dunkle Bild wäre sonst mitten im Clip.
  //  Ein Clip dauert höchstens CLIP_MAX_SEKUNDEN, die Probe kommt also
  //  spätestens 20 Sekunden später.
  bool lichtprobe = istNacht && !nehmeAuf &&
                    (millis() - letzteLichtprobe > NACHT_PROBE_SEKUNDEN * 1000UL);
  if (lichtprobe) {
    letzteLichtprobe = millis();
    if (irIstAn()) irBlitzAus();

    // Mit FESTER Belichtung (IR_HINTER_DEM_FENSTER) wäre die Probe aussagelos:
    // Werte, die auf IR-Licht eingestellt sind, sagen nichts darüber, wie hell
    // es ohne IR ist — die Kamera fände nie in den Tagbetrieb zurück. Für
    // dieses eine Bild deshalb das Tagprofil mit Automatik aufsetzen (leise,
    // sonst steht der Serielle Monitor jede Minute voll).
    if (IR_HINTER_DEM_FENSTER) kameraProfil(false, false);

    // Die Automatik braucht ein paar Bilder, bis sie sich eingestellt hat.
    kameraLeerlaufen(IR_SETTLE_BILDER + 2);
  }

  // --- IR-Licht vorbereiten ----------------------------------------------
  //  Dauerlicht: Die LEDs bleiben an, damit sich die Belichtungsautomatik
  //  einpendeln kann. Blitz: Sie gehen nur für dieses Bild an — und danach
  //  müssen die veralteten Bilder aus den Kamerapuffern raus, sonst
  //  beleuchtet der Blitz nur Bilder, die niemand zu sehen bekommt.
  bool lichtBleibtAn = irDauerlichtJetzt() || betrieb;
  if (istNacht && !lichtprobe) {
    if (!irIstAn()) { irBlitzAn(); kameraLeerlaufen(IR_SETTLE_BILDER); }
  } else if (!istNacht && irIstAn()) {
    irBlitzAus();
  }

  // --- Bild holen ---------------------------------------------------------
  camera_fb_t* bild = esp_camera_fb_get();
  if (!bild) {
    systemKameraFehlerPlus();     // wird auf der Website mitgezählt
    delay(30);
    return;
  }

  // --- Livestream bedienen ------------------------------------------------
  //  Die Lichtprobe nicht mitschicken — der Zuschauer bekommt sonst jede
  //  Minute ein schwarzes Bild zu sehen. Er merkt nur, dass ein Bild fehlt.
  if (!lichtprobe) webFrameSenden(bild);

  // --- Bewegung prüfen ----------------------------------------------------
  BewegungErgebnis b = bewegungPruefen(bild);

  // Diese Zahl steht auf der Website und ist die Einstellhilfe für
  // IR_HELLIGKEIT (siehe config.h). Gezeigt wird das normale Bild, nicht die
  // absichtlich dunkle Lichtprobe — sonst stellte man nach dem falschen Wert.
  if (!lichtprobe) aktuelleHelligkeit = b.helligkeit;

  // Tag/Nacht umschalten. Etwas Abstand zwischen den Schwellen ("Hysterese"),
  // damit es in der Dämmerung nicht dauernd hin und her springt.
  //
  // Es zählt nur ein Bild OHNE eigenes IR-Licht: tagsüber jedes, nachts die
  // Lichtprobe von oben.
  if (!istNacht || lichtprobe) {
    bool warNacht = istNacht;
    if (!istNacht && b.helligkeit < NACHT_SCHWELLE)          istNacht = true;
    else if (istNacht && b.helligkeit > NACHT_SCHWELLE + 25) istNacht = false;
    if (warNacht != istNacht) {
      Serial.printf("[Licht] Umschalten auf %s (Helligkeit %u)\n",
                    istNacht ? "NACHT" : "TAG", b.helligkeit);
      kameraProfil(istNacht, true);   // Graustufen, Verstärkung, Belichtung
      bewegungZuruecksetzen();        // sonst löst der Lichtwechsel Fehlalarm aus
    }
  }

  // Die Lichtprobe ist absichtlich dunkel — als Bewegung zählt sie nicht,
  // und das nächste Bild darf nicht mit ihr verglichen werden.
  if (lichtprobe) {
    // Das für die Probe aufgesetzte Tagprofil wieder wegräumen. Hat die Probe
    // gerade auf TAG umgeschaltet, steht es ohnehin schon richtig.
    if (IR_HINTER_DEM_FENSTER && istNacht) kameraProfil(true, false);
    b.gueltig = false;
    b.bewegung = false;
    bewegungZuruecksetzen();
  }

  // --- Stream ODER Aufnahme, nicht beides --------------------------------
  //  Bei Full HD geht ein Bild entweder ins WLAN oder auf die SD-Karte.
  //  Beides gleichzeitig überfordert die Karte, und dann fehlen Bilder
  //  im Clip. Siehe config.h, STREAM_HAT_VORRANG.
  bool streamBlockiert = STREAM_HAT_VORRANG && webHatZuschauer();

  if (streamBlockiert) {
    if (nehmeAuf) {
      Serial.println("[Modus] Zuschauer da -> laufende Aufnahme wird beendet.");
      aufnahmeBeenden();
    }
    // Gezählt wird trotzdem weiter! Die Lichtschranke kostet nichts, und
    // die Statistik soll keine Löcher haben, nur weil jemand zuschaut.
    lichtschrankePruefen();

    // Kein vorlaufMerken() beim Streamen: Das Gedächtnis ist danach leer,
    // der erste Clip nach dem Zuschauen hat also keinen Vorlauf. Dafür
    // bleibt der Stream flüssig.

  } else {
    // --- Auslöser sammeln -------------------------------------------------
    // Zwei Quellen, die verschiedene Dinge sehen:
    //   Lichtschranke = jemand fliegt durch das Loch  (exakt, zählt)
    //   Bildvergleich = im Kasten bewegt sich etwas   (z. B. Füttern)
    bool ausgeloest = false;
    if (lichtschrankePruefen()) ausgeloest = true;
    if (b.gueltig && b.bewegung) ausgeloest = true;

    if (ausgeloest) {
      letzteBewegung = millis();
      if (!nehmeAuf) aufnahmeStarten(bild);
    }

    if (nehmeAuf) {
      aviFrame(bild->buf, bild->len);
      clipBilder++;
      tonNachschieben();          // der Ton seit dem letzten Bild kommt dahinter

      bool ruheVorbei = (millis() - letzteBewegung > NACHLAUF_SEKUNDEN * 1000UL);
      bool zuLang     = (millis() - aufnahmeStart  > CLIP_MAX_SEKUNDEN * 1000UL);
      if (ruheVorbei || zuLang) aufnahmeBeenden();
    } else if (!lichtprobe) {
      vorlaufMerken(bild);        // ins Gedächtnis für den nächsten Anflug
      // Die Lichtprobe gehört da nicht hinein — sie wäre ein schwarzes Bild
      // mitten im Vorlauf des nächsten Clips.
    }
  }

  esp_camera_fb_return(bild);     // Bildpuffer wieder freigeben — sonst Absturz!

  // --- IR-Licht wieder aus, wenn es nur ein "Blitz" war -------------------
  if (istNacht && !lichtBleibtAn) irBlitzAus();

  // --- Bildrate berechnen (nur für die Anzeige) --------------------------
  fpsZaehler++;
  if (millis() - fpsFenster >= 2000) {
    aktuelleFps = fpsZaehler * 1000.0f / (millis() - fpsFenster);
    fpsZaehler  = 0;
    fpsFenster  = millis();
  }
}
