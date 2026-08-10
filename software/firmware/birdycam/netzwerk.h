// ============================================================================
//  netzwerk.h — Router oder eigenes WLAN
// ============================================================================
//  Zwei Wege, die Kamera zu erreichen:
//
//   1. ROUTER-BETRIEB ("Station")
//      Die Kamera ist ein Gerät in deinem Heimnetz, wie ein Handy.
//      Du rufst sie vom Sofa auf. Sparsam, Uhrzeit kommt aus dem Internet.
//
//   2. EIGENES WLAN ("Access Point")
//      Die Kamera IST der Router. Dein Handy verbindet sich direkt mit ihr.
//      Funktioniert überall, auch ohne Heimnetz — kostet aber mehr Strom,
//      weil ein WLAN-Sender ständig Funkbaken aussenden muss und nicht
//      schlafen darf.
//
//  Welcher Weg gilt, steht in config.h unter NETZ_MODUS.
//
//  Im Modus AUTO wird zuerst der Router versucht. Kommt er nicht, macht die
//  Kamera ihr eigenes WLAN auf — man steht also nie ohne Zugang da.
// ============================================================================

#pragma once
#include <Arduino.h>

// Einmal beim Start aufrufen. Baut die Verbindung nach NETZ_MODUS auf.
void netzwerkStart();

// In jeder Runde der Hauptschleife aufrufen. Kümmert sich um:
//   - Verbindungsabbrüche zum Router (baut neu auf)
//   - den Captive-Portal-DNS im eigenen WLAN
//   - optionales Abschalten des eigenen WLAN bei Nacht
void netzwerkPflegen(bool istNacht);

// Läuft gerade das eigene WLAN (Access Point)?
bool netzwerkIstEigenes();

// Ist überhaupt jemand erreichbar? (Router verbunden oder eigenes WLAN an)
bool netzwerkBereit();

// Die Adresse, unter der die Website erreichbar ist, z. B. "192.168.4.1"
String netzwerkAdresse();

// Klartext für die Website, z. B. "eigenes WLAN" oder "Router (-61 dBm)"
String netzwerkModusText();

// Wie viele Geräte sind mit dem eigenen WLAN verbunden? (0 im Routerbetrieb)
int netzwerkClients();

// Wie oft musste die Router-Verbindung neu aufgebaut werden?
uint32_t netzwerkNeuverbindungen();

// Ist die Uhrzeit plausibel gesetzt?
bool zeitIstGesetzt();

// Uhrzeit von außen setzen (macht die Website mit der Browser-Zeit).
// Gibt false zurück, wenn der Wert unplausibel ist.
bool zeitSetzen(uint32_t unixZeit);
