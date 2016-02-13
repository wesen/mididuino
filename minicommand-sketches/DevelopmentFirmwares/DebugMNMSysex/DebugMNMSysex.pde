#include <MNM.h>

class DebugCallbacks : public MNMCallback {
public:
  DebugCallbacks() {
  }

  void onKitMessage() {
    MNM.kit.fromSysex(MidiSysex.data + 5, MidiSysex.recordLen - 5);
    GUI.flash_strings_fill("KIT", MNM.kit.name);
//    printf("got kit \"%s\"\n", kit.name);
  }
};

DebugCallbacks debugCallbacks;

void setup() {
    MNMSysexListener.setup();
    MNMSysexListener.addOnKitMessageCallback(&debugCallbacks, (mnm_callback_ptr_t)&DebugCallbacks::onKitMessage);
}

bool handleEvent(gui_event_t *evt) {
  if (EVENT_PRESSED(evt, Buttons.BUTTON1)) {
    uint8_t currentKit = MNM.getCurrentKit();
    GUI.flash_p_strings_fill(PSTR("MNM KIT"), PSTR(""));
    GUI.setLine(GUI.LINE2);
    GUI.flash_put_value(0, currentKit);
    MNM.requestKit(currentKit);
  }
}

void loop() {
}
