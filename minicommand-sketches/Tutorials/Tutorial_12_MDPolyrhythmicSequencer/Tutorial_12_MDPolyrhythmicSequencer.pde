#include <MidiUart.h>
#include <Midi.h>
#include <MidiClock.h>
#include <MD.h>

void on16Callback() {
  if ((MidiClock.div16th_counter % 3) == 0) {
    MD.setTrackParam(3, MODEL_FLTF, random(30, 100));
    MD.triggerTrack(3, 100);
  }
}

void setup() {
  MidiClock.mode = MidiClock.EXTERNAL_MIDI;
  MidiClock.setOn16Callback(on16Callback);
  MidiClock.transmit = false;
  MidiClock.start();
}

void loop() {
}

void handleGui() {
}
