#ifndef MIDIUARTPARENT_H__
#define MIDIUARTPARENT_H__

#include <midi-common.hh>
#include "Vector.hh"

class MidiUartParent {
public:
  uint8_t running_status;
  uint8_t currentChannel;
  bool useRunningStatus;
  
  MidiUartParent() {
    useRunningStatus = false;
    running_status = 0;
    currentChannel = 0x0;
  }

  virtual void initSerial() {
    running_status = 0;
  }
  
  virtual void puts(uint8_t *data, uint16_t cnt) {
    while (cnt--)
      putc(*(data++));
  }
  
  virtual void putc(uint8_t c) { }
  virtual void putc_immediate(uint8_t c) { }
  virtual bool avail() { return false; }
  virtual uint8_t getc() {
    return 0;
  }

  void sendCommandByte(uint8_t byte) {
    if (MIDI_IS_REALTIME_STATUS_BYTE(byte) || MIDI_IS_SYSCOMMON_STATUS_BYTE(byte)) {
      if (!MIDI_IS_REALTIME_STATUS_BYTE(byte)) {
	running_status = 0;
      }
      putc_immediate(byte);
    } else {
      if (useRunningStatus) {
	if (running_status != byte) 
	  putc(byte);
	running_status = byte;
      } else {
	putc(byte);
      }
    }
  }

  Vector<midi_callback_t, 4>noteOnCallbacks;
  Vector<midi_callback_t, 4>noteOffCallbacks;
  Vector<midi_callback_t, 4>CCCallbacks;

  void addOnNoteOnCallback(midi_callback_t cb) {
    noteOnCallbacks.add(cb);
  }
  void removeOnNoteOnCallback(midi_callback_t cb) {
    noteOnCallbacks.remove(cb);
  }
  void addOnNoteOffCallback(midi_callback_t cb) {
    noteOffCallbacks.add(cb);
  }
  void removeOnNoteOffCallback(midi_callback_t cb) {
    noteOffCallbacks.remove(cb);
  }
  void addOnControlChangeCallback(midi_callback_t cb) {
    CCCallbacks.add(cb);
  }
  void removeOnControlChangeCallback(midi_callback_t cb) {
    CCCallbacks.remove(cb);
  }
  

  inline void resetRunningStatus() {
    running_status = 0;
  }

  inline void sendNoteOn(uint8_t note, uint8_t velocity) {
    sendNoteOn(currentChannel, note, velocity);
  }
  inline void sendNoteOff(uint8_t note, uint8_t velocity) {
    sendNoteOff(currentChannel, note, velocity);
  }
  inline void sendNoteOff(uint8_t note) {
    sendNoteOff(currentChannel, note, 0);
  }
  inline void sendCC(uint8_t cc, uint8_t value) {
    sendCC(currentChannel, cc, value);
  }
  inline void sendProgramChange(uint8_t program) {
    sendProgramChange(currentChannel, program);
  }

  inline void sendPolyKeyPressure(uint8_t note, uint8_t pressure) {
    sendPolyKeyPressure(currentChannel, note, pressure);
  }

  inline void sendChannelPressure(uint8_t pressure) {
    sendChannelPressure(currentChannel, pressure);
  }

  inline void sendPitchBend(int16_t pitchbend) {
    sendPitchBend(currentChannel, pitchbend);
  }

  inline void sendNRPN(uint16_t parameter, uint8_t value) {
    sendNRPN(currentChannel, parameter, value);
  }
  inline void sendNRPN(uint16_t parameter, uint16_t value) {
    sendNRPN(currentChannel, parameter, value);
  }

  inline void sendRPN(uint16_t parameter, uint8_t value) {
    sendRPN(currentChannel, parameter, value);
  }
  inline void sendRPN(uint16_t parameter, uint16_t value) {
    sendRPN(currentChannel, parameter, value);
  }

  void sendNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) {
    uint8_t msg[3] = {
      MIDI_NOTE_ON | channel,
      note,
      velocity
    };
    for (int i = 0 ; i < noteOnCallbacks.size; i++) {
      if (noteOnCallbacks.arr[i] != NULL)
	noteOnCallbacks.arr[i](msg);
    }
    sendCommandByte(msg[0]);
    putc(msg[1]);
    putc(msg[2]);
  }

  void sendNoteOff(uint8_t channel, uint8_t note, uint8_t velocity) {
    uint8_t msg[3] = {
      MIDI_NOTE_OFF | channel,
      note,
      velocity
    };
    for (int i = 0 ; i < noteOffCallbacks.size; i++) {
      if (noteOffCallbacks.arr[i] != NULL)
	noteOffCallbacks.arr[i](msg);
    }
    sendCommandByte(msg[0]);
    putc(msg[1]);
    putc(msg[2]);
  }

  void sendCC(uint8_t channel, uint8_t cc, uint8_t value) {
    uint8_t msg[3] = {
      MIDI_CONTROL_CHANGE | channel,
      cc,
      value
    };
    for (int i = 0 ; i < CCCallbacks.size; i++) {
      if (CCCallbacks.arr[i] != NULL)
	CCCallbacks.arr[i](msg);
    }
    sendCommandByte(msg[0]);
    putc(msg[1]);
    putc(msg[2]);
  }

  void sendProgramChange(uint8_t channel, uint8_t program) {
    sendCommandByte(MIDI_PROGRAM_CHANGE | channel);
    putc(program);
  }

  void sendPolyKeyPressure(uint8_t channel, uint8_t note, uint8_t pressure) {
    sendCommandByte(MIDI_AFTER_TOUCH | channel);
    putc(note);
    putc(pressure);
  }

  void sendChannelPressure(uint8_t channel, uint8_t pressure) {
    sendCommandByte(MIDI_CHANNEL_PRESSURE | channel);
    putc(pressure);
  }

  void sendPitchBend(uint8_t channel, int16_t pitchbend) {
    sendCommandByte(MIDI_PITCH_WHEEL | channel);
    pitchbend += 8192;
    putc(pitchbend & 0x7F);
    putc((pitchbend >> 7) & 0x7F);
  }

  void sendNRPN(uint8_t channel, uint16_t parameter, uint8_t value) {
    sendCC(channel, 99, (parameter >> 7) & 0x7F);
    sendCC(channel, 98, (parameter & 0x7F));
    sendCC(channel, 6, value);
  }
  void sendNRPN(uint8_t channel, uint16_t parameter, uint16_t value) {
    sendCC(channel, 99, (parameter >> 7) & 0x7F);
    sendCC(channel, 98, (parameter & 0x7F));
    sendCC(channel, 6, (value >> 7) & 0x7F);
    sendCC(channel, 38, (value & 0x7F));
  }

  void sendRPN(uint8_t channel, uint16_t parameter, uint8_t value) {
    sendCC(channel, 101, (parameter >> 7) & 0x7F);
    sendCC(channel, 100, (parameter & 0x7F));
    sendCC(channel, 6, value);
  }
  void sendRPN(uint8_t channel, uint16_t parameter, uint16_t value) {
    sendCC(channel, 101, (parameter >> 7) & 0x7F);
    sendCC(channel, 100, (parameter & 0x7F));
    sendCC(channel, 6, (value >> 7) & 0x7F);
    sendCC(channel, 38, (value & 0x7F));
  }

  void sendSysex(uint8_t *data, uint8_t cnt) {
    sendCommandByte(0xF7);
    puts(data, cnt);
    sendCommandByte(0xF0);
  }
  inline void sendRaw(uint8_t *msg, uint16_t cnt) {
    puts(msg, cnt);
  }
  inline void sendRaw(uint8_t byte) {
    putc(byte);
  }
};

#endif /* MIDIUARTPARENT_H__ */
