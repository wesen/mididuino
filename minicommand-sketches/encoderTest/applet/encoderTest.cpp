#include <SDCard.h>
#include <string.h>

#include "WProgram.h"
void onNoteCallback(uint8_t *msg);
void setup();
void loop();
void handleGui();
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

#include <Midi.h>

#include "WProgram.h"
#include <avr/interrupt.h>

void setup();
void loop();
void handleGui();

#define PHASE_FACTOR 16
static inline uint32_t phase_mult(uint32_t val) {
  return (val * PHASE_FACTOR) >> 8;
}

ISR(TIMER1_OVF_vect) {
  //  setLed2();

  clock++;
#ifdef MIDIDUINO_MIDI_CLOCK
  if (MidiClock.state == MidiClock.STARTED)
    MidiClock.handleTimerInt();
#endif

  //  clearLed2();
}

// XXX CMP to have better time

void gui_poll() {
  uint16_t sr = SR165.read16();
  Buttons.clear();
  Buttons.poll(sr >> 8);
  Encoders.poll(sr);
}

uint16_t lastRunningStatusReset = 0;

ISR(TIMER2_OVF_vect) {
  //  setLed2();

#ifdef MIDIDUINO_POLL_GUI_IRQ
  gui_poll();
  handleGui();
#endif
  slowclock++;

  if (abs(slowclock - lastRunningStatusReset) > 3000) {
    MidiUart.resetRunningStatus();
    lastRunningStatusReset = slowclock;
  }
  
  //  clearLed2();
}

MidiClass Midi;
MidiClass Midi2;

void __mainInnerLoop(bool callLoop) {
    if (MidiUart.avail()) {
      Midi.handleByte(MidiUart.getc());
    }

    if (MidiUart2.avail()) {
      Midi2.handleByte(MidiUart2.getc());
    }
    
#if defined(MIDIDUINO_POLL_GUI) && !defined(MIDIDUINO_POLL_GUI_IRQ)
    cli();
    gui_poll();
    handleGui();
    sei();
#endif

    if (callLoop)
      loop();
}

int main(void) {
    delay(100);
  init();
  clearLed();
  clearLed2();

  for (int i = 0; i < GUI_NUM_BUTTONS; i++) {
    SET_B_CURRENT(i);
  }

  MidiSysex.addSysexListener(&MididuinoSysexListener);
  setup();
  sei();

  for (;;) {
    __mainInnerLoop();
  }
  return 0;
}
