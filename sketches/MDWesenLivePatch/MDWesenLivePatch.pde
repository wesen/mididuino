#include <MD.h>

bool supaTriggaActive = false;
bool breakdownActive = false;

RangeEncoder flfEncoder(0, 127, "FLF", 0);
RangeEncoder flwEncoder(0, 127, "FLW", 127);
RangeEncoder fbEncoder(0, 127, "FB", 32);
RangeEncoder levEncoder(0, 127, "LEV", 100);
EncoderPage page(&flfEncoder, &flwEncoder, &fbEncoder, &levEncoder);

RangeEncoder timEncoder(0, 127, "TIM", 24);
RangeEncoder frqEncoder(0, 127, "FRQ", 0);
RangeEncoder modEncoder(0, 127, "MOD", 0);
EncoderPage page2(&timEncoder, &frqEncoder, &modEncoder, &fbEncoder);

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

#define RAM_R1_TRACK 14
#define RAM_P1_TRACK 15

void doBreakdown() {
  uint8_t val = MidiClock.div16th_counter % 32;
  repeat_speed_type_t speed = (repeat_speed_type_t)repeatSpeedEncoder.getValue();
  breakdown_type_t breakdown = (breakdown_type_t)breakdownEncoder.getValue();  
  if ((val % 8) == 0) {  
    MD.sliceTrack32(RAM_P1_TRACK, val, val + 8);
  }
}

void doSupatrigga() {
  uint8_t val = MidiClock.div16th_counter % 32;
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
void on16Callback() {
  if (supaTriggaActive) {
    doSupatrigga();
  } else if (breakdownActive) {
    doBreakdown();
  } else {
    uint8_t val = MidiClock.div16th_counter % 32;
    if ((val % 8) == 0) {
      MD.sliceTrack32(RAM_P1_TRACK, val, val + 8);
    }
  }
}

void setup() {
  GUI.setPage(&page);
  
  MidiClock.mode = MidiClock.EXTERNAL;
  MidiClock.transmit = false;
  MidiClock.setOn16Callback(on16Callback);
  MidiClock.start();
}

void loop() {
  GUI.updatePage();
  
  if (flfEncoder.hasChanged())
    MD.setEchoParam(MD_ECHO_FLTF, flfEncoder.getValue());
  if (flwEncoder.hasChanged())
    MD.setEchoParam(MD_ECHO_FLTW, flwEncoder.getValue());
  if (fbEncoder.hasChanged())
    MD.setEchoParam(MD_ECHO_FB, fbEncoder.getValue());
  if (levEncoder.hasChanged())
    MD.setEchoParam(MD_ECHO_LEV, levEncoder.getValue());
  if (timEncoder.hasChanged())
    MD.setEchoParam(MD_ECHO_TIME, timEncoder.getValue());
  if (frqEncoder.hasChanged())
    MD.setEchoParam(MD_ECHO_MFRQ, frqEncoder.getValue());
  if (modEncoder.hasChanged())
    MD.setEchoParam(MD_ECHO_MOD, modEncoder.getValue());
  
  GUI.update();
}

Page *previousPage = NULL;

void handleGui() {
  if (BUTTON_PRESSED(Buttons.BUTTON1)) {
    GUI.setPage(&page);
  } else if (BUTTON_PRESSED(Buttons.BUTTON2)) {
    GUI.setPage(&page2);
  }
  if (BUTTON_PRESSED(Buttons.BUTTON3)) {
    supaTriggaActive = true;
  } else if (BUTTON_RELEASED(Buttons.BUTTON3)) {
    supaTriggaActive = false;
  }
  
  if (BUTTON_PRESSED(Buttons.BUTTON4)) {
    previousPage = GUI.page;
    GUI.setPage(&breakPage);
    breakdownActive = true;
  } else if (BUTTON_RELEASED(Buttons.BUTTON4)) {
    if (previousPage != NULL) {
      GUI.setPage(previousPage);
    }
    breakdownActive = false;
  }
}
