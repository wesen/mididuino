#ifndef MDBREAKDOWN_PAGE_H__
#define MDBREAKDOWN_PAGE_H__

#include <MD.h>
#include <MDBreakdown.h>

class BreakdownPage : public EncoderPage {
 public:
    EnumEncoder repeatSpeedEncoder, breakdownEncoder;

  BreakdownPage() : EncoderPage() {
    repeatSpeedEncoder.initEnumEncoder(MDBreakdown::repeatSpeedNames, REPEAT_SPEED_CNT, "SPD", 0);
    breakdownEncoder.initEnumEncoder(MDBreakdown::breakdownNames, BREAKDOWN_CNT, "BRK", 0);
    encoders[0] = &repeatSpeedEncoder;
    encoders[2] = &breakdownEncoder;
  }
  
  virtual void setup() {
    mdBreakdown.setup();
  }
  
  virtual void loop() {
    if (repeatSpeedEncoder.hasChanged()) {
      mdBreakdown.repeatSpeed = (repeat_speed_type_t)repeatSpeedEncoder.getValue();
    }
    if (breakdownEncoder.hasChanged()) {
      mdBreakdown.breakdown = (breakdown_type_t)breakdownEncoder.getValue();
    }
  }

  virtual bool handleEvent(gui_event_t *event) {
    if (EVENT_PRESSED(event, Buttons.BUTTON1)) {
      mdBreakdown.storedBreakdownActive = !mdBreakdown.storedBreakdownActive;
      if (mdBreakdown.storedBreakdownActive) {
        GUI.flash_p_strings_fill(PSTR("BREAKDOWN ON"), PSTR(""));
      } 
      else {
        GUI.flash_p_strings_fill(PSTR("BREAKDOWN OFF"), PSTR(""));
      }
      return true;
    } 

    return false;
  }

  virtual void show() {
    mdBreakdown.startBreakdown();
  }

  virtual void hide() {
    mdBreakdown.stopBreakdown();
  }
};


#endif /* MDBREAKDOWN_PAGE_H__ */
