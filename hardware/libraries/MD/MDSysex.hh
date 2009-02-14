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

typedef enum handle_kit_state_e {
  handle_kit_name = 0,
  handle_kit_params,
  handle_kit_levels,
  handle_kit_models,
  handle_kit_none
} handle_kit_state_t;
  
typedef void (*md_callback_t)();
typedef void (*md_kit_callback_t)(md_kit_t *kit);
typedef void (*md_global_callback_t)(md_global_t *global);
typedef void (*md_status_callback_t)(uint8_t type, uint8_t param);

typedef enum {
    MD_NONE,
    
    MD_GET_CURRENT_KIT,
    MD_GET_KIT,
    
    MD_GET_CURRENT_GLOBAL,
    MD_GET_GLOBAL,
    
    MD_DONE
} getCurrentKitStatus_t;
class MachineDrumSysexClass : public MididuinoSysexClass {
public:
  bool isMachineDrumSysex;

  md_status_callback_t onStatusResponseCallback;
  md_global_callback_t onGlobalMessageCallback;
  md_kit_callback_t    onKitMessageCallback;
  md_callback_t onCurrentKitCallback;
  
  md_kit_t    lastReceivedKit;
  md_global_t lastReceivedGlobal;

  handle_kit_state_t handle_kit_state;

  getCurrentKitStatus_t mdGetCurrentKitStatus;

public:
  MachineDrumSysexClass(uint8_t *data, uint16_t size) : MididuinoSysexClass(data, size) {
    onStatusResponseCallback = NULL;
    onGlobalMessageCallback = NULL;
    onKitMessageCallback = NULL;
    onCurrentKitCallback = NULL;
  }

  uint8_t msgType;

  void getCurrentKit(md_callback_t callback);

  void setOnCurrentKitCallback(md_callback_t callback) {
    onCurrentKitCallback = callback;
  }
  
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
