#ifndef MDBREAKDOWN_H__
#define MDBREAKDOWN_H__

#include "Midi.h"

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
}  breakdown_type_t;

class MDBreakdown : public ClockCallback {
 public:
  static const char *repeatSpeedNames[REPEAT_SPEED_CNT];
  static const uint8_t repeatSpeedMask[REPEAT_SPEED_CNT];
  static const char *breakdownNames[BREAKDOWN_CNT];
  
  bool supaTriggaActive, restorePlayback, breakdownActive, storedBreakdownActive;
  uint8_t ramP1Track;

  repeat_speed_type_t repeatSpeed;
  breakdown_type_t breakdown;

  MDBreakdown() {
    supaTriggaActive = false;
    restorePlayback = false;
    breakdownActive = false;
    storedBreakdownActive = false;
    breakdown = BREAKDOWN_NONE;
    repeatSpeed = REPEAT_2_BARS;
    ramP1Track = 127;
  }

  void setup();
  
  void startBreakdown();
  void stopBreakdown();
  void on16Callback();
  void doBreakdown();

  void startSupatrigga();
  void stopSupatrigga();
  void doSupatrigga();
};

extern MDBreakdown mdBreakdown;

#endif /* MDBREAKDOWN_H__ */
