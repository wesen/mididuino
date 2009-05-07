#include <MD.h>

RangeEncoder trackEncoder(0, 15, "TRK");
RangeEncoder outputEncoder(0, 6, "OUT");

EncoderPage page(&trackEncoder, &outputEncoder);

#define MD_OUTPUT_A 0x0
#define MD_OUTPUT_B 0x1
#define MD_OUTPUT_C 0x2
#define MD_OUTPUT_D 0x3
#define MD_OUTPUT_E 0x4
#define MD_OUTPUT_F 0x5
#define MD_OUTPUT_MAIN 0x6

bool loadedKit = false;

void setTrackRouting(uint8_t track, uint8_t output) {
  uint8_t buf[10] = { 
    0xf0, 0x00, 0x20, 0x3c, 
    0x02, 0x00, 0x5c, 0x00,
    0x00, 0xf7 };
  buf[7] = track;
  buf[8] = output;
  MidiUart.sendRaw(buf, 10);
}

void setup() {
  GUI.setPage(&page);
}

void loop() {
  GUI.updatePage();
 
  if (trackEncoder.hasChanged() && loadedKit) {
    GUI.setLine(GUI.LINE1);
    GUI.put_p_string_at_fill(8, getMachineName(MD.trackModels[trackEncoder.getValue()]));
  }
  
  GUI.update();
}

void onCurrentKitCallback() {
  GUI.flash_strings_fill("LOADED KIT", MD.name);
  GUI.setLine(GUI.LINE1);
  loadedKit = true;
}

void handleGui() {
  if (BUTTON_PRESSED(Buttons.BUTTON1)) {
      loadedKit = false;
      MDSysex.getCurrentKit(onCurrentKitCallback);
    }
    
  if (BUTTON_PRESSED(Buttons.BUTTON2)) {
    setTrackRouting(trackEncoder.getValue(), outputEncoder.getValue());
  }
}
