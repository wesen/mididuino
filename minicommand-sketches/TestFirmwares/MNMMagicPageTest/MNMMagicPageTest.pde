#include <MNM.h>
#include <MNMMagicPage.h>
#include <MidiClockPage.h>

class MNMMagicPageTestSketch : public Sketch, public MNMCallback {
public:
  //  AutoEncoderPage<MNMEncoder> autoPage;
  MagicSwitchPage magicSwitchPage;

  void setup() {
    magicSwitchPage.setup();
    magicSwitchPage.setShortName("P0");

    MNMTask.addOnKitChangeCallback(this, (mnm_callback_ptr_t)&MNMMagicPageTestSketch::onKitChanged);
    ccHandler.setup();

    setPage(&magicSwitchPage);
  }

  void onKitChanged() {
    GUI.setLine(GUI.LINE1);
    GUI.flash_p_string_fill(PSTR("SWITCH KIT"));
    GUI.setLine(GUI.LINE2);
    GUI.flash_string_fill(MNM.kit.name);

    for (int j = 0; j < 4; j++) {
      magicSwitchPage.magicPages[j].clearRecording();
    }
  }

  virtual void show() {
    if (currentPage() == NULL) {
      setPage(&magicSwitchPage);
    }
  }

  virtual Page *getPage(uint8_t i) {
    return &magicSwitchPage;
  }
};

MNMMagicPageTestSketch sketch;

void setup() {
  initMNMTask();

  sketch.setup();
  GUI.setSketch(&sketch);

  initClockPage();
}
