#include <SDCard.h>
#include <string.h>

RangeEncoder enc1(0, 127, "TST");
RangeEncoder enc2(0, 127, "TST");
RangeEncoder enc3(0, 127, "TST");
RangeEncoder enc4(0, 127, "TST");

void onNoteCallback(uint8_t *msg) {
  GUI.setLine(GUI.LINE1);
  GUI.flash_string_fill("NOTE RECV");
}

EncoderPage page(&enc1, &enc2, &enc3, &enc3);
void setup() {
  SDCard.init();
  //  setLed2();
//  setLed();
  page.encoders[0] = &enc1;
  page.encoders[1] = &enc2;
  page.encoders[2] = &enc3;
  page.encoders[3] = &enc4;
  GUI.setPage(&page);
  Midi2.setOnNoteOnCallback(onNoteCallback);
}

void loop() {
  GUI.updatePage();

  GUI.update();
}

void handleGui() {
  if (BUTTON_PRESSED(Buttons.BUTTON1)) {
    GUI.flash_string_fill("B1");
    setLed();
  } else if (BUTTON_RELEASED(Buttons.BUTTON1)) {
    clearLed();
  }
  
  if (BUTTON_PRESSED(Buttons.BUTTON2)) {
    GUI.flash_string_fill("B2");
    setLed2();
  } else if (BUTTON_RELEASED(Buttons.BUTTON2)) {
    clearLed2();
  }
  
  if (BUTTON_PRESSED(Buttons.BUTTON3)) {
    GUI.flash_string_fill("B3");
    OCR3A = 100;
  } else if (BUTTON_RELEASED(Buttons.BUTTON3)) {
    OCR3A = 200;
  }
  
  if (BUTTON_PRESSED(Buttons.BUTTON4)) {
    GUI.flash_string_fill("B4");
    SDCardFile file;
    if (!file.create("/RW.txt")) {
      GUI.flash_string_fill("CREATE");
      if (!file.open("/RW.txt")) {
        GUI.flash_string_fill("OPEN EXIST");
        return;
      }
    }
    if (file.write((uint8_t*)"HELO", 4) != 4) {
      GUI.flash_string_fill("WRITE");
      return;
    }
    file.close();
    
    char buf[5];
    if (!file.open("/RW.txt")) {
      GUI.flash_string_fill("OPEN");
      return;
    }
    if (file.read((uint8_t *)buf, 4) != 4) {
      GUI.flash_string_fill("READ");
      return;
    }

    buf[4] = 0;
    file.close();
    if (!strncmp(buf, "HELO", 5)) {
      GUI.flash_strings_fill("SD SUCCESS", "");
    } else {
      GUI.flash_strings_fill("SD FAIL", "");
    }
    //    if (sd_raw_init() == 0) {
//      GUI.put_string_at_fill(0, "SD RAW FAILED");
//      return;
//    } 
//    struct sd_raw_info info = { 0 } ;

//    if (sd_raw_get_info(&info) == 0) {
//      GUI.put_string_at_fill(0, "SD INFO FAILED");
//      return;
//    }
    
  }
  
 if (BUTTON_PRESSED(Buttons.ENCODER1)) {
   GUI.setLine(GUI.LINE2);
   GUI.flash_string_fill("E1");
 }
  if (BUTTON_PRESSED(Buttons.ENCODER2)) {
    GUI.flash_string_fill("E2");
    MidiUart.sendNoteOn(100, 100);
  }
    
  if (BUTTON_PRESSED(Buttons.ENCODER3))
    GUI.flash_string_fill("E3");
  if (BUTTON_PRESSED(Buttons.ENCODER4))
    GUI.flash_string_fill("E4");
}
