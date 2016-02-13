#include <MidiClockPage.h>
#include <OT.h>
#include <AutoEncoderPage.h>

class OctatrackWesenLiveSketch : public Sketch {
public:
  AutoEncoderPage<OTEncoder> autoOTPages[4];
  SwitchPage switchPage;

  void setupPages() {
    for (int i = 0; i < 4; i++) {
      autoOTPages[i].setup();
      autoOTPages[i].setShortName("P ");
      autoOTPages[i].shortName[1] = '0' + i;
    }

    switchPage.initPages(&autoOTPages[0], &autoOTPages[1], &autoOTPages[2], &autoOTPages[3]);
    switchPage.parent = this;
  }

  virtual void setup() {
    for (uint8_t i = 0; i < OT_NUM_TRACKS; i++) {
      OT.tracks[i].fx1 = OT_FX_FILTER;
      OT.tracks[i].fx2 = OT_FX_DELAY;
    }
    setupPages();
    ccHandler.setup();

    setPage(&autoOTPages[0]);
  }


  virtual bool handleEvent(gui_event_t *event) {
    if (EVENT_PRESSED(event, Buttons.BUTTON1)) {
      pushPage(&switchPage);
    }
    else if (EVENT_RELEASED(event, Buttons.BUTTON1)) {
      popPage(&switchPage);
    }

    return true;
  }

  void clearAllRecording() {
    for (int i = 0; i < 4; i++) {
      autoOTPages[i].clearRecording();
    }
  }

};

OctatrackWesenLiveSketch sketch;

void setup() {
  sketch.setup();
  GUI.setSketch(&sketch);

  initClockPage();
}

void loop() {
}
