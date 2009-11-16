#include <MDLFOPage.h>
#include <MidiClockPage.h>

class MDLFOSketch : 
public Sketch, public MDCallback {
public:
  MDLFOPage lfoPage;
  MDLFOConfigPage lfoConfigPage;
  MDLFOTrackSelectPage lfoTrackSelectPage;
  
  MDLFOSketch() : lfoConfigPage(&lfoPage), lfoTrackSelectPage(&lfoPage) {
  }
  
  void setup() {
    MDTask.addOnKitChangeCallback(this, (md_callback_ptr_t)&MDLFOSketch::onKitChanged);
    setPage(&lfoPage);
  }

  virtual bool handleEvent(gui_event_t *event) {
    if (EVENT_PRESSED(event, Buttons.BUTTON1)) {
      pushPage(&lfoTrackSelectPage);
    } 
    else if (EVENT_RELEASED(event, Buttons.BUTTON1)) {
      popPage(&lfoTrackSelectPage);
    }
    if (EVENT_PRESSED(event, Buttons.BUTTON2)) {
      pushPage(&lfoConfigPage);
    } 
    else if (EVENT_RELEASED(event, Buttons.BUTTON2)) {
      popPage(&lfoConfigPage);
    }
  }

  void onKitChanged() {
    lfoPage.loadFromKit();
  }
};

MDLFOSketch sketch;

void setup() {
  initMDTask();
  sketch.setup();
  GUI.setSketch(&sketch);
  initClockPage();
}

void loop() {
}
