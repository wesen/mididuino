// Einbinden der MIDI Library
#include <MidiUart.h>
#include <Midi.h>
#include <MD.h>
// EInbinden der MidiClock
#include <MidiClock.h>
MidiClass Midi;

byte mididuino_sysex_data[128];

int ledPin = 2;
int ledStatus = HIGH;

void on16Callback() {
  digitalWrite(ledPin, ledStatus);
  ledStatus = !ledStatus;
}

void setup() {
  MidiClock.mode = MidiClock.EXTERNAL_MIDI;
//  MidiClock.setTempo(100);
  MidiClock.transmit = false;
  MidiClock.start();
  
  MidiClock.setOn16Callback(on16Callback);
  
  pinMode(ledPin, OUTPUT);
  MidiUart.init();
}

int potWert = 100;

void loop() {
  while (MidiUart.avail()) {
    byte c = MidiUart.getc();
    if (c == 0xF8) {
      MidiClock.handleClock();
    }
    Midi.handleByte(c);
  }

  int newPotWert = map(analogRead(0), 0, 1023, 50, 250);
  if (abs(potWert - newPotWert) >= 2) {
    potWert = newPotWert;
    MidiClock.setTempo(potWert);
  }
}
