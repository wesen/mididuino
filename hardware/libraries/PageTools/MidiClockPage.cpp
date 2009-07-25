#include "MidiClockPage.h"

const char *MidiClockPage::clockSourceEnum[] = {
    "NONE", "MIDI1", "MIDI2"
  };
  

void MidiClockPage::setup() {
  clockSourceEncoder.initEnumEncoder(clockSourceEnum, 3, "CLK");
  transmitEncoder.initBoolEncoder("SND");
  encoders[0] = &clockSourceEncoder;
  encoders[2] = &transmitEncoder;
}

void MidiClockPage::show() {
    GUI.flash_strings_fill("MIDI CLOCK", "SETUP");
    redisplayPage();
  }

void MidiClockPage::loop() {
  if (clockSourceEncoder.hasChanged()) {
    switch (clockSourceEncoder.getValue()) {
    case 0: // NONE
      MidiClock.stop();
      MidiClock.mode = MidiClock.OFF;
      //      GUI.flash_strings_fill("MIDI CLOCK SRC", "NONE");
      break;
      
    case 1: // MIDI 1
      MidiClock.stop();
      MidiClock.mode = MidiClock.EXTERNAL_MIDI;
      //      GUI.flash_strings_fill("MIDI CLOCK SRC", "MIDI PORT 1");
      MidiClock.start();
      break;
      
    case 2: // MIDI 2
      MidiClock.stop();
      MidiClock.mode = MidiClock.EXTERNAL_UART2;
      //      GUI.flash_strings_fill("MIDI CLOCK SRC", "MIDI PORT 2");
      MidiClock.start();
      break;
    }
  }
  if (transmitEncoder.hasChanged()) {
    if (transmitEncoder.getBoolValue()) {
      MidiClock.transmit = true;
      //      GUI.flash_strings_fill("MIDI CLOCK OUT", "ACTIVATED");
    } else {
      MidiClock.transmit = false;
      //      GUI.flash_strings_fill("MIDI CLOCK OUT", "DEACTIVATED");
    }
  }

}

 bool MidiClockPage::handleEvent(gui_event_t *event) {
  if (EVENT_PRESSED(event, Buttons.BUTTON1)) {
    MidiUart.sendNoteOn(90,10);
    GUI.sketch->popPage(this);
    return true;
  }
  return true;
}
