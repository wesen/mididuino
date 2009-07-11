#ifndef MIDISYSEX_H__
#define MIDISYSEX_H__

#include <inttypes.h>

#include "WProgram.h"

#ifndef SYSEX_BUF_SIZE
#define SYSEX_BUF_SIZE 128
#endif

class MidiSysexListenerClass {
 public:
  uint8_t ids[3];

  MidiSysexListenerClass() {
    ids[0] = 0;
    ids[1] = 0;
    ids[2] = 0;
  };
  
  virtual void start() {
  }
  virtual void abort() {
  }
  virtual void end() {
  }
  virtual void handleByte(uint8_t byte) {
  }
};

#define NUM_SYSEX_SLAVES 4

class MidiSysexClass {
 protected:
  bool aborted;
  bool recording;
  uint8_t recvIds[3];
  bool sysexLongId;

 public:
  uint16_t max_len;
  uint16_t recordLen;
  uint8_t *data;
  uint8_t *recordBuf;
  uint16_t maxRecordLen;

  uint16_t len;

  MidiSysexListenerClass *listeners[NUM_SYSEX_SLAVES];
  
  MidiSysexClass(uint8_t *_data, uint16_t size) {
    data = _data;
    max_len = size;
    len = 0;
    aborted = false;
    recording = false;
    recordBuf = NULL;
    maxRecordLen = 0;
    sysexLongId = false;
  }

  void initSysexListeners() {
    for (int i = 0; i < NUM_SYSEX_SLAVES; i++)
      listeners[i] = NULL;
  }
  bool addSysexListener(MidiSysexListenerClass *listener) {
    for (int i = 0; i < NUM_SYSEX_SLAVES; i++) {
      if (listeners[i] == NULL || listeners[i] == listener) {
	listeners[i] = listener;
	return true;
      }
    }
    return false;
  }
  void removeSysexListeners(MidiSysexListenerClass *listener) {
    for (int i = 0; i < NUM_SYSEX_SLAVES; i++) {
      if (listeners[i] == listener)
	listeners[i] = NULL;
    }
  }
  bool isListenerActive(MidiSysexListenerClass *listener);

  void startRecord(uint8_t *buf = NULL, uint16_t maxLen = 0);
  void stopRecord();
  void resetRecord(uint8_t *buf = NULL, uint16_t maxLen = 0);
  bool recordByte(uint8_t c);
  void reset();
  
  void start();
  void abort();
  void end();
  void handleByte(uint8_t byte);
};

class MididuinoSysexListenerClass : public MidiSysexListenerClass {
 public:
  MididuinoSysexListenerClass();
  virtual void handleByte(uint8_t byte);
};

extern MidiSysexClass MidiSysex;
extern MididuinoSysexListenerClass MididuinoSysexListener;

#endif /* MIDISYSEX_H__ */
