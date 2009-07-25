#include "MidiClockPage.h"
#include <SDCard.h>

const char *MidiClockPage::clockSourceEnum[] = {
    "NONE", "MIDI1", "MIDI2"
  };

void MidiClockPage::writeClockSettings() {
  uint8_t buf[2];
  buf[0] = MidiClock.mode;
  buf[1] = MidiClock.transmit ? 1 : 0;
  if (!SDCard.writeFile("/ClockSettings.txt", buf, 2, true)) {
    GUI.flash_strings_fill("ERROR SAVING", "CLOCK SETUP");
  }
}

void MidiClockPage::readClockSettings() {
  uint8_t buf[2];
  if (!SDCard.readFile("/ClockSettings.txt", buf, 2)) {
    GUI.flash_strings_fill("ERROR READING", "CLOCK SETUP");
  } else {
    MidiClock.mode = (MidiClockClass::clock_mode_t)buf[0];
    MidiClock.transmit = buf[1];
    if (MidiClock.mode == MidiClock.EXTERNAL_MIDI || MidiClock.mode == MidiClock.EXTERNAL_UART2) {
      MidiClock.start();
    }
  }
}

void MidiClockPage::setup() {
  clockSourceEncoder.initEnumEncoder(clockSourceEnum, 3, "CLK");
  transmitEncoder.initBoolEncoder("SND");
  readClockSettings();
  switch (MidiClock.mode) {
  case MidiClockClass::OFF:
    clockSourceEncoder.setValue(0);
    break;
  case MidiClockClass::EXTERNAL_MIDI:
    clockSourceEncoder.setValue(1);
    break;
  case MidiClockClass::EXTERNAL_UART2:
    clockSourceEncoder.setValue(2);
    break;
  }
  if (MidiClock.transmit) {
    transmitEncoder.setValue(1);
  }
  encoders[0] = &clockSourceEncoder;
  encoders[2] = &transmitEncoder;
}

void MidiClockPage::show() {
    GUI.flash_strings_fill("MIDI CLOCK", "SETUP");
    redisplayPage();
  }

void MidiClockPage::loop() {
  bool changed = false;
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
    changed = true;
  }
  if (transmitEncoder.hasChanged()) {
    if (transmitEncoder.getBoolValue()) {
      MidiClock.transmit = true;
      //      GUI.flash_strings_fill("MIDI CLOCK OUT", "ACTIVATED");
    } else {
      MidiClock.transmit = false;
      //      GUI.flash_strings_fill("MIDI CLOCK OUT", "DEACTIVATED");
    }
    changed = true;
  }

  if (changed) {
    writeClockSettings();
  }
}

 bool MidiClockPage::handleEvent(gui_event_t *event) {
  if (EVENT_PRESSED(event, Buttons.BUTTON1)) {
    GUI.sketch->popPage(this);
    return true;
  }
  return true;
}

MidiClockPage midiClockPage;
