#include <MNM.h>

class TestMNMSketch : public Sketch, MNMCallback {
public:
  EncoderPage page;

  TestMNMSketch() {
  }

  void setup() {
  }

  bool handleEvent(gui_event_t *event) {
    if (EVENT_PRESSED(event, Buttons.BUTTON1)) {
      GUI.setLine(GUI.LINE2);
      GUI.put_value(0, 0);
      uint8_t track = MNM.getCurrentTrack(1000);
      GUI.setLine(GUI.LINE1);
      GUI.put_p_string(PSTR("TRACK"));
      GUI.setLine(GUI.LINE2);
      GUI.put_value(0, track);
    }

    return false;
  }
};

TestMNMSketch sketch;

void setup() {
  MidiUart.sendString("HELLO");
  //  enableProfiling();
//  initMNMTask();
  sketch.setup();
  MNMSysexListener.setup();
  GUI.setSketch(&sketch);
}

void loop() {
}
