#ifndef MDSYSEX_H__
#define MDSYSEX_H__

#include "WProgram.h"
#include "Midi.h"
#include "MidiSysex.hh"
#include "Vector.hh"
#include "Callback.hh"
#include "MD.h"

typedef enum {
    MD_NONE,
    
    MD_GET_CURRENT_KIT,
    MD_GET_KIT,
    
    MD_GET_CURRENT_GLOBAL,
    MD_GET_GLOBAL,
    
    MD_DONE
} getCurrentKitStatus_t;

class MDSysexListenerClass : public MidiSysexListenerClass {
public:
  CallbackVector<MDCallback,4> onGlobalMessageCallbacks;
  CallbackVector<MDCallback,4> onKitMessageCallbacks;
  CallbackVector<MDCallback,4> onSongMessageCallbacks;
  CallbackVector<MDCallback,4> onPatternMessageCallbacks;
  CallbackVector2<MDCallback,4,uint8_t,uint8_t> onStatusResponseCallbacks;
  
  bool isMDMessage;
  uint8_t msgType;

  MDSysexListenerClass() : MidiSysexListenerClass() {
    ids[0] = 0;
    ids[1] = 0x20;
    ids[2] = 0x3c;
  }

  void addOnStatusResponseCallback(MDCallback *obj, md_status_callback_ptr_t func) {
    onStatusResponseCallbacks.add(obj, func);
  }
  void removeOnStatusResponseCallback(MDCallback *obj, md_status_callback_ptr_t func) {
    onStatusResponseCallbacks.remove(obj, func);
  }
  void removeOnStatusResponseCallback(MDCallback *obj) {
    onStatusResponseCallbacks.remove(obj);
  }

  void addOnGlobalMessageCallback(MDCallback *obj, md_callback_ptr_t func) {
    onGlobalMessageCallbacks.add(obj, func);
  }
  void removeOnGlobalMessageCallback(MDCallback *obj, md_callback_ptr_t func) {
    onGlobalMessageCallbacks.remove(obj, func);
  }
  void removeOnGlobalMessageCallback(MDCallback *obj) {
    onGlobalMessageCallbacks.remove(obj);
  }
  
  void addOnKitMessageCallback(MDCallback *obj, md_callback_ptr_t func) {
    onKitMessageCallbacks.add(obj, func);
  }
  void removeOnKitMessageCallback(MDCallback *obj, md_callback_ptr_t func) {
    onKitMessageCallbacks.remove(obj, func);
  }
  void removeOnKitMessageCallback(MDCallback *obj) {
    onKitMessageCallbacks.remove(obj);
  }
  
  void addOnPatternMessageCallback(MDCallback *obj, md_callback_ptr_t func) {
    onPatternMessageCallbacks.add(obj, func);
  }
  void removeOnPatternMessageCallback(MDCallback *obj, md_callback_ptr_t func) {
    onPatternMessageCallbacks.remove(obj, func);
  }
  void removeOnPatternMessageCallback(MDCallback *obj) {
    onPatternMessageCallbacks.remove(obj);
  }
  
  void addOnSongMessageCallback(MDCallback *obj, md_callback_ptr_t func) {
    onSongMessageCallbacks.add(obj, func);
  }
  void removeOnSongMessageCallback(MDCallback *obj, md_callback_ptr_t func) {
    onSongMessageCallbacks.remove(obj, func);
  }
  void removeOnSongMessageCallback(MDCallback *obj) {
    onSongMessageCallbacks.remove(obj);
  }
  
  virtual void start();
  virtual void handleByte(uint8_t byte);
  virtual void end();

  void setup();
};

#include "MDMessages.hh"

extern MDSysexListenerClass MDSysexListener;

#endif /* MDSYSEX_H__ */
