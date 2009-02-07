#ifndef MDSYSEX_H__
#define MDSYSEX_H__

#include "WProgram.h"
#include "Midi.h"

typedef struct md_kit_s {
  char name[16];
  uint8_t trackModels[16];
} md_kit_t;

typedef struct md_global_s {
  uint8_t baseChannel;
} md_global_t;  
  
typedef void (*md_callback_t)();
typedef void (*md_kit_callback_t)(md_kit_t *kit);
typedef void (*md_global_callback_t)(md_global_t *global);
typedef void (*md_status_callback_t)(uint8_t type, uint8_t param);

class MachineDrumSysexClass : public MididuinoSysexClass {
  bool isMachineDrumSysex;
  md_status_callback_t onStatusResponseCallback;
  md_global_callback_t onGlobalMessageCallback;
  md_kit_callback_t    onKitMessageCallback;
  md_kit_t    lastReceivedKit;
  md_global_t lastReceivedGlobal;
  
public:
  MachineDrumSysexClass(uint8_t *data, uint16_t size) : MididuinoSysexClass(data, size) {
    onStatusResponseCallback = NULL;
    onGlobalMessageCallback = NULL;
    onKitMessageCallback = NULL;
  }

  uint8_t msgType;

  void setOnStatusResponseCallback(md_status_callback_t callback) {
    onStatusResponseCallback = callback;
  }
  void setOnGlobalMessageCallback(md_global_callback_t callback) {
    onGlobalMessageCallback = callback;
  }
  void setOnKitMessageCallback(md_kit_callback_t callback) {
    onKitMessageCallback = callback;
  }

  void handleGlobalDump(uint8_t c);
  void handleKitDump(uint8_t c);
  
  virtual void start();
  virtual void handleByte(uint8_t byte);
  virtual void end();

  void setup();
};

extern MachineDrumSysexClass MDSysex;


#endif /* MDSYSEX_H__ */
