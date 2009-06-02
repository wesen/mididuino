#include <MD.h>

void onCurrentKitCallback() {
  GUI.flash_strings_fill("LOADED KIT:", MD.name);
}

RangeEncoder trackEncoder(0, 15, "TRK");
EncoderPage page(&trackEncoder);

void onNoteOnCallback(uint8_t *msg) {
  uint8_t track = trackEncoder.getValue();
  if (MD.isMelodicTrack(track)) {
    MD.sendNoteOn(track, msg[1], msg[2]);
  }
}


void setup() {
  GUI.setPage(&page);
  Midi2.setOnNoteOnCallback(onNoteOnCallback);
}

void loop() {
  GUI.updatePage();
  if (trackEncoder.hasChanged()) {
    uint8_t track = trackEncoder.getValue();
    if (MD.isMelodicTrack(track)) {
      GUI.setLine(GUI.LINE1);
      GUI.put_p_string_at_fill(4, getMachineName(MD.trackModels[track]));
    } else {
      GUI.setLine(GUI.LINE1);
      GUI.put_string_at_fill(4, "XXX");
    }
  }
  GUI.update();
}

void handleGui() {
  if (BUTTON_PRESSED(Buttons.BUTTON1)) {
    MDSysex.getCurrentKit(onCurrentKitCallback);
  }
}
