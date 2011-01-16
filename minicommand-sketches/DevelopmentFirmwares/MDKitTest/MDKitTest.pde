#include <MD.h>

MDKit kit;

class MDKitTestPage : public EncoderPage, MDCallback {
  public:
  RangeEncoder kitEncoder;
  
  MDKitTestPage() : kitEncoder(0, 64, "KIT") {
  }
  
  void onKitChanged() {
    kit.fromSysex(MidiSysex.data + 5, MidiSysex.recordLen - 5);
    GUI.flash_strings_fill("FOOBAR", MD.kit.name);
    ElektronDataToSysexEncoder encoder(&MidiUart);
    kit.toSysex(encoder);
  }
  
  void setup() {
    MDTask.addOnKitChangeCallback(this, (md_callback_ptr_t)&MDKitTestPage::onKitChanged);
  }
  
  virtual bool handleEvent(gui_event_t *event) {
    if (EVENT_PRESSED(event, Buttons.BUTTON3)) {
      bool result = MD.getBlockingKit(kitEncoder.getValue());
    }
  }
};

MDKitTestPage page;

void setup() {
  initMDTask();
  
  GUI.setPage(&page);
}

void loop() {
}
