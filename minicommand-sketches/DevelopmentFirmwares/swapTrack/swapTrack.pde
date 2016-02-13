#include <MD.h>
#include <Scales.h>
#include <MDStudioPages.h>


MDPattern pattern;
MDSwapTrackPage swapTrackPage(&pattern);

void setup() {
  initMDTask();
  GUI.setPage(&swapTrackPage);
}

bool handleEvent(gui_event_t *event) {
  if (EVENT_PRESSED(event, Buttons.BUTTON3)) {
    GUI.flash_p_strings_fill(PSTR("REQUEST"), PSTR("PATTERN"));
    MD.getBlockingPattern(MD.currentPattern);
    GUI.flash_p_strings_fill(PSTR("GOT"), PSTR("PATTERN"));
  }
}
