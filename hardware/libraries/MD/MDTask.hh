#ifndef MD_TASK_H__
#define MD_TASK_H__

#include "Vector.hh"
#include "Callback.hh"

extern MDClass MD;

class MDTaskClass : 
  public Task, public MDCallback {
public:
  bool autoLoadKit;
  bool autoLoadGlobal;
  bool reloadKit;
  bool reloadGlobal;
  bool redisplay;

  MDTaskClass(uint16_t interval) : Task(interval) {
    redisplay = false;
    autoLoadKit = reloadKit = false;
    autoLoadGlobal = true;
    reloadGlobal = false;
  }

  const static uint8_t MD_CB_CLASS_ID = 10;
  const static uint8_t MD_CB_KITCHANGE_ID = 0;;

  CallbackVector<MDCallback, 8> kitChangeCallbacks;
  CallbackVector<MDCallback, 8> globalChangeCallbacks;
  CallbackVector<MDCallback, 8> patternChangeCallbacks;

  void addOnKitChangeCallback(MDCallback *obj, md_callback_ptr_t func) {
    kitChangeCallbacks.add(obj, func);
  }
  void removeOnKitChangeCallback(MDCallback *obj, md_callback_ptr_t func) {
    kitChangeCallbacks.remove(obj, func);
  }
  void removeOnKitChangeCallback(MDCallback *obj) {
    kitChangeCallbacks.remove(obj);
  }
  
  void addOnGlobalChangeCallback(MDCallback *obj, md_callback_ptr_t func) {
    globalChangeCallbacks.add(obj, func);
  }
  void removeOnGlobalChangeCallback(MDCallback *obj, md_callback_ptr_t func) {
    globalChangeCallbacks.remove(obj, func);
  }
  void removeOnGlobalChangeCallback(MDCallback *obj) {
    globalChangeCallbacks.remove(obj);
  }
  
  void addOnPatternChangeCallback(MDCallback *obj, md_callback_ptr_t func) {
    patternChangeCallbacks.add(obj, func);
  }
  void removeOnPatternChangeCallback(MDCallback *obj, md_callback_ptr_t func) {
    patternChangeCallbacks.remove(obj, func);
  }
  void removeOnPatternChangeCallback(MDCallback *obj) {
    patternChangeCallbacks.remove(obj);
  }
  
  void setup(uint16_t interval = 3000, bool autoLoadKit = false, bool autoLoadGlobal = true,
	     bool reloadGlobal = true);

  virtual void run() {
    MD.sendRequest(MD_STATUS_REQUEST_ID,
		   MD_CURRENT_KIT_REQUEST);
    MD.sendRequest(MD_STATUS_REQUEST_ID, MD_CURRENT_GLOBAL_SLOT_REQUEST);
    MD.sendRequest(MD_STATUS_REQUEST_ID, MD_CURRENT_PATTERN_REQUEST);
  }

  void onStatusResponseCallback(uint8_t type, uint8_t value);
  void onGlobalMessageCallback();
  void onKitMessageCallback();

  virtual void destroy();
};

void initMDTask();

extern MDTaskClass MDTask;
#endif /* MD_TASK_H__ */
