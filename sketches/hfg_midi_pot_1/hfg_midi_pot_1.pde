// Einbinden der Ruinwesen MIDI Library
#include <MidiUart.h>
#include <Midi.h>
MidiClass Midi;

int potPin = 0;
int tasterPin = 3;

void setup() {
  // Initialisieren der MIDI Schnittstelle
  MidiUart.init();
  pinMode(tasterPin, INPUT);
  digitalWrite(tasterPin, HIGH);
}

int tasterStatus = HIGH;
int pitch = 0;

void loop() {
  int tasterWert = digitalRead(tasterPin);
  
  // taster entweder losgelassen oder gedrueckt
  if (tasterStatus != tasterWert) {
    if (tasterWert == LOW) { 
      // taster gedrueckt
      pitch = map(analogRead(0), 0, 1023, 40, 100);
      MidiUart.sendNoteOn(pitch, 100);
    } else {
      // taster losgelassen
      MidiUart.sendNoteOff(pitch);
    }
    tasterStatus = tasterWert;
  }  
}
