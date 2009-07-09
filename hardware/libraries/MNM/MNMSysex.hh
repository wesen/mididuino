#ifndef MNM_SYSEX_H__
#define MNM_SYSEX_H__

#include "WProgram.h"
#include "Midi.h"
#include "MidiSysex.hh"
#include "Vector.hh"

typedef void (*mnm_callback_t)();
typedef void (*mnm_status_callback_t)(uint8_t type, uint8_t param);

typedef enum {
} mnm_getCurrentKitStatus_t;

class MNMSysexListenerClass : public MidiSysexListenerClass {
public:
  mnm_callback_t onPatternMessageCallback;
  mnm_status_callback_t onStatusResponseCallback;
  mnm_callback_t onGlobalMessageCallback;
  mnm_callback_t    onKitMessageCallback;
  mnm_callback_t onSongMessageCallback;
  mnm_callback_t onCurrentKitCallback;
  
  mnm_getCurrentKitStatus_t mnmGetCurrentKitStatus;

  MNMSysexListenerClass() : MidiSysexListenerClass() {
  }
};

extern MNMSysexListenerClass MNMSysexListener;

#endif /* MNM_SYSEX_H__ */
