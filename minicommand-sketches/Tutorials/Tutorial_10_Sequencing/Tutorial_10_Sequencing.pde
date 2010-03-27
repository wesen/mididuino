#include <Scales.h>

RangeEncoder lengthEncoder(1, 16, "LEN");
EncoderPage page(&lengthEncoder);

uint8_t melodyNotes[16] = { 
  0 };
uint8_t prevNote = 0;

void randomizeNotes() {
  for (int i = 0; i < countof(melodyNotes); i++) {
    melodyNotes[i] = scalePitch(48 + random(24), 0, invMajorScale);
  }
}

void on16Callback(uint32_t pos) {
  GUI.setLine(GUI.LINE2);
  GUI.put_value(1, (uint8_t)(pos % 16));
  MidiUart.sendNoteOff(prevNote);
  prevNote = melodyNotes[pos % lengthEncoder.getValue()];
  MidiUart.sendNoteOn(prevNote, 100);
}

void setup() {
  GUI.setLine(GUI.LINE1);
  GUI.put_string_fill("    SEQUENCER");

  randomizeNotes();
  lengthEncoder.setValue(8);
  MidiClock.mode = MidiClock.INTERNAL;
  MidiClock.setTempo(100);

  GUI.setPage(&page);
}

bool handleEvent(gui_event_t *evt) {
  if (EVENT_PRESSED(evt, Buttons.BUTTON1)) {
    randomizeNotes();
    return true;
  }
  
  if (EVENT_PRESSED(evt, Buttons.BUTTON3)) {
    MidiClock.start();
    return true;
  }
  
  if (EVENT_PRESSED(evt, Buttons.BUTTON4)) {
    MidiClock.stop();
    return true;
  }
  
  return false;
}

