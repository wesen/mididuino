// fltf fltw von r1
// del-send von r1
// srr von r1
// fb-send von delay

// zweite seite delay

#include <MD.h>

bool supaTriggaActive = false;
bool restorePlayback = false;
bool breakdownActive = false;
bool storedBreakdownActive = false;
bool ramP1Active = false;

#define RAM_R1_TRACK 14
#define RAM_P1_TRACK 15

RangeEncoder flfEncoder(0, 127, "FLF", 0);
RangeEncoder flwEncoder(0, 127, "FLW", 127);
RangeEncoder fbEncoder(0, 127, "FB", 32);
RangeEncoder levEncoder(0, 127, "LEV", 100);
EncoderPage page(&flfEncoder, &flwEncoder, &fbEncoder, &levEncoder);

MDEncoder pFlfEncoder(RAM_P1_TRACK, MODEL_FLTF, "FLF", 0);
MDEncoder pFlwEncoder(RAM_P1_TRACK, MODEL_FLTW, "FLW", 127);
MDEncoder pSrrEncoder(RAM_P1_TRACK, MODEL_SRR, "SRR", 0);
MDEncoder pVolEncoder(RAM_P1_TRACK, MODEL_VOL, "DEL", 100);
EncoderPage page4(&pFlfEncoder, &pFlwEncoder, &pSrrEncoder, &pVolEncoder);

MDEncoder rFlfEncoder(RAM_P1_TRACK, MODEL_FLTF, "FLF", 0);
MDEncoder rFlwEncoder(RAM_P1_TRACK, MODEL_FLTW, "FLW", 127);
MDEncoder rSrrEncoder(RAM_P1_TRACK, MODEL_SRR, "SRR", 0);
MDEncoder rVolEncoder(RAM_P1_TRACK, MODEL_VOL, "DEL", 100);
EncoderPage page5(&rFlfEncoder, &rFlwEncoder, &rSrrEncoder, &rVolEncoder);

typedef enum {
  REPEAT_2_BARS = 0,
  REPEAT_1_BAR,
  REPEAT_2_BEATS,
  REPEAT_1_BEAT,
  REPEAT_1_8TH,
  REPEAT_1_16TH,
  REPEAT_SPEED_CNT
} repeat_speed_type_t;

char repeat_speed_names[REPEAT_SPEED_CNT][8] PROGMEM = {
  "2 BARS ",
  "1 BAR  ",
  "2 BEATS",
  "1 BEAT ",
  "1 8TH  ",
  "1 16TH "
};

uint8_t repeat_speed_mask[REPEAT_SPEED_CNT] = {
  32 - 1,
  16 - 1, 
  8  - 1,
  4  - 1,
  2  - 1,
  1  - 1
};

RangeEncoder repeatSpeedEncoder(0, REPEAT_SPEED_CNT-1, "RPT", 0);

typedef enum {
  BREAKDOWN_NONE = 0,
  BREAKDOWN_2_BARS,
  BREAKDOWN_4_BARS,
  BREAKDOWN_2_AND_4_BARS,
  BREAKDOWN_CNT
} breakdown_type_t;

char breakdown_names[BREAKDOWN_CNT][8] PROGMEM = {
  "NONE   ",
  "2 BARS ",
  "4 BARS ",
  "2+4 BAR"
};
RangeEncoder breakdownEncoder(0, BREAKDOWN_CNT - 1, "BRK", 0);

repeat_speed_type_t repeatSpeed = REPEAT_2_BARS;
breakdown_type_t breakdown = BREAKDOWN_NONE;

class BreakPage : public EncoderPage {
  public:
  BreakPage() : EncoderPage(&repeatSpeedEncoder, NULL, &breakdownEncoder) {    
  }
  
  virtual void display(bool redisplay = false) {
    EncoderPage::display(redisplay);
    if (redisplay || breakdownEncoder.hasChanged()) {
      displayBreakdown();
    }
    if (redisplay || repeatSpeedEncoder.hasChanged()) {
      displayRepeatSpeed();
    }
    if (breakdownEncoder.hasChanged()) {
      breakdown = (breakdown_type_t)breakdownEncoder.getValue();
    }
    if (repeatSpeedEncoder.hasChanged()) {
      repeatSpeed = (repeat_speed_type_t)repeatSpeedEncoder.getValue();
    }
  }
  
  void displayBreakdown() {
    GUI.setLine(GUI.LINE2);
    GUI.put_p_string_at(8, breakdown_names[breakdownEncoder.getValue()]);
  }
  
  void displayRepeatSpeed() {
    GUI.setLine(GUI.LINE2);
    GUI.put_p_string_at(0, repeat_speed_names[repeatSpeedEncoder.getValue()]);
  }
};

BreakPage breakPage;

void doBreakdown() {
  uint8_t val = (MidiClock.div16th_counter) % 64;

  switch (breakdown) {
  case BREAKDOWN_2_BARS:
     if (val == 24 || val == 56) {
       MD.sliceTrack32(RAM_P1_TRACK, 24, 32, true);
       return;
     }
  break;
  
  case BREAKDOWN_4_BARS:
     if (val == 56) {
       MD.sliceTrack32(RAM_P1_TRACK, 24, 32, true);
       return;
     }
  break;
  
  case BREAKDOWN_2_AND_4_BARS:
     if (val == 56) {
       MD.sliceTrack32(RAM_P1_TRACK, 24, 32, true);
       return;
     } else if (val == 24) {
       MD.sliceTrack32(RAM_P1_TRACK, 24, 32, true);
       return;
     } else if (val == 28) {
       MD.sliceTrack32(RAM_P1_TRACK, 24, 32, true);
       return;
     }
  break;
  
  }
 
  if ((val % 8) == 0 || ((val & repeat_speed_mask[repeatSpeed]) == 0)) {
    MD.sliceTrack32(RAM_P1_TRACK, val & repeat_speed_mask[repeatSpeed], 32);
  } 
}

void doSupatrigga() {
  uint8_t val = (MidiClock.div16th_counter) % 32;
  if ((val % 4) == 0) {
    uint8_t from = 0, to = 0;
    if (random(100) > 50) {
      from = random(0, 6);
      to = random(from + 2, 8);
    } else {
      from = random(2, 8);
      to = random(0, from - 2);
    }
    MD.sliceTrack32(RAM_P1_TRACK, from * 4, to * 4);
  }
}

void on32Callback() {
//  GUI.flash_put_value(0, MidiClock.div32th_counter);
}

void on16Callback() {
  if (!ramP1Active)
    return;
  if (restorePlayback && !storedBreakdownActive) {
    uint8_t val = (MidiClock.div16th_counter) % 32;
    if ((val % 4) == 0) {
      restorePlayback = false;
      MD.sliceTrack32(RAM_P1_TRACK, val, 32, true);
    }
    
  }
  if (supaTriggaActive) {
    doSupatrigga();
  } else if (breakdownActive || storedBreakdownActive) {
    doBreakdown();
  } else {
    uint8_t val = (MidiClock.div16th_counter) % 32;
    if (val == 0) 
      MD.sliceTrack32(RAM_P1_TRACK, 0 , 32);
/*
if ((val % 8) == 0) {
      if (BUTTON_DOWN(Buttons.BUTTON1)) {
        MD.sliceTrack32(RAM_P1_TRACK, val, val + 8, false);
      } else {
        MD.sliceTrack32(RAM_P1_TRACK, val, val + 8, true);
      }
    }
    */
  }
}

void setup() {
  GUI.setPage(&page4);
  MD.baseChannel = 4;

  MidiClock.mode = MidiClock.EXTERNAL;
  MidiClock.transmit = false;
  MidiClock.setOn16Callback(on16Callback);
//  MidiClock.setOn32Callback(on32Callback);
  MidiClock.start();
}

void loop() {
  GUI.updatePage();
  if (MidiClock.state == MidiClock.STARTED)
    setLed();
  else
    clearLed();
  
  if (flfEncoder.hasChanged())
    MD.setEchoParam(MD_ECHO_FLTF, flfEncoder.getValue());
  if (flwEncoder.hasChanged())
    MD.setEchoParam(MD_ECHO_FLTW, flwEncoder.getValue());
  if (fbEncoder.hasChanged())
    MD.setEchoParam(MD_ECHO_FB, fbEncoder.getValue());
  if (levEncoder.hasChanged())
    MD.setEchoParam(MD_ECHO_LEV, levEncoder.getValue());

  if (rFlfEncoder.hasChanged())
    MD.setTrackParam(RAM_R1_TRACK, MODEL_FLTF, rFlfEncoder.getValue());
  if (rFlwEncoder.hasChanged())
    MD.setTrackParam(RAM_R1_TRACK, MODEL_FLTW, rFlwEncoder.getValue());
  if (rSrrEncoder.hasChanged())
    MD.setTrackParam(RAM_R1_TRACK, MODEL_SRR, rSrrEncoder.getValue());
  if (rVolEncoder.hasChanged())
    MD.setTrackParam(RAM_R1_TRACK, MODEL_DEL, rVolEncoder.getValue());

  if (pFlfEncoder.hasChanged())
    MD.setTrackParam(RAM_P1_TRACK, MODEL_FLTF, pFlfEncoder.getValue());
  if (pFlwEncoder.hasChanged())
    MD.setTrackParam(RAM_P1_TRACK, MODEL_FLTW, pFlwEncoder.getValue());
  if (pSrrEncoder.hasChanged())
    MD.setTrackParam(RAM_P1_TRACK, MODEL_SRR, pSrrEncoder.getValue());
  if (pVolEncoder.hasChanged())
    MD.setTrackParam(RAM_P1_TRACK, MODEL_DEL, pVolEncoder.getValue());

  GUI.update();
}

Page *previousPage = NULL;

enum {
  PAGE_NOTHING = 0,
  PAGE_BUTTON1,
  PAGE_BUTTON4
} pageMode = PAGE_NOTHING;

void handleGui() {
again:
  switch (pageMode) {
    case PAGE_NOTHING:
    if (BUTTON_PRESSED(Buttons.BUTTON4)) {
      previousPage = GUI.page;
      GUI.setPage(&breakPage);
      breakdownActive = true;
      pageMode = PAGE_BUTTON4;
      goto again;
    }
    if (BUTTON_PRESSED(Buttons.BUTTON1)) {
      if (GUI.page == &page5) {
        GUI.flash_strings_fill("RAM P1 CTRL", "", 300);
        GUI.setPage(&page4);
      } else {
        GUI.flash_strings_fill("RAM R1 CTRL", "", 300);
        GUI.setPage(&page5);
      }
    }
    if (BUTTON_PRESSED(Buttons.BUTTON2)) {
      GUI.flash_strings_fill("DELAY CTRL", "", 300);
      GUI.setPage(&page);
    }


    if (BUTTON_PRESSED(Buttons.BUTTON3)) {
      GUI.setLine(GUI.LINE1);
      GUI.put_string_fill("SUPATRIGGA");
      GUI.setLine(GUI.LINE2);
      GUI.clearLine();
      supaTriggaActive = true;
    } else if (BUTTON_RELEASED(Buttons.BUTTON3)) {
      restorePlayback = true;
      GUI.page->display(true);
      supaTriggaActive = false;
    } 
    
  
    break;
    
    case PAGE_BUTTON4:
    if (BUTTON_RELEASED(Buttons.BUTTON4)) {
      if (previousPage != NULL) {
        GUI.setPage(previousPage);
      }
      breakdownActive = false;
      restorePlayback = true;
      pageMode = PAGE_NOTHING;
      goto again;
    }
    if (BUTTON_PRESSED(Buttons.BUTTON1)) {
      storedBreakdownActive = !storedBreakdownActive;
      if (storedBreakdownActive) {
        GUI.flash_p_strings_fill(PSTR("BREAKDOWN ON"), PSTR(""));
      } else {
        GUI.flash_p_strings_fill(PSTR("BREAKDOWN OFF"), PSTR(""));
      }
    }
    if (BUTTON_PRESSED(Buttons.BUTTON3)) {
      ramP1Active = !ramP1Active;
      if (ramP1Active) {
        GUI.flash_p_strings_fill(PSTR("P1 CTRL ON"), PSTR(""));
      } else {
        GUI.flash_p_strings_fill(PSTR("P1 CTRL OFF"), PSTR(""));
      }
    }
    break;
  }
}
