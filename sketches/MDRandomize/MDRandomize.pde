#include <MD.h>
#include <GUI.h>

bool loadedKit = false;

uint8_t undoParams[24] = {
  0
};

#define SELECT_FILTER 0
#define SELECT_AMD    1
#define SELECT_EQ     2
#define SELECT_EFFECT 3
#define SELECT_LOWSYN 4
#define SELECT_UPSYN  5
#define SELECT_SYN    6
#define SELECT_LFO    7
#define SELECT_SENDS  8
#define SELECT_DIST   9
#define SELECT_FX_LOWSYN 10
#define SELECT_FX_SYN    11
#define SELECT_ALL       12

PROGMEM char selectNames[13][7] = {
  "FILTER",
  "AMD   ",
  "EQ    ",
  "EFFECT",
  "LOWSYN",
  "UP SYN",
  "SYN   ",
  "LFO   ",
  "SENDS ",
  "DIST  ",
  "FX LOW",
  "FX SYN",
  "ALL   "
};

class RandomizePage : public Page {
public:
  RangeEncoder trackEncoder;
  RangeEncoder amtEncoder;
  RangeEncoder selectEncoder;

  RandomizePage() : trackEncoder(0, 15, "TRK"), 
                    amtEncoder(0, 128, "AMT"),
                    selectEncoder(0, 12, "AMT") {
  }
  
  virtual void update() {
    uint8_t tmp = SREG;
    cli();
    selectEncoder.update(&Encoders.encoders[0]);
    amtEncoder.update(&Encoders.encoders[2]);
    trackEncoder.update(&Encoders.encoders[3]);
    Encoders.clearEncoders();
    SREG = tmp;
  }
  
  virtual void handle() {
    if (trackEncoder.hasChanged()) {
      displayTrack();
      m_memcpy(undoParams, MD.trackParams[trackEncoder.getValue()], 24);
      if (loadedKit) {
        GUI.setLine(GUI.LINE2);
        GUI.flash_p_string_at(9, getMachineName(MD.trackModels[trackEncoder.getValue()]));
      }
    }
    trackEncoder.checkHandle();
    if (selectEncoder.hasChanged()) {
      displaySelect();
    }
    selectEncoder.checkHandle();
    if (amtEncoder.hasChanged()) {
      GUI.setLine(GUI.LINE2);
      GUI.put_value_at(11, amtEncoder.getValue());
    }
    amtEncoder.checkHandle();
    
  }
  
  void displaySelect() {
    GUI.setLine(GUI.LINE2);
    GUI.put_p_string(selectNames[selectEncoder.getValue()]);
  }

  void displayKit() {
    GUI.setLine(GUI.LINE1);
    if (!loadedKit) {
      GUI.put_p_string(PSTR("NO KIT     "));
    } else {
      GUI.put_string(MD.name);
    }
  }
  
  void displayTrack() {
    GUI.setLine(GUI.LINE1);
    GUI.put_value_at(13, trackEncoder.getValue());
  }
  
  virtual void display(bool redisplay = false) {
    if (redisplay) {
      displayTrack();
      displayKit();
      displaySelect();
    }
  }
  
};

RandomizePage randomizePage;

void setup() {
  GUI.setPage(&randomizePage);
  GUI.setLine(GUI.LINE1);
  GUI.flash_p_string_fill(PSTR("MD RANDOMIZE"));
  GUI.setLine(GUI.LINE2);
  GUI.flash_p_string_fill(PSTR(""));
  setupMDSysex();
}

uint8_t lastOverflow = 0;
void loop() {
  GUI.updatePage();
  GUI.update();
   if (lastOverflow != MidiUart.rxRb.overflow) {
     lastOverflow = MidiUart.rxRb.overflow;
     LCD.line2();
     LCD.putnumber(MidiUart.rxRb.overflow);
   }
}

void randomize() {
}

void undo() {
}

void handleGui() {
  handleGuiSysex();
  
  if (BUTTON_PRESSED(Buttons.BUTTON2)) {
    randomize();
  }
  if (BUTTON_PRESSED(Buttons.BUTTON3)) {
    GUI.setLine(GUI.LINE1);
    GUI.flash_p_string_fill(PSTR("UNDO"));
    undo();
  }
}
