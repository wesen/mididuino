// Einbinden der RuinWesen MIDI Library
#include <MidiUart.h>
#include <Midi.h>
MidiClass Midi;

// Speichern der aktuellen Potentiometerwerte
int potValues[4]; 

// Speichern der Kontrollernummern
int ccNumbers[4] = { 10, 11, 12, 13 };

void setup() {
  // Initialisieren der MIDI SChnittstelle
  MidiUart.init();
}

void loop() {
  int i;
  for (i = 0; i < 4; i++) {
    // Auslesen des Potentiometerwertes
    int value = analogRead(i);
    // Konvertieren des analogen Wertes auf den MIDI-Wertebereich
    value = map(value, 0, 1023, 0, 127);
    // Erkennen, ob der Potentiometer bewegt wurde
    if (abs(value - potValues[i]) >= 2) {
      // Speichern des neuen Potentiometerwertes
      potValues[i] = value;
      // Senden der MIDI CONTROL CHANGE Nachricht
      MidiUart.sendCC(ccNumbers[i], value);
    }
  }
}
