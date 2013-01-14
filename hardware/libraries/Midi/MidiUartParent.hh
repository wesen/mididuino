/* Copyright (c) 2009 - http://ruinwesen.com/ */

#ifndef MIDIUARTPARENT_H__
#define MIDIUARTPARENT_H__

#include <midi-common.hh>
#include "Vector.hh"
#include "Callback.hh"

/**
 * \addtogroup Midi
 *
 * @{
 **/

/**
 * \addtogroup midi_uart Midi UART Parent Class
 *
 * @{
 **/

class MidiUartParent {
  /**
   * \addtogroup midi_uart 
   *
   * @{
   **/
	
public:
  uint8_t running_status;
  uint8_t currentChannel;
  bool useRunningStatus;
  uint16_t sendActiveSenseTimer;
  uint16_t sendActiveSenseTimeout;
  uint16_t recvActiveSenseTimer;
  bool activeSenseEnabled;
  
  MidiUartParent() {
    useRunningStatus = false;
    running_status = 0;
    currentChannel = 0x0;
    activeSenseEnabled = 0;
    recvActiveSenseTimer = 0;
    sendActiveSenseTimer = 0;
  }

  virtual ~MidiUartParent() {

  }

  /**
   * Sets the timeout (in 1.024 ms increments) value for the active
   * sense byte. If the timeout is 0, disable the active sense
   * funcitonality. Else, every timeout ticks, an active sense byte
   * will be sent.
   **/
  void setActiveSenseTimer(uint16_t timeout) {
    if (timeout == 0) {
      activeSenseEnabled = false;
    } else {
      activeSenseEnabled = true;
      sendActiveSenseTimer = 0;
      sendActiveSenseTimeout = timeout;
    }
  }

  /**
   * This method is called periodically by the slow timer (every 1.024 ms).
   **/
  void tickActiveSense() {
    if (recvActiveSenseTimer < 65535) {
      recvActiveSenseTimer++;
    }
    if (activeSenseEnabled) {
      if (sendActiveSenseTimer == 0) {
        putc(MIDI_ACTIVE_SENSE);
        sendActiveSenseTimer = sendActiveSenseTimeout;
      } else {
        sendActiveSenseTimer--;
      }
    }
  }
	
  virtual void initSerial() {
    running_status = 0;
  }

  /** Write raw bytes to the midi output, handle with care as this
   ** will disrupt the running status.
   **/
  virtual void puts(uint8_t *data, uint16_t cnt) {
    while (cnt--)
      putc(*(data++));
  }
  
  virtual void putc(uint8_t c) { }
  /** Write a byte out immediately (bypassing the buffer if there is
   ** one.
   **/
  virtual void putc_immediate(uint8_t c) { putc(c); }
  virtual bool avail() { return false; }

  virtual uint8_t getc() {
    return 0;
  }

  /** Send a MIDI command with only one byte. **/
  virtual void sendMessage(uint8_t cmdByte) {
    sendCommandByte(cmdByte);
  }
  /** Send a MIDI command with one command byte and one data byte. **/
  virtual void sendMessage(uint8_t cmdByte, uint8_t byte1) {
    sendCommandByte(cmdByte);
    putc(byte1);
  }
  /** Send a MIDI command with one command byte and two data bytes. **/
  virtual void sendMessage(uint8_t cmdByte, uint8_t byte1, uint8_t byte2) {
    sendCommandByte(cmdByte);
    putc(byte1);
    putc(byte2);
  }

  void sendCommandByte(uint8_t byte) {
    if (MIDI_IS_REALTIME_STATUS_BYTE(byte) || MIDI_IS_SYSCOMMON_STATUS_BYTE(byte)) {
      if (!MIDI_IS_REALTIME_STATUS_BYTE(byte)) {
        running_status = 0;
        putc(byte);
      } else {
        putc_immediate(byte);
      }
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

  CallbackVector1<MidiCallback, 8, uint8_t *>noteOnCallbacks;
  CallbackVector1<MidiCallback, 8, uint8_t *>noteOffCallbacks;
  CallbackVector1<MidiCallback, 8, uint8_t *>ccCallbacks;

  /** Add a callback for when a MIDI NOTE ON is received. **/
  void addOnNoteOnCallback(MidiCallback *obj, void(MidiCallback::*func)(uint8_t *msg)) {
    noteOnCallbacks.add(obj, func);
  }
  /** Remove a specific MIDI NOTE ON callback. **/
  void removeOnNoteOnCallback(MidiCallback *obj, void(MidiCallback::*func)(uint8_t *msg) ) {
    noteOnCallbacks.remove(obj, func);
  }
  /** Remove all MIDI NOTE ON callbacks registered with this obj. **/
  void removeOnNoteOnCallback(MidiCallback *obj) {
    noteOnCallbacks.remove(obj);
  }

  /** Add a callback for when a MIDI NOTE OFF is received. **/
  void addOnNoteOffCallback(MidiCallback *obj, void(MidiCallback::*func)(uint8_t *msg)) {
    noteOffCallbacks.add(obj, func);
  }
  /** Remove a specific MIDI NOTE OFF callback. **/
  void removeOnNoteOffCallback(MidiCallback *obj, void(MidiCallback::*func)(uint8_t *msg) ) {
    noteOffCallbacks.remove(obj, func);
  }
  /** Remove all MIDI NOTE OFF callbacks registered with this obj. **/
  void removeOnNoteOffCallback(MidiCallback *obj) {
    noteOffCallbacks.remove(obj);
  }

  /** Add a callback for when a MIDI CC is received. **/
  void addOnControlChangeCallback(MidiCallback *obj, void(MidiCallback::*func)(uint8_t *msg)) {
    ccCallbacks.add(obj, func);
  }
  /** Remove a specific MIDI CC callback. **/
  void removeOnControlChangeCallback(MidiCallback *obj, void(MidiCallback::*func)(uint8_t *msg) ) {
    ccCallbacks.remove(obj, func);
  }
  /** Remove all MIDI CC callbacks registered with this obj. **/
  void removeOnControlChangeCallback(MidiCallback *obj) {
    ccCallbacks.remove(obj);
  }

  
  inline void resetRunningStatus() {
    running_status = 0;
  }

  /** Send a MIDI NOTE ON message on the current channel. **/
  inline void sendNoteOn(uint8_t note, uint8_t velocity) {
    sendNoteOn(currentChannel, note, velocity);
  }
  /** Send a MIDI NOTE OFF message with a specific velocity (not really supported by most synths) on the current channel. **/
  inline void sendNoteOff(uint8_t note, uint8_t velocity) {
    sendNoteOff(currentChannel, note, velocity);
  }
  /** Send a MIDI NOTE OFF by sending a MIDI NOTE OFF with velocity 0 on the current channel. **/
  inline void sendNoteOff(uint8_t note) {
    sendNoteOff(currentChannel, note, 0);
  }
  /** Send a MIDI CC message on the current channel. **/
  inline void sendCC(uint8_t cc, uint8_t value) {
    sendCC(currentChannel, cc, value);
  }
  /** Send a MIDI PROGRAM CHANGE message on the current channel. **/
  inline void sendProgramChange(uint8_t program) {
    sendProgramChange(currentChannel, program);
  }

  /** Send a MIDI POLYPHONIC KEY PRESSURE message on the current channel. **/
  inline void sendPolyKeyPressure(uint8_t note, uint8_t pressure) {
    sendPolyKeyPressure(currentChannel, note, pressure);
  }

  /** Send a MIDI CHANNEL PRESSURE message on the current channel. **/
  inline void sendChannelPressure(uint8_t pressure) {
    sendChannelPressure(currentChannel, pressure);
  }

  /** Send a MIDI PITCH BEND message on the current channel. **/
  inline void sendPitchBend(int16_t pitchbend) {
    sendPitchBend(currentChannel, pitchbend);
  }

  /** Send a MIDI NRPN message with 8 bit value on the current channel. **/
  inline void sendNRPN(uint16_t parameter, uint8_t value) {
    sendNRPN(currentChannel, parameter, value);
  }
  /** Send a MIDI NRPN message with 16 bit value on the current channel. **/
  inline void sendNRPN(uint16_t parameter, uint16_t value) {
    sendNRPN(currentChannel, parameter, value);
  }

  /** Send a MIDI RPN message with 8 bit value on the current channel. **/
  inline void sendRPN(uint16_t parameter, uint8_t value) {
    sendRPN(currentChannel, parameter, value);
  }
  /** Send a MIDI RPN message with 16 bit value on the current channel. **/
  inline void sendRPN(uint16_t parameter, uint16_t value) {
    sendRPN(currentChannel, parameter, value);
  }

  /** Send a MIDI NOTE ON message on the specified channel. **/
  void sendNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) {
    if ((channel >= 16) ||
        (note >= 128) ||
        (velocity >= 128))
      return;

    uint8_t msg[3] = {
      (uint8_t)(MIDI_NOTE_ON | channel),
      note,
      velocity
    };
    noteOnCallbacks.call(msg);
    sendMessage(msg[0], msg[1], msg[2]);
  }

  /** Send a MIDI NOTE OFF message on the specified channel. **/
  void sendNoteOff(uint8_t channel, uint8_t note, uint8_t velocity) {
    if ((channel >= 16) ||
        (note >= 128) ||
        (velocity >= 128))
      return;

    uint8_t msg[3] = {
      (uint8_t)(MIDI_NOTE_OFF | channel),
      note,
      velocity
    };
    noteOffCallbacks.call(msg);
    sendMessage(msg[0], msg[1], msg[2]);
  }

  /* Send a MIDI CC message on the specified channel.  **/
  void sendCC(uint8_t channel, uint8_t cc, uint8_t value) {
    if ((channel >= 16) ||
        (cc >= 128) ||
        (value >= 128))
      return;

    uint8_t msg[3] = {
      (uint8_t)(MIDI_CONTROL_CHANGE | channel),
      cc,
      value
    };
    ccCallbacks.call(msg);
    sendMessage(msg[0], msg[1], msg[2]);
  }

  /** Send a MIDI PROGRAM CHANGE message on the specified channel. **/
  void sendProgramChange(uint8_t channel, uint8_t program) {
    if ((channel >= 16) ||
        (program >= 128))
      return;

    sendMessage(MIDI_PROGRAM_CHANGE | channel, program);
  }

  /** Send a MIDI POLYPHONIC KEY PRESSURE message on the specified channel. **/
  void sendPolyKeyPressure(uint8_t channel, uint8_t note, uint8_t pressure) {
    if ((channel >= 16) ||
        (note >= 128) ||
        (pressure >= 128))
      return;

    sendMessage(MIDI_AFTER_TOUCH | channel, note, pressure);
  }

  /** Send a MIDI CHANNEL PRESSURE message on the specified channel. **/
  void sendChannelPressure(uint8_t channel, uint8_t pressure) {
    if ((channel >= 16) ||
        (pressure >= 128))
      return;

    sendMessage(MIDI_CHANNEL_PRESSURE | channel, pressure);
  }

  /** Send a MIDI PITCH BEND message on the specified channel. **/
  void sendPitchBend(uint8_t channel, int16_t pitchbend) {
    pitchbend += 8192;
    sendMessage(MIDI_PITCH_WHEEL | channel, pitchbend & 0x7F, (pitchbend >> 7) & 0x7F);
  }

  /** Send a MIDI NRPN message with 8 bit value on the specified channel. **/
  void sendNRPN(uint8_t channel, uint16_t parameter, uint8_t value) {
    sendCC(channel, 99, (parameter >> 7) & 0x7F);
    sendCC(channel, 98, (parameter & 0x7F));
    sendCC(channel, 6, value);
  }
  /** Send a MIDI NRPN message with 16 bit value on the specified channel. **/
  void sendNRPN(uint8_t channel, uint16_t parameter, uint16_t value) {
    sendCC(channel, 99, (parameter >> 7) & 0x7F);
    sendCC(channel, 98, (parameter & 0x7F));
    sendCC(channel, 6, (value >> 7) & 0x7F);
    sendCC(channel, 38, (value & 0x7F));
  }

  /** Send a MIDI RPN message with 8 bit value on the specified channel. **/
  void sendRPN(uint8_t channel, uint16_t parameter, uint8_t value) {
    sendCC(channel, 101, (parameter >> 7) & 0x7F);
    sendCC(channel, 100, (parameter & 0x7F));
    sendCC(channel, 6, value);
  }
  /** Send a MIDI RPN message with 16 bit value on the specified channel. **/
  void sendRPN(uint8_t channel, uint16_t parameter, uint16_t value) {
    sendCC(channel, 101, (parameter >> 7) & 0x7F);
    sendCC(channel, 100, (parameter & 0x7F));
    sendCC(channel, 6, (value >> 7) & 0x7F);
    sendCC(channel, 38, (value & 0x7F));
  }

  /** Send a MIDI SYSEX message. **/
  virtual void sendSysex(uint8_t *data, uint8_t cnt) {
    sendCommandByte(0xF0);
    puts(data, cnt);
    sendCommandByte(0xF7);
  }
  /** Send raw bytes on the MIDI Uart. **/
  inline void sendRaw(uint8_t *msg, uint16_t cnt) {
    puts(msg, cnt);
  }
  /** Send a single raw byte to the MIDI Uart. **/
  inline void sendRaw(uint8_t byte) {
    putc(byte);
  }

  /** Wrap a string into a sysex message and send it out to the MIDI
   * Uart. This can be used to send debug strings over to the host
   * computer and display them.
   **/
  void sendString(const char *data) {
    sendString(data, m_strlen(data));
  }
  void sendString(const char *data, uint16_t cnt);

  /** Custom version of printf sending out debug strings on the MIDI Uart. **/
  void printfString(const char *fmt, ...) {
    va_list lp;
    va_start(lp, fmt);
    char buf[128];
    uint16_t len = m_vsnprintf(buf, sizeof(buf), fmt, lp);
    va_end(lp);
    sendString(buf, len);
  }

  /* @} */
};

#endif /* MIDIUARTPARENT_H__ */
