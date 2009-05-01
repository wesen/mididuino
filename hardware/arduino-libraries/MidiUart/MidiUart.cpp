#include <MidiUart.h>
#include <midi-common.hh>

MidiUartClass MidiUart;

MidiUartClass::MidiUartClass() {
  currentChannel = 0x0;
  initSerial();
}

void MidiUartClass::initSerial() {
  Serial.begin(31250);
}

void MidiUartClass::puts(uint8_t *data, uint8_t cnt) {
  while (cnt--)
    putc(*(data++));
}

void MidiUartClass::putc_immediate(uint8_t c) {
  putc(c);
}

void MidiUartClass::putc(uint8_t c) {
  Serial.write(c);
}

bool MidiUartClass::avail() {
  return Serial.available();
}

uint8_t MidiUartClass::getc() {
  return Serial.read();
}

void MidiUartClass::sendNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) {
  putc(MIDI_NOTE_ON | channel);
  putc(note);
  putc(velocity);
}

void MidiUartClass::sendNoteOff(uint8_t channel, uint8_t note, uint8_t velocity) {
  putc(MIDI_NOTE_OFF | channel);
  putc(note);
  putc(velocity);
}

void MidiUartClass::sendCC(uint8_t channel, uint8_t cc, uint8_t value) {
  putc(MIDI_CONTROL_CHANGE | channel);
  putc(cc);
  putc(value);
}

void MidiUartClass::sendProgramChange(uint8_t channel, uint8_t program) {
  putc(MIDI_PROGRAM_CHANGE | channel);
  putc(program);
}

void MidiUartClass::sendPolyKeyPressure(uint8_t channel, uint8_t note, uint8_t pressure) {
  putc(MIDI_AFTER_TOUCH | channel);
  putc(note);
  putc(pressure);
}

void MidiUartClass::sendChannelPressure(uint8_t channel, uint8_t pressure) {
  putc(MIDI_CHANNEL_PRESSURE | channel);
  putc(pressure);
}

void MidiUartClass::sendPitchBend(uint8_t channel, int16_t pitchbend) {
  putc(MIDI_PITCH_WHEEL | channel);
  pitchbend += 8192;
  putc(pitchbend & 0x7F);
  putc((pitchbend >> 7) & 0x7F);
}

void MidiUartClass::sendNRPN(uint8_t channel, uint16_t parameter, uint8_t value) {
  sendCC(channel, 99, (parameter >> 7) & 0x7F);
  sendCC(channel, 98, (parameter & 0x7F));
  sendCC(channel, 6, value);
}
void MidiUartClass::sendNRPN(uint8_t channel, uint16_t parameter, uint16_t value) {
  sendCC(channel, 99, (parameter >> 7) & 0x7F);
  sendCC(channel, 98, (parameter & 0x7F));
  sendCC(channel, 6, (value >> 7) & 0x7F);
  sendCC(channel, 38, (value & 0x7F));
}

void MidiUartClass::sendRPN(uint8_t channel, uint16_t parameter, uint8_t value) {
  sendCC(channel, 101, (parameter >> 7) & 0x7F);
  sendCC(channel, 100, (parameter & 0x7F));
  sendCC(channel, 6, value);
}
void MidiUartClass::sendRPN(uint8_t channel, uint16_t parameter, uint16_t value) {
  sendCC(channel, 101, (parameter >> 7) & 0x7F);
  sendCC(channel, 100, (parameter & 0x7F));
  sendCC(channel, 6, (value >> 7) & 0x7F);
  sendCC(channel, 38, (value & 0x7F));
}

