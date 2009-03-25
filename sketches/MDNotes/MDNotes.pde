#include <MD.h>

enum {
  MD_NOTES_STATUS_NORMAL = 0,
  MD_NOTES_STATUS_CONFIG_CHANNEL_WAIT_NOTE,
  MD_NOTES_STATUS_CONFIG_CHANNEL
} mdNotesStatus = MD_NOTES_STATUS_NORMAL;

bool loadedKit = false;

class InputChannel {
  public:
    static const uint8_t NORMAL_CHANNEL_TYPE = 0;
    static const uint8_t POLY_CHANNEL_TYPE   = 1;
    static const uint8_t MULTI_CHANNEL_TYPE  = 2;
    
    uint8_t type;
    uint16_t trackmask;
    uint8_t voices;
    uint8_t last_voice;
    
    InputChannel() {
      type = NORMAL_CHANNEL_TYPE;
      trackmask = 0;
      voices = 0;
      last_voice = 0;
    }
    
    uint8_t getVoices() {
      uint8_t result = 0;
      for (uint8_t i = 0; i < 16; i++) {
        if (IS_BIT_SET(trackmask, i))
          result++;
      }
      return result;
    }
    
    uint8_t getVoiceTrack(uint8_t voice) {
      uint8_t cnt = 0;
      for (uint8_t i = 0; i < 16; i++) {
        if (IS_BIT_SET(trackmask, i)) {
          if (cnt++ == voice)
            return i;
        }
      }
    }
};

InputChannel inputChannels[16];

void onCurrentKitCallback() {
  loadedKit = true;
}

void handleGuiSysex() {
  if (BUTTON_PRESSED(Buttons.BUTTON1) && !isConfigPageActive()) {
    loadedKit = false;
    MDSysex.getCurrentKit(onCurrentKitCallback);
  }
}

void setup() {
  setupMidi();
  setupPages();
}

void loop() {
  GUI.updatePage();
  
//  if (!MidiUart.avail())
    GUI.update();
}

void handleGui() {
  handleGuiSysex();
  handleGuiPages();
}
