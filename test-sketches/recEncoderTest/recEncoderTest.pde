#include <MD.h>

CCEncoder ccEnc(0, 0, "CC");
RecordingEncoder<64> recEnc(&ccEnc);
EncoderPage page(&recEnc);

void on32Callback() {
  recEnc.playback(MidiClock.div32th_counter);
}

void onCCCallback(uint8_t *msg) {
  GUI.setLine(GUI.LINE2);
  GUI.flash_put_value(0, msg[1]);
  GUI.flash_put_value(1, msg[2]);
}

void onCCCallback2(uint8_t *msg) {
  GUI.setLine(GUI.LINE1);
  GUI.flash_put_value(0, msg[1]);
  GUI.flash_put_value(1, msg[2]);
}


void setup() {
  GUI.setPage(&page);
  MidiClock.mode = MidiClock.EXTERNAL;
  MidiClock.setOn32Callback(on32Callback);
  MidiClock.start();
}

void loop() {
  GUI.updatePage();
  while (!EventRB.isEmpty()) {
    gui_event_t event;
    EventRB.getp(&event);

    if (EVENT_PRESSED(event, Buttons.BUTTON2)) {
      recEnc.startRecording();
    }
    if (EVENT_RELEASED(event, Buttons.BUTTON2)) {
      recEnc.stopRecording();
    }
    if (EVENT_PRESSED(event, Buttons.BUTTON3)) {
      recEnc.clearRecording();
    }
    if (EVENT_PRESSED(event, Buttons.BUTTON4)) {
      Midi.addOnControlChangeCallback(onCCCallback);
    }
    if (EVENT_RELEASED(event, Buttons.BUTTON4)) {
      Midi.removeOnControlChangeCallback(onCCCallback);
    }
    if (EVENT_PRESSED(event, Buttons.BUTTON1)) {
      Midi.addOnControlChangeCallback(onCCCallback2);
    }
    if (EVENT_RELEASED(event, Buttons.BUTTON1)) {
      Midi.removeOnControlChangeCallback(onCCCallback2);
    }
    if (EVENT_PRESSED(event, Buttons.ENCODER2)) {
      Midi.setOnControlChangeCallback(onCCCallback);
    }
    if (EVENT_PRESSED(event, Buttons.ENCODER3)) {
      Midi.setOnControlChangeCallback(onCCCallback2);
    }
  }
  GUI.update();
}

void handleGui() {
  pollEventGUI();
}
