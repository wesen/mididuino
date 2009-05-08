#include <MidiUart.h>
#include <Midi.h>
#include <MidiClock.h>
MidiClass Midi;

int ledStatus = HIGH;

void on16Callback() {
  digitalWrite(12, ledStatus);
  ledStatus = !ledStatus;
}

void setup() {
  MidiUart.init();
  MidiClock.setOn16Callback(on16Callback);
  MidiClock.setTempo(100);
  MidiClock.mode = MidiClock.INTERNAL_MIDI;
  MidiClock.transmit = true;
  MidiClock.start();
}

void loop() {
}
