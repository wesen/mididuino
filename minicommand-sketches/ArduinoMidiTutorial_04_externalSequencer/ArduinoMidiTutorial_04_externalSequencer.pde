#include <MidiUart.h>
#include <Midi.h>
#include <MidiClock.h>
MidiClass Midi;

int ledPin = 13;

void on16Callback() {
  if ((MidiClock.div16th_counter % 4) == 0) {
//    digitalWrite(ledPin, HIGH);
    MidiUart.sendNoteOn(100, 100);
  } else if ((MidiClock.div16th_counter % 4) == 2) {
//    digitalWrite(ledPin, LOW);
    MidiUart.sendNoteOff(100);
  }

}

void setup() {
  pinMode(ledPin, OUTPUT);
  MidiUart.init();
  MidiClock.mode = MidiClock.EXTERNAL_MIDI;
  MidiClock.transmit = false;
  MidiClock.setOn16Callback(on16Callback);
  MidiClock.start();
}

void loop() {
  while (MidiUart.avail()) {
    byte b = MidiUart.getc();
      Midi.handleByte(b);
  }
}
