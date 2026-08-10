// ============================================================================
//  systeminfo.h — Wie geht es dem Gerät?
// ============================================================================
//  Sammelt alles, was man über den Zustand des Systems wissen will, und
//  bewertet es. Das Ergebnis landet auf der Website als "Systemzustand".
//
//  Warum das wichtig ist: Der Kasten hängt ab März unerreichbar im Garten.
//  Man kann dann nichts mehr nachschauen — außer über die Website. Also
//  muss die Website alles zeigen, was man zur Beurteilung braucht:
//  Speicher, WLAN, Temperatur, Neustarts, Kartenfehler.
//
//  Zusätzlich passt dieses Modul auf das WLAN auf: Fliegt die Verbindung
//  raus, baut es sie neu auf. Ohne das wäre die Kamera nach dem ersten
//  Router-Neustart für immer offline.
// ============================================================================

#pragma once
#include <Arduino.h>

// Einmal beim Start aufrufen (nach dem WLAN).
void systemStart();

// In jeder Runde der Hauptschleife aufrufen. Kümmert sich um:
//   - WLAN-Überwachung und Neuverbindung
//   - Minimum des freien Speichers mitschreiben
void systemPflegen();

// Laufzeit in Sekunden. Nutzt esp_timer statt millis(), weil millis()
// nach 49 Tagen überläuft — und die Kamera soll länger laufen.
uint32_t systemLaufzeitS();

// Zähler, die von außen hochgedreht werden
void systemKameraFehlerPlus();
uint32_t systemKameraFehler();
uint32_t systemWlanNeuverbindungen();

// Warum ist das Gerät zuletzt gestartet? ("Einschalten", "Watchdog", ...)
const char* systemNeustartGrund();

// Alles als JSON-Objekt für die Website (ohne umschließende Klammern,
// damit web.cpp es in seine Antwort einbetten kann).
String systemAlsJson();

// Gesamtbewertung: 0 = alles gut, 1 = Achtung, 2 = Problem
// warnungen wird mit einer JSON-Liste von Klartext-Meldungen gefüllt.
uint8_t systemGesundheit(String& warnungen);
