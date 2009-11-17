#include <MD.h>

void on16Callback(uint32_t pos) {
  if ((pos % 3) == 0) {
    MD.setTrackParam(3, MODEL_FLTF, random(30, 100));
    MD.triggerTrack(3, 100);
  }
}

void setup() {
  GUI.setLine(GUI.LINE1);
  GUI.put_string_fill("MD POLY SEQ");
  
  MidiClock.mode = MidiClock.EXTERNAL_MIDI;
  MidiClock.transmit = false;
  MidiClock.start();
}
