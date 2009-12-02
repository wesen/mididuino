#include <MD.h>

void on16Callback() {
  if ((MidiClock.div16th_counter % 4) == 0) {
    MD.sliceTrack32(15, MidiClock.div16th_counter % 32, 32, true);
    MD.triggerTrack(2, 100);
  }
  GUI.setLine(GUI.LINE1);
  GUI.put_value(3, (uint8_t)(MidiClock.div16th_counter % 64));
}

void on96Callback() {
}

void setup() {
//  MidiClock.mode = MidiClock.EXTERNAL_UART2;
   MidiClock.mode = MidiClock.EXTERNAL_MIDI;
  MidiClock.start();
  MDTask.setup();
  GUI.addTask(&MDTask);
//  MidiClock.transmit = true;
  //MidiClock.setOn96Callback(on96Callback);
  MidiClock.setOn16Callback(on16Callback);
}
  
void loop() {
} 
