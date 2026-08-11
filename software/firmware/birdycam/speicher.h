// ============================================================================
//  speicher.h — SD-Karte, Ringspeicher und Statistik
// ============================================================================
//  Der "Ringspeicher" ist die Antwort auf "nach 200 Clips soll er den
//  ältesten überschreiben". Es gibt feste Dateinamen clip_000.avi bis
//  clip_199.avi, und ein Zähler sagt, welcher als nächstes dran ist.
//  Danach fängt er wieder bei 0 an. Wie ein Karussell.
// ============================================================================

#pragma once
#include <Arduino.h>
#include <FS.h>
#include <SD.h>

// ---- Statistik: alles, was die Website anzeigt -----------------------------
struct Statistik {
  uint32_t besucheGesamt   = 0;   // Bewegungen seit dem ersten Start
  uint32_t besucheHeute    = 0;
  uint32_t stunden[24]     = {0}; // Anflüge je Tageszeit -> das Balkendiagramm
  uint32_t clipsGesamt     = 0;
  uint32_t fotosGesamt     = 0;
  uint32_t audioGesamt     = 0;
  uint32_t letzterBesuch   = 0;   // Unix-Zeit
  uint32_t ersterHeute     = 0;
  uint32_t letzteAufenthaltS = 0; // wie lange war der letzte Vogel drin?
  uint16_t ringClip        = 0;   // welcher Ringplatz ist als nächstes dran
  uint16_t ringFoto        = 0;
  uint16_t ringAudio       = 0;
  uint8_t  tagImJahr       = 0;   // um Mitternacht "heute" zurücksetzen

  // ---- Werte, die um Mitternacht nach /tage.csv wandern -------------------
  uint32_t clipsHeute      = 0;
  uint32_t fotosHeute      = 0;
  uint32_t audioHeute      = 0;
  uint32_t sekundenDrinHeute = 0; // Aufenthaltsdauer aller Vögel zusammen
  uint16_t akkuMinMv       = 0;   // tiefster Akkustand heute in Millivolt
  uint16_t akkuMaxMv       = 0;   // höchster (0 = heute noch nichts gemessen)
  uint32_t tagDatum        = 0;   // laufender Tag als JJJJMMTT, z. B. 20260416
};

extern Statistik stats;

// SD-Karte starten. Gibt false zurück, wenn keine Karte steckt.
bool speicherStart();

// true, wenn die Karte da ist und beschreibbar
bool speicherBereit();

// Nächsten Dateinamen im Ring holen und den Zähler weiterdrehen.
// Beispiel: naechsterClipName() -> "/clips/clip_017.avi"
String naechsterClipName();
String naechsterFotoName();
String naechsterAudioName();

// Statistik von der Karte laden / auf die Karte schreiben
void statistikLaden();
void statistikSpeichern();

// Muss regelmäßig aufgerufen werden: speichert die Statistik höchstens
// einmal pro Minute (schont die Karte) und macht um Mitternacht "heute" leer.
void statistikPflegen();

// Ein Besuch wurde erkannt -> Zähler hochdrehen
void besuchZaehlen();

// Ein Vogel ist wieder ausgeflogen: Aufenthaltsdauer dazurechnen.
void aufenthaltZaehlen(uint32_t sekunden);

// Akkustand für die Tagesstatistik mitschreiben (Minimum und Maximum).
// Regelmäßig aufrufen, z. B. bei jeder Akkuprüfung.
void akkuHeuteMerken(float volt);

// Belegung der Karte in Prozent (für die Website)
uint8_t kartenBelegungProzent();

// Statistik als JSON-Text (das liest die Website)
String statistikAlsJson();

// ---- Tagesarchiv /tage.csv -------------------------------------------------
//  Eine Zeile je Tag, alle gleich lang, immer nur angehängt. Warum CSV und
//  nicht JSON steht in config.h bei TAGE_CSV_AN.
//
//  Aufbau der Zeile (156 Bytes inkl. Zeilenende):
//    datum;besuche;erster;letzter;drin_s;clips;fotos;audio;akku_min;akku_max;
//    h00;h01;...;h23
//
//  Beispiel:
//    2026-04-16;0047;05:41;20:12;001820;0031;0112;0000;3.61;4.09;000;000;...

#define TAGE_DATEI        "/tage.csv"
#define TAGE_ZEILE_BYTES  156

// Die letzten `anzahl` Tage als JSON für die Website. Enthält außerdem den
// Stunden-Durchschnitt über diese Tage — damit muss der Browser nicht 720
// Einzelwerte durchrechnen und die Antwort bleibt klein.
String tageAlsJson(uint16_t anzahl);

// Den laufenden Tag sofort ins Archiv schreiben, ohne auf Mitternacht zu
// warten. Nutzt die Website, damit ein Tag nicht verloren geht, wenn die
// Kamera abends ausgeschaltet wird.
bool tagJetztSichern();
