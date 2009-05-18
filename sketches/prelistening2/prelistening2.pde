#include <MD.h>

RangeEncoder trackEncoder(0, 15, "TRK");
RangeEncoder outputEncoder(0, 6, "OUT");

EncoderPage page(&trackEncoder); // , &outputEncoder);

uint8_t trackPrelisten[16] = { 0 };

#define MD_OUTPUT_A 0x0
#define MD_OUTPUT_B 0x1
#define MD_OUTPUT_C 0x2
#define MD_OUTPUT_D 0x3
#define MD_OUTPUT_E 0x4
#define MD_OUTPUT_F 0x5
#define MD_OUTPUT_MAIN 0x6

bool loadedKit = false;

void displayPrelisten() {
  char data[17];
  for (int i = 0; i < 16; i++) {
    if (trackPrelisten[i]) {
      data[i] = '-';
    } else {
     data[i] =  'X';
    }
  }
  data[16] = '\0';
  GUI.setLine(GUI.LINE1);
  GUI.put_string(data);
}

void setTrackRouting(uint8_t track, uint8_t output) {
  uint8_t buf[10] = { 
    0xf0, 0x00, 0x20, 0x3c, 
    0x02, 0x00, 0x5c, 0x00,
    0x00, 0xf7 };
  buf[7] = track;
  buf[8] = output;
  MidiUart.sendRaw(buf, 10);
}

void togglePrelistenTrack(uint8_t track) {
  if (trackPrelisten[track] == 0) {
    trackPrelisten[track] = 1;
    setTrackRouting(track, MD_OUTPUT_C);
    GUI.setLine(GUI.LINE1);
    GUI.flash_string("PRELISTEN ");
    GUI.flash_put_value_at(10, track);
  } else {
    trackPrelisten[track] = 0;
    setTrackRouting(track, MD_OUTPUT_MAIN);
    GUI.setLine(GUI.LINE1);
    GUI.flash_string("MAIN OUT ");
    GUI.flash_put_value_at(10, track);
  }  
}

void setup() {
  GUI.setPage(&page);
}

void onNoteOnCallbackMD(uint8_t *msg) {
}

void loop() {
  GUI.updatePage();
 
  if (trackEncoder.hasChanged() && loadedKit) {
    GUI.setLine(GUI.LINE1);
    GUI.put_p_string_at_fill(8, getMachineName(MD.trackModels[trackEncoder.getValue()]));
  }
  
  GUI.update();
  Midi.setOnNoteOnCallback(onNoteOnCallbackMD);
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
//    setTrackRouting(trackEncoder.getValue(), outputEncoder.getValue());
    togglePrelistenTrack(trackEncoder.getValue());
  }
  
  if (BUTTON_PRESSED(Buttons.BUTTON3)) {
    displayPrelisten();
  } else if (BUTTON_RELEASED(Buttons.BUTTON3)) {
    if (GUI.page != NULL) {
      GUI.page->display(true);
    }
  }
}
