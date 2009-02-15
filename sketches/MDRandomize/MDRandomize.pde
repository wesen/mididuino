#include <MD.h>

bool loadedKit = false;

void onControlChangeCallback(uint8_t *msg) {
  uint8_t channel = MIDI_VOICE_CHANNEL(msg[0]);
  uint8_t track, param;
  MD.parseCC(channel, msg[1], &track, &param);
  if (track != 255) {
    MD.trackParams[track][param] = msg[2];
  }
}

void setup() {
  GUI.flash_p_string_clear(PSTR("MD RANDOMIZE"));
  setupPages();
  Midi.setOnControlChangeCallback(onControlChangeCallback);
}

void loop() {
  GUI.updatePage();
  GUI.update();
}

void handleGui() {
  if (BUTTON_PRESSED(Buttons.BUTTON1)) {
    loadedKit = false;
    MDSysex.getCurrentKit(onCurrentKitCallback);
  }
  handleGuiPages();  
}
