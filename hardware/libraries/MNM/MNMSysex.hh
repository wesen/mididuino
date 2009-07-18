#ifndef MNM_SYSEX_H__
#define MNM_SYSEX_H__

#include "WProgram.h"
#include "Midi.h"
#include "MidiSysex.hh"
#include "Vector.hh"
#include "Elektron.hh"
#include "Circular.hh"

typedef void (*mnm_callback_t)();
typedef void (*mnm_status_callback_t)(uint8_t type, uint8_t param);

class MNMSysexStatusCallback {
public:
  virtual void onMNMStatusCallback(uint8_t type, uint8_t param) = 0;
};

class MNMSysexKitCallback {
public:
  virtual void onMNMKitCallback() = 0;
};

class MNMSysexGlobalCallback {
public:
  virtual void onMNMGlobalCallback() = 0;
};

class MNMSysexPatternCallback {
public:
  virtual void onMNMPatternCallback() = 0;
};

class MNMSysexSongCallback {
public:
  virtual void onMNMSongCallback() = 0;
};

class MNMSysexListenerClass : public MidiSysexListenerClass {
public:
  mnm_status_callback_t onStatusResponseCallback;
  Vector<mnm_status_callback_t, 4> statusCallbacks;
  Vector<MNMSysexStatusCallback *, 4> statusCallbackObjs;
  
  mnm_callback_t onGlobalMessageCallback;
  mnm_callback_t onKitMessageCallback;
  mnm_callback_t onSongMessageCallback;
  mnm_callback_t onPatternMessageCallback;
  
  bool isMNMMessage;
  bool isMNMEncodedMessage;
  uint8_t msgType;

  CircularBuffer<uint8_t, 4> sysexCirc;
  MNMSysexToDataEncoder encoder;
  uint16_t msgLen;
  uint16_t msgCksum;
  
  MNMSysexListenerClass() : MidiSysexListenerClass() {
    ids[0] = 0;
    ids[1] = 0x20;
    ids[2] = 0x3c;
    onStatusResponseCallback = NULL;
    onGlobalMessageCallback = NULL;
    onKitMessageCallback = NULL;
  }

  void addOnStatusResponseCallback(mnm_status_callback_t callback) {
    statusCallbacks.add(callback);
  }
  void removeOnStatusResponseCallback(mnm_status_callback_t callback) {
    statusCallbacks.remove(callback);
  }
  void addOnStatusResponseCallback(MNMSysexStatusCallback *callback) {
    statusCallbackObjs.add(callback);
  }
  void removeOnStatusResponseCallback(MNMSysexStatusCallback *callback) {
    statusCallbackObjs.remove(callback);
  }
  /* compatibility to old stuff */
  void setOnStatusResponseCallback(mnm_status_callback_t callback) {
    if (onStatusResponseCallback != NULL) {
      removeOnStatusResponseCallback(onStatusResponseCallback);
    } 

    onStatusResponseCallback = callback;
    addOnStatusResponseCallback(onStatusResponseCallback);
  }
  
  void setOnGlobalMessageCallback(mnm_callback_t callback) {
    onGlobalMessageCallback = callback;
  }
  void setOnKitMessageCallback(mnm_callback_t callback) {
    onKitMessageCallback = callback;
  }

  void setOnSongMessageCallback(mnm_callback_t callback) {
    onSongMessageCallback = callback;
  }

  void setOnPatternMessageCallback(mnm_callback_t callback) {
    onPatternMessageCallback = callback;
  }
  
  virtual void start();
  virtual void handleByte(uint8_t byte);
  virtual void end();

  void setup();
  
};

extern MNMSysexListenerClass MNMSysexListener;

#endif /* MNM_SYSEX_H__ */
