// Einbinden der Ruin & Wesen MIDI Library
#include <MidiUart.h>
#include <Midi.h>
MidiClass Midi;

// Konfigurieren der LED und Taster Pins
int ledPins[2] = { 10, 11 };
int buttonPins[2] = { 12, 13 };

void setup() {
   // initialisieren der MIDI schnittstelle
   MidiUart.init();

   // initialisieren der beiden Taster und LEDs
   int i;
   for (i = 0; i < 2; i++) {
     pinMode(ledPins[i], OUTPUT);       // die LED pins als output setzen
     pinMode(buttonPins[i], INPUT);     // die Tasterpins als input setzen
     digitalWrite(buttonPins[i], HIGH); // aktivieren des Pullup-Widerstandes
   }
}

// Speichern der Potentiometerwerte
int potValues[4];
int ccNumbers[4] = { 10, 11, 12, 13 };

void loop() {
  int i;
  
  // Abfragen der Potentiometerwerte
  for (i = 0; i < 4; i++) {
    int value = analogRead(i); // Einlesen des analogen Wertes
    value = map(value, 0, 1023, 0, 127); // Konvertieren in den MIDI-Bereich

    // Falls sich ein Potentiometer genug verŠndert hat
    if (abs(value - potValues[i]) >= 1) {
      potValues[i] = value;
      MidiUart.sendCC(ccNumbers[i], value);
    }
  }
  
  // XXX Kantenerkennung
  
  // Abfragen der Taster
  for (i = 0; i < 2; i++) {
    // der Taster ist gedrŸckt
    if (digitalRead(buttonPins[i]) == LOW) {
    }
}
