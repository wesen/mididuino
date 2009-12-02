#include <string.h>

RangeEncoder enc1(0, 127, "TST");
RangeEncoder enc2(0, 127, "TST");
RangeEncoder enc3(0, 127, "TST");
RangeEncoder enc4(0, 127, "TST");

class NoteCallback : public MidiCallback {
  public:
  void onNoteCallback(uint8_t *msg) {
  GUI.setLine(GUI.LINE1);
  GUI.flash_string_fill("NOTE RECV");
  }
};

NoteCallback foo;

EncoderPage page(&enc1, &enc2, &enc3, &enc4);
void setup() {
  GUI.setPage(&page);
  Midi2.addOnNoteOnCallback(&foo, (midi_callback_ptr_t)&NoteCallback::onNoteCallback);
  GUI.addEventHandler(handleEvent);
}

void loop() {
//  GUI.updatePage();

//  GUI.update();
}

bool handleEvent(gui_event_t *event) {
  if (EVENT_PRESSED(event, Buttons.BUTTON1)) {
    GUI.flash_string_fill("B1");
    setLed();
  } else if (EVENT_RELEASED(event, Buttons.BUTTON1)) {
    clearLed();
  }
  
  if (EVENT_PRESSED(event, Buttons.BUTTON2)) {
    GUI.flash_string_fill("B2");
    setLed2();
  } else if (EVENT_RELEASED(event, Buttons.BUTTON2)) {
    clearLed2();
  }
  
  if (EVENT_PRESSED(event, Buttons.BUTTON3)) {
    GUI.flash_string_fill("B3");
    OCR3A = 100;
  } else if (EVENT_RELEASED(event, Buttons.BUTTON3)) {
    OCR3A = 200;
  }
  
  if (EVENT_PRESSED(event, Buttons.BUTTON4)) {
    GUI.flash_string_fill("B4");
  }
  
 if (EVENT_PRESSED(event, Buttons.ENCODER1)) {
   GUI.setLine(GUI.LINE2);
   GUI.flash_string_fill("E1");
 }
  if (EVENT_PRESSED(event, Buttons.ENCODER2)) {
    GUI.flash_string_fill("E2");
    MidiUart.sendNoteOn(100, 100);
  }
    
  if (EVENT_PRESSED(event, Buttons.ENCODER3))
    GUI.flash_string_fill("E3");
  if (EVENT_PRESSED(event, Buttons.ENCODER4))
    GUI.flash_string_fill("E4");
}
