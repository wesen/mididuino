/*
 * MidiCtrl - Juno Sysex listener
 *
 * (c) January 2013 - Manuel Odendahl - wesen@ruinwesen.com
 */

#ifndef JUNO_SYSEX_H__
#define JUNO_SYSEX_H__

#include "PlatformConfig.h"
#include "Midi.h"
#include "MidiSysex.hh"
#include "Vector.hh"
#include "Callback.hh"
#include "Juno.h"

class JunoSysexListenerClass : public MidiSysexListenerClass {
public:
  uint8_t msgType;

  CallbackVector3<JunoCallback, 4, uint8_t, uint8_t, uint8_t> onControlChangeCallbacks;
  CallbackVector3<JunoCallback, 4, uint8_t, uint8_t, JunoPatch *> onBankChangeCallbacks;
  CallbackVector3<JunoCallback, 4, uint8_t, uint8_t, JunoPatch *> onManualModeCallbacks;

  JunoSysexListenerClass() : MidiSysexListenerClass() {
    ids[0] = 0x41;
  }

  virtual ~JunoSysexListenerClass() {
  }

  /**
   * Add the juno sysex listener to the MIDI sysex subsystem. This needs to be called if you
   * want to use the JunoSysexListener.
   */
  void setup();

  void addOnControlChangeCallback(JunoCallback *obj, juno_callback_control_change_ptr_t func) {
    onControlChangeCallbacks.add(obj, func);
  }
  void removeOnControlChangeCallback(JunoCallback *obj, juno_callback_control_change_ptr_t func) {
    onControlChangeCallbacks.remove(obj, func);
  }
  void removeOnControlChangeCallback(JunoCallback *obj) {
    onControlChangeCallbacks.remove(obj);
  }

  void addOnBankChangeCallback(JunoCallback *obj, juno_callback_bank_change_ptr_t func) {
    onBankChangeCallbacks.add(obj, func);
  }
  void removeOnBankChangeCallback(JunoCallback *obj, juno_callback_bank_change_ptr_t func) {
    onBankChangeCallbacks.remove(obj, func);
  }
  void removeOnBankChangeCallback(JunoCallback *obj) {
    onBankChangeCallbacks.remove(obj);
  }


  void addOnManualModeCallback(JunoCallback *obj, juno_callback_manual_mode_ptr_t func) {
    onManualModeCallbacks.add(obj, func);
  }
  void removeOnManualModeCallback(JunoCallback *obj, juno_callback_manual_mode_ptr_t func) {
    onManualModeCallbacks.remove(obj, func);
  }
  void removeOnManualModeCallback(JunoCallback *obj) {
    onManualModeCallbacks.remove(obj);
  }
};

extern JunoSysexListenerClass JunoSysexListener;

#endif /* JUNO_SYSEX_H__ */
