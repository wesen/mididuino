#include <CppUnitLite2.h>
#include <TestResultStdErr.h>

/*
 * use:
 * - CHECK (condition)
 * - CHECK_EQUAL(expected, actual)
 */

#include <GUI.h>

CRingBuffer<gui_event_t, 32> _eventRB;
gui_event_t _event;

bool TestButtonsHandler(gui_event_t *event) {
  _eventRB.putp(event);
  return true;
}

struct ButtonFixture {
  ButtonFixture() {
    Buttons.clearButtons();
    Encoders.clearEncoders();
    _eventRB.reset();
    GUI.addEventHandler(TestButtonsHandler);
  }

  ~ButtonFixture() {
    GUI.removeEventHandler(TestButtonsHandler);
  }
};


TEST_F (ButtonFixture, SingleButtonPressRelease) {
  GUI_tick();

  CHECK(BUTTON_UP(Buttons.BUTTON1));
  CHECK(!BUTTON_DOWN(Buttons.BUTTON1));
  CHECK(BUTTON_UP(Buttons.BUTTON2));

  /* press a button */
  Buttons.pressButton(Buttons.BUTTON1);
  GUI_tick();
  
  CHECK(BUTTON_DOWN(Buttons.BUTTON1));
  CHECK(!BUTTON_UP(Buttons.BUTTON1));
  CHECK(BUTTON_UP(Buttons.BUTTON2));

  CHECK_EQUAL(1, _eventRB.size());
  _eventRB.getp(&_event);
  CHECK(EVENT_PRESSED(&_event, Buttons.BUTTON1));

  /* let the GUI tick without doing anything */
  GUI_tick();

  CHECK(BUTTON_DOWN(Buttons.BUTTON1));
  CHECK(!BUTTON_UP(Buttons.BUTTON1));
  CHECK(BUTTON_UP(Buttons.BUTTON2));
  CHECK_EQUAL(0, _eventRB.size());
  
  /* release the button */
  Buttons.releaseButton(Buttons.BUTTON1);
  GUI_tick();

  CHECK(!BUTTON_DOWN(Buttons.BUTTON1));
  CHECK(BUTTON_UP(Buttons.BUTTON1));
  CHECK(BUTTON_UP(Buttons.BUTTON2));
  CHECK_EQUAL(1, _eventRB.size());
  _eventRB.getp(&_event);
  CHECK(EVENT_RELEASED(&_event, Buttons.BUTTON1));
}

TEST_F (ButtonFixture, TwoButtonPressRelease) {
  GUI_tick();

  Buttons.pressButton(Buttons.BUTTON1);
  GUI_tick();
  
  CHECK_EQUAL(1, _eventRB.size());
  _eventRB.getp(&_event);
  CHECK(EVENT_PRESSED(&_event, Buttons.BUTTON1));

  GUI_tick();
  CHECK_EQUAL(0, _eventRB.size());
  GUI_tick();
  CHECK_EQUAL(0, _eventRB.size());

  Buttons.pressButton(Buttons.BUTTON2);

  /* let the GUI tick without doing anything */
  GUI_tick();

  CHECK_EQUAL(1, _eventRB.size());
  _eventRB.getp(&_event);
  CHECK(EVENT_PRESSED(&_event, Buttons.BUTTON2));

  GUI_tick();
  CHECK_EQUAL(0, _eventRB.size());
  GUI_tick();
  CHECK_EQUAL(0, _eventRB.size());

  /* release the button */
  Buttons.releaseButton(Buttons.BUTTON1);
  GUI_tick();

  CHECK_EQUAL(1, _eventRB.size());
  _eventRB.getp(&_event);
  CHECK(EVENT_RELEASED(&_event, Buttons.BUTTON1));

  GUI_tick();
  CHECK_EQUAL(0, _eventRB.size());
  GUI_tick();
  CHECK_EQUAL(0, _eventRB.size());

  Buttons.releaseButton(Buttons.BUTTON2);
  GUI_tick();

  CHECK_EQUAL(1, _eventRB.size());
  _eventRB.getp(&_event);
  CHECK(EVENT_RELEASED(&_event, Buttons.BUTTON2));
}
