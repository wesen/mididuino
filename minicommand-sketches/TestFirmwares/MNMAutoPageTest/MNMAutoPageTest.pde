#include <MNM.h>
#include <AutoEncoderPage.h>
#include <MidiClockPage.h>

class MyAutoEncoderPage : public AutoEncoderPage<MNMEncoder> {
  bool handleEvent(gui_event_t *event) {
    bool ret = AutoEncoderPage<MNMEncoder>::handleEvent(event);
    return ret;
  }
};

class MNMAutoPageTestSketch : public Sketch, public MNMCallback, public ClockCallback {
public:
  //  AutoEncoderPage<MNMEncoder> autoPage;
  MyAutoEncoderPage autoPage;

  void setup() {
    autoPage.setup();
    autoPage.setShortName("P0");

    MNMTask.addOnKitChangeCallback(this, (mnm_callback_ptr_t)&MNMAutoPageTestSketch::onKitChanged);
    MidiClock.addOn32Callback(this, (midi_clock_callback_ptr_t)&MNMAutoPageTestSketch::on32Callback);

    ccHandler.setup();

    setPage(&autoPage);
  }

  void onKitChanged() {
    autoPage.clearRecording();

    GUI.setLine(GUI.LINE1);
    GUI.flash_p_string_fill(PSTR("SWITCH KIT"));
    GUI.setLine(GUI.LINE2);
    GUI.flash_string_fill(MNM.kit.name);
  }

  void on32Callback(uint32_t counter) {
    autoPage.on32Callback(counter);
  }

  virtual void show() {
    if (currentPage() == NULL) {
      setPage(&autoPage);
    }
  }

  virtual Page *getPage(uint8_t i) {
    return &autoPage;
  }
};

MNMAutoPageTestSketch sketch;

void setup() {
  initMNMTask();

  sketch.setup();
  GUI.setSketch(&sketch);

  initClockPage();
}
