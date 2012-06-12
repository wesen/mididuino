#include <GUI.h>

class ClockCallbackTest : public ClockCallback {
  public:
  ClockCallbackTest() {
  }
  
  void onStartCallback() {
    GUI.flash_strings_fill("RECEIVED", "START");
  }
  
  void onStopCallback() {
    GUI.flash_strings_fill("RECEIVED", "STOP");
  }
  
  void onContinueCallback() {
    GUI.flash_strings_fill("RECEIVED", "CONTINUE");
  }
  
  void on32Callback(uint32_t pos) {
    if (BUTTON_DOWN(Buttons.BUTTON1)) {
      delay(100);
    }
    GUI.setLine(GUI.LINE2);
    GUI.put_value16(2, pos & 0xFFFF);
  }
};

ClockCallbackTest cbTest;

void setup() {
  GUI.flash_strings_fill("CLOCK CALLBACK", "TEST FIRMWARE");
//  MidiClock.addOn16Callback(&cbTest, (midi_clock_callback_ptr_t)&ClockCallbackTest::on16Callback);
  MidiClock.addOn32Callback(&cbTest, (midi_clock_callback_ptr_t)&ClockCallbackTest::on32Callback);
  MidiClock.addOnStartCallback(&cbTest, (midi_clock_callback_ptr0_t)&ClockCallbackTest::onStartCallback);
  MidiClock.addOnStopCallback(&cbTest, (midi_clock_callback_ptr0_t)&ClockCallbackTest::onStopCallback);
  MidiClock.addOnContinueCallback(&cbTest, (midi_clock_callback_ptr0_t)&ClockCallbackTest::onContinueCallback);
  MidiClock.useImmediateClock = true;
}


