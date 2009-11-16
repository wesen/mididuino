#include <MidiEuclidSketch.h>
#include <AutoMidiController.h>
#include <MidiClockPage.h>

AutoMidiControllerSketch sketch;
PitchEuclidSketch sketch2;
SketchSwitchPage sketchSwitchPage(NULL, &sketch, &sketch2);

void setup() {
  sketch.setup();
  sketch2.setup();
  GUI.setSketch(&_defaultSketch);
  GUI.setPage(&sketchSwitchPage);
  GUI.addEventHandler(handleEvent);

  initClockPage();
}

bool handleEvent(gui_event_t *event) {
  if (sketchSwitchPage.handlePopEvent(event)) {
    return true;
  }
  
  if (BUTTON_DOWN(Buttons.BUTTON1) && EVENT_PRESSED(event, Buttons.BUTTON4)) {
    GUI.setSketch(&_defaultSketch);
    GUI.setPage(&sketchSwitchPage);
    return true;
  } else {
    return false;
  }
}

void loop() {
}
