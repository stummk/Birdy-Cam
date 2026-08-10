// ============================================================================
//  lichtschranke.h — Vögel zählen
// ============================================================================
//  Ein Infrarot-Strahl liegt quer im Einflugloch. Fliegt ein Vogel durch,
//  bricht er ihn für etwa 100 bis 300 Millisekunden.
//
//  Daraus werden echte Zahlen:
//     Unterbrechung 1  ->  EINFLUG   (Uhrzeit merken)
//     Unterbrechung 2  ->  AUSFLUG   (Dauer = Zeit dazwischen)
//
//  Warum das besser ist als Bildvergleich: Ein Sonnenfleck, der durch den
//  Kasten wandert, sieht im Bild wie Bewegung aus. Der Strahl im Loch
//  interessiert sich dafür nicht. Deshalb sind die Zahlen auf der Website
//  Messwerte und keine Schätzungen.
//
//  Technisch benutzen wir einen "Interrupt": Der Chip unterbricht das
//  Hauptprogramm sofort, wenn sich der Pin ändert. Sonst würden wir kurze
//  Unterbrechungen verpassen — das Hauptprogramm ist ja mit Bildern
//  beschäftigt und schaut nur ~8 mal pro Sekunde nach.
// ============================================================================

#pragma once
#include <Arduino.h>

// Einmal beim Start aufrufen.
bool lichtschrankeStart();

// In jeder Runde der Hauptschleife aufrufen. Gibt true zurück, wenn gerade
// ein Vogel erkannt wurde (dann soll die Kamera aufnehmen).
bool lichtschrankePruefen();

// Sitzt gerade ein Vogel im Kasten?
bool vogelIstDrin();

// Wie lange ist er schon drin? (Sekunden, 0 wenn keiner drin ist)
uint32_t vogelDrinSeitSekunden();

// Zähler für die Website
uint32_t lichtschrankeDurchfluege();
uint32_t lichtschrankeIgnoriert();

// Ist der Strahl gerade frei?
bool strahlIstFrei();
