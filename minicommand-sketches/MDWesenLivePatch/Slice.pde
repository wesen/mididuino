typedef enum {
  REPEAT_2_BARS = 0,
  REPEAT_1_BAR,
  REPEAT_2_BEATS,
  REPEAT_1_BEAT,
  REPEAT_1_8TH,
  REPEAT_1_16TH,
  REPEAT_SPEED_CNT
} 
repeat_speed_type_t;

typedef enum {
  BREAKDOWN_NONE = 0,
  BREAKDOWN_2_BARS,
  BREAKDOWN_4_BARS,
  BREAKDOWN_2_AND_4_BARS,
  BREAKDOWN_CNT
} 
breakdown_type_t;

static const char * repeat_speed_names[REPEAT_SPEED_CNT] = {
  "2 BARS ",
  "1 BAR  ",
  "2 BEATS",
  "1 BEAT ",
  "1 8TH  ",
  "1 16TH "
};

static const uint8_t repeat_speed_mask[REPEAT_SPEED_CNT] = {
  32 - 1,
  16 - 1, 
  8  - 1,
  4  - 1,
  2  - 1,
  1  - 1
};

static const char *breakdown_names[BREAKDOWN_CNT] = {
  "NONE   ",
  "2 BARS ",
  "4 BARS ",
  "2+4 BAR"
};



class BreakdownPage : 
public EncoderPage, public ClockCallback {
public:
  EnumEncoder repeatSpeedEncoder, breakdownEncoder;
  bool supaTriggaActive, restorePlayback, breakdownActive, storedBreakdownActive;
  uint8_t ramP1Track;

  BreakdownPage() : EncoderPage() {
    ramP1Track = 127;
    supaTriggaActive = false;
    restorePlayback = false;
    breakdownActive = false;
    storedBreakdownActive = false;

    repeatSpeedEncoder.initEnumEncoder(repeat_speed_names, REPEAT_SPEED_CNT, "SPD", 0);
    breakdownEncoder.initEnumEncoder(breakdown_names, BREAKDOWN_CNT, "BRK", 0);
    setEncoders(&repeatSpeedEncoder, NULL, &breakdownEncoder);
  }

  virtual bool handleEvent(gui_event_t *event) {
    if (EVENT_PRESSED(event, Buttons.BUTTON1)) {
      storedBreakdownActive = !storedBreakdownActive;
      if (storedBreakdownActive) {
        GUI.flash_p_strings_fill(PSTR("BREAKDOWN ON"), PSTR(""));
      } 
      else {
        GUI.flash_p_strings_fill(PSTR("BREAKDOWN OFF"), PSTR(""));
      }
      return true;
    } 

    return false;
  }

  void on16Callback() {
    if (restorePlayback && !storedBreakdownActive) {
      uint8_t val = (MidiClock.div16th_counter) % 32;
      if ((val % 4) == 0) {
        restorePlayback = false;
        MD.sliceTrack32(ramP1Track, val, 32, true);
        return;
      }
    }
    if (supaTriggaActive) {
      doSupatrigga();
    } 
    else if (breakdownActive || storedBreakdownActive) {
      doBreakdown();
    } 
    else {
      uint8_t val = (MidiClock.div16th_counter) % 32;
      if (val == 0) {
        MD.sliceTrack32(ramP1Track, 0 , 32);
      }
    }
  }

  void doBreakdown() {
    uint8_t val = (MidiClock.div16th_counter) % 64;

    repeat_speed_type_t repeatSpeed = REPEAT_2_BARS;
    breakdown_type_t breakdown = BREAKDOWN_NONE;

    breakdown = (breakdown_type_t)breakdownEncoder.getValue();
    repeatSpeed = (repeat_speed_type_t)repeatSpeedEncoder.getValue();
    switch (breakdown) {
    case BREAKDOWN_2_BARS:
      if (val == 24 || val == 56) {
        MD.sliceTrack32(ramP1Track, 24, 32, true);
        return;
      }
      break;

    case BREAKDOWN_4_BARS:
      if (val == 56) {
        MD.sliceTrack32(ramP1Track, 24, 32, true);
        return;
      }
      break;

    case BREAKDOWN_2_AND_4_BARS:
      if (val == 56) {
        MD.sliceTrack32(ramP1Track, 24, 32, true);
        return;
      } 
      else if (val == 24) {
        MD.sliceTrack32(ramP1Track, 24, 32, true);
        return;
      } 
      else if (val == 28) {
        MD.sliceTrack32(ramP1Track, 24, 32, true);
        return;
      }
      break;

    }

    if ((val % 8) == 0 || ((val & repeat_speed_mask[repeatSpeed]) == 0)) {
      MD.sliceTrack32(ramP1Track, val & repeat_speed_mask[repeatSpeed], 32);
    } 
  }

  void doSupatrigga() {
    uint8_t val = (MidiClock.div16th_counter) % 32;
    if ((val % 4) == 0) {
      uint8_t from = 0, to = 0;
      if (random(100) > 50) {
        from = random(0, 6);
        to = random(from + 2, 8);
      } 
      else {
        from = random(2, 8);
        to = random(0, from - 2);
      }
      MD.sliceTrack32(ramP1Track, from * 4, to * 4);
    }
  }

  virtual void show() {
    breakdownActive = true;
  }

  virtual void hide() {
    breakdownActive = false;
    restorePlayback = true;
  }
  
  void startSupatrigga() {
    supaTriggaActive = true;
  }
  
  void stopSupatrigga() {
    restorePlayback = true;
    supaTriggaActive = false;
  }
};
