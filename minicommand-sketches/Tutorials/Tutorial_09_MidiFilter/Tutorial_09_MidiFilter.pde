#include <Scales.h>

void onNoteOn(uint8_t *msg) {
  uint8_t newPitch = scalePitch(msg[1], 0, invMajorScale);
  GUI.setLine(GUI.LINE2);
  GUI.flash_printf_fill("ON  %b -> %b", msg[1], newPitch);
  MidiUart.sendNoteOn(MIDI_VOICE_CHANNEL(msg[0]), newPitch, msg[2]);
}

void onNoteOff(uint8_t *msg) {
  uint8_t newPitch = scalePitch(msg[1], 0, invMajorScale);
  GUI.setLine(GUI.LINE2);
  GUI.flash_printf_fill("OFF %b -> %b", msg[1], newPitch);
  MidiUart.sendNoteOff(MIDI_VOICE_CHANNEL(msg[0]), newPitch, msg[2]);
}

void setup() {
  GUI.setLine(GUI.LINE1);
  GUI.put_string_fill("MIDI FILTER");
}
