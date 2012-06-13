#include "VirtualMidi.h"

void VirtualMidi::putc(uint8_t c) {
  MidiClass::handleByte(c);
}
