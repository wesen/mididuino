#include <MNM.h>

void setup() {
    
}


class DebugCallbacks : public MNMCallback {
public:
  DebugCallbacks() {
  }

  void onKitMessage() {
    MNM.kit.fromSysex(MidiSysex.data + 5, MidiSysex.recordLen - 5);
//    printf("got kit \"%s\"\n", kit.name);
  }
};

void loop() {
  uint8_t track = MNM.getCurrentTrack(1000);
  GUI.flash_p_strings_fill(PSTR("MNM KIT"), PSTR(""));
  GUI.setLine(GUI.LINE2);
  GUI.flash_put_value(0, track);
  delay(500);
}
