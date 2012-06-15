/*
 * MidiCtrl - Host implementation of a MIDI Uart skeletton (with sysex listener)
 *
 * (c) July 2011 - Manuel Odendahl - wesen@ruinwesen.com
 */

#include <unistd.h>

#include "Platform.h"
#include "MidiUartHost.h"

uint8_t sysexBuf[8192];
uint8_t sysexBuf2[8192];

#ifndef TEST_SUITE
MidiClass Midi(NULL, sysexBuf, sizeof(sysexBuf));
MidiClass Midi2(NULL, sysexBuf2, sizeof(sysexBuf2));
MidiUartHostClass MidiUart;
#endif

void MidiUartHostSysexListener::end() {
  // XXX Debug
  //  printf("send sysex of %d bytes\n", sysex->len);

  uint8_t buf[sysex->len +2];
  buf[0] = 0xF0;
  m_memcpy(buf + 1, sysex->data, sysex->len);
  buf[sysex->len + 1] = 0xF7;
  uart->midiSendLong(buf, sysex->len + 2);
}

void handleIncomingMidi() {
  MidiUart.runLoop();
  while (MidiUart.avail()) {
    Midi.handleByte(MidiUart.getc());
  }
  usleep(1000);
}

void MidiUartHostParent::onOutputMessage(uint8_t *msg, uint8_t len) {
  //  printf("output message %d bytes\n", len);
  //  hexdump(msg, len);

  if (len > 3) {
    midiSendLong(msg, len);
  } else {
    switch (len) {
    case 0:
      return;
    case 1:
      midiSendShort(msg[0], 0, 0);
      break;
    case 2:
      midiSendShort(msg[0], msg[1], 0);
      break;
    case 3:
      midiSendShort(msg[0], msg[1], msg[2]);
      break;
    default:
      return;
    }
  }
}

bool MidiUartHostParent::init(int _inputDevice, int _outputDevice) {
  inputDevice = _inputDevice;
  outputDevice = _outputDevice;
    
  outputMidi.init();
  outputMidi.addOnMessageCallback(this, (midi_callback_ptr2_t)&MidiUartHostParent::onOutputMessage);

  outputMidi.midiSysex.addSysexListener(&sysexListener);

  return true;
}

