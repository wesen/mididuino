//
//  TOP LEVEL SKETCH
//
#include <MidiClockPage.h>
OctatrackTransposeSketch sketch;
SketchSwitchPage sketchSwitchPage(NULL, &sketch, NULL, NULL, NULL);

void setup() {
  sketch.setupMonster(true); 
  GUI.setSketch(&_defaultSketch);
  GUI.setPage(&sketchSwitchPage);
  GUI.addEventHandler(handleEvent);

  initClockPage();
}

bool handleEvent(gui_event_t *event) {
   // Top Left button
//   if (EVENT_PRESSED(event, Buttons.BUTTON1)) {       
//       // Top Right button (while holding Top Left)
//       if (EVENT_PRESSED(event, Buttons.BUTTON4)) {
//          GUI.setPage(&sketchSwitchPage);
//          return true;          
//       }
//   } else {   
       return sketchSwitchPage.handleGlobalEvent(event);
//   }
}

void loop() {
}
