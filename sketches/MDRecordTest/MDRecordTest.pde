#include <MD.h>


bool recording = false;
bool triggerRecording = false;
int recordStart = 0;

void on16Callback() {
  if (triggerRecording && (MidiClock.div16th_counter % 16) == 0) {
    triggerRecording = false;
    recording = true;
    recordStart = MidiClock.div16th_counter;
  }
  if (recording) {
    int pos = MidiClock.div16th_counter - recordStart;
    if (pos >= 32) {
      recording = false;
      return;
    } 
    
    if (pos >= 15) {
      if (((pos + 1) % 4) == 0) {
        MD.setTrackParam(0, 0, (pos - 15) * 5);
      }
    }

    if (pos < 16) {
      if ((pos % 4) == 0) {
        MD.triggerTrack(0, 100);
      }
    }
      GUI.put_value(0, pos);
  }
}

void setup() {
  MidiClock.mode = MidiClock.EXTERNAL;
  MidiClock.transmit = false;
  MidiClock.setOn16Callback(on16Callback);
  MidiClock.start();
}

void loop() {
  GUI.updatePage();
  
  
  GUI.update();
}

void handleGui() {
  if (BUTTON_PRESSED(Buttons.BUTTON1)) {
    triggerRecording = true;
  }
}
