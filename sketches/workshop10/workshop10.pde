#include <Sequencer.h>

DrumTrack drumTrack(0b0001000100010001);
DrumTrack drumTrack2(0b001, 3);
DrumTrack drumTrack3(0b10010, 5);

void on16thNoteCallback() {
  if (drumTrack.isHit(MidiClock.div16th_counter))
     MidiUart.sendNoteOn(60, 100);
  if (drumTrack2.isHit(MidiClock.div16th_counter))
     MidiUart.sendNoteOn(62, 100);
  if (drumTrack3.isHit(MidiClock.div16th_counter))
     MidiUart.sendNoteOn(64, 100);
}

void setup() {
  MidiClock.mode = MidiClock.EXTERNAL;
  MidiClock.transmit = false;
  MidiClock.setOn16Callback(on16thNoteCallback);
  MidiClock.start();
}

void handleGui() {
}

void loop() {
  GUI.updatePage();
  GUI.update();
}

