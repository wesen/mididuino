#include <CppUnitLite2.h>
#include <TestResultStdErr.h>

/*
 * use:
 * - CHECK (condition)
 * - CHECK_EQUAL(expected, actual)
 */

#include <GUI.h>

CRingBuffer<gui_event_t, 32> _eventRB;

bool TestButtonsHandler(gui_event_t *event) {
  printf("got event from GUI\n");
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


TEST_F (ButtonFixture, TestButtons) {
  CHECK(BUTTON_UP(Buttons.BUTTON1));
  CHECK(!BUTTON_DOWN(Buttons.BUTTON1));
  CHECK(BUTTON_UP(Buttons.BUTTON2));

  Buttons.pressButton(Buttons.BUTTON1);

  CHECK(BUTTON_DOWN(Buttons.BUTTON1));
  CHECK(!BUTTON_UP(Buttons.BUTTON1));
  CHECK(BUTTON_UP(Buttons.BUTTON2));
  CHECK(BUTTON_PRESSED(Buttons.BUTTON1));
  CHECK(!BUTTON_RELEASED(Buttons.BUTTON1));
  CHECK(!BUTTON_PRESSED(Buttons.BUTTON2));
  CHECK(!BUTTON_RELEASED(Buttons.BUTTON2));

  pollEventGUI();
  GUI.loop();

  CHECK_EQUAL(1, _eventRB.size());

  gui_event_t event;
  _eventRB.getp(&event);
  CHECK(EVENT_PRESSED(&event, Buttons.BUTTON1));
}
