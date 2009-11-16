#include <MDWesenLivePatchSketch.h>
#include <MDPatternEuclidSketch.h>
#include <MelodyHelperSketch.h>
#include <MDNotesRecordSketch.h>

#include <MidiClockPage.h>

// MDWesenLivePatchSketch sketch;

#define F2   for (uint16_t bla = 0; bla < 32; bla++) i = bla;
#define F F2 F2 F2 F2 F2 F2 F2 F2 F2 F2 F2 F2 F2 F2 F2 F2 F2 F2 F2 F2 F2 F2 F2 F2 F2 F2 F2 F2 F2 F2 F2 F2 F2 F2 F2 F2 F2 F2 F2 F2 F2 F2 F2 F2 F2 F2 F2 

void foo() {
  volatile uint8_t i;
  
//  F F F F F F F   F F F F F F F   F F F F F F F   F F F F F F F   F F F F F F F   F F F F F F F   F F F F F F F  
}
  
MDWesenLivePatchSketch sketch;
MDPatternEuclidSketch sketch2;
SketchSwitchPage sketchSwitchPage(NULL, NULL, &sketch2);

void setup() {
//  foo();
  initMDTask();
  
//  sketch.setupMonster(true);
  sketch2.setupMonster(true);
//  sketch3.setupMonster(true);
//  sketch4.setupMonster(true);
  GUI.setSketch(&_defaultSketch);
  GUI.setPage(&sketchSwitchPage);
  GUI.addEventHandler(handleEvent);

  initClockPage();
}

bool handleEvent(gui_event_t *event) {
  return sketchSwitchPage.handleGlobalEvent(event);
}

void loop() {
}
