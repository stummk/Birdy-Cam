// ============================================================================
//  audio.h — Mikrofon, Ton für Clips und Livestream, Vogelgesang
// ============================================================================
//  Das Mikrofon sitzt fest auf der XIAO Sense und läuft die ganze Zeit mit.
//  Damit die Kamera davon nicht ins Stottern kommt, horcht es auf dem
//  ZWEITEN Prozessorkern — der ESP32-S3 hat zwei. Einer für Bild, einer
//  für Ton.
//
//  Der Ton landet in einem Ringpuffer im PSRAM, und daraus bedienen sich
//  ZWEI Abnehmer unabhängig voneinander:
//
//      Mikrofon ──> [ Ring, 4 Sekunden ] ──┬──> Videoclip (AVI-Tonspur)
//                                          └──> Livestream (Tür 82)
//
//  Warum ein Ring mit zwei Lesezeigern und nicht einfach zwei Puffer:
//  Der Clip braucht den Ton der letzten Sekunden RÜCKWIRKEND (Vorlauf),
//  der Stream braucht ihn ab jetzt. Ein Ring kann beides, weil er das
//  Vergangene noch eine Weile aufhebt.
//
//  ⚠️ Was das NICHT kann: sagen, WELCHER Vogel singt. Dafür bräuchte man
//  BirdNET, und das braucht einen Raspberry Pi. Siehe Machbarkeit 1.7.
// ============================================================================

#pragma once
#include <Arduino.h>

// Die zwei Abnehmer. Jeder hat seinen eigenen Lesezeiger im Ring.
#define TON_ABNEHMER_CLIP     0
#define TON_ABNEHMER_STREAM   1
#define TON_ABNEHMER_ANZAHL   2

// Einmal beim Start aufrufen. Startet Mikrofon und Hintergrund-Aufgabe.
// Gibt false zurück, wenn AUDIO_AN aus ist oder das Mikrofon nicht mag.
bool audioStart();

// Läuft das Mikrofon? (Alles andere hier ist ohne das sinnlos.)
bool audioLaeuft();

// Aktueller Lautstärkewert — praktisch, um GESANG_SCHWELLE einzustellen.
// Gemessen an den UNVERSTÄRKTEN Werten, passt also zu Tutorial-Schritt 6.
uint16_t audioLautstaerke();

// Abtastrate in Proben pro Sekunde (aus config.h).
uint32_t audioAbtastrate();

// ---------------------------------------------------------------------------
//  Ton abholen
// ---------------------------------------------------------------------------

// Lesezeiger auf "jetzt" setzen: alles, was bisher im Ring liegt, wird
// übersprungen. Vor dem Start einer Aufnahme oder eines Streams aufrufen.
void tonAufsetzen(uint8_t abnehmer);

// Holt bis zu maxProben neue Proben ab und gibt zurück, wie viele es waren
// (0 = im Moment nichts Neues). Blockiert nie.
size_t tonAbholen(uint8_t abnehmer, int16_t* ziel, size_t maxProben);

// Den Ton der letzten Sekunden für den Clip-Vorlauf holen. Liefert IMMER
// genau `proben` Werte: Ist noch nicht so viel Ton da (kurz nach dem Start),
// wird vorne mit Stille aufgefüllt. So bleiben Bild und Ton gleich lang.
// Setzt danach den Clip-Lesezeiger auf "jetzt".
void tonVorlaufAbholen(int16_t* ziel, size_t proben);

// Wie viele Proben hat dieser Abnehmer verpasst, weil er zu langsam war?
// Steht auf der Website unter "Technische Details".
uint32_t tonVerloren(uint8_t abnehmer);

// ---------------------------------------------------------------------------
//  Vogelgesang als eigene WAV-Datei  (GESANG_AUFNEHMEN)
// ---------------------------------------------------------------------------

// true, wenn eine Aufnahme fertig im Speicher liegt und weggeschrieben
// werden möchte. Wird in der Hauptschleife abgefragt.
bool audioFertig();

// Läuft gerade eine Gesangsaufnahme?
bool audioNimmtAuf();

// Die fertige Aufnahme als WAV-Datei speichern und Platz wieder freigeben.
bool audioSpeichern(const String& pfad);
