#ifndef MD_TASK_H__
#define MD_TASK_H__

#include "Vector.hh"
#include "Callback.hh"

extern MDClass MD;

typedef void(*md_task_callback_t)();

class MDTaskClass : 
  public Task {
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
  
  CallbackHandler<MD_CB_CLASS_ID, MD_CB_KITCHANGE_ID, 4> kitChangeHandler;
  
  Vector<md_task_callback_t, 4>kitChangeCallbacks;
  Vector<md_task_callback_t, 4>globalChangeCallbacks;
  Vector<md_task_callback_t, 4>patternChangeCallbacks;

  void addOnKitChangeCallback(md_task_callback_t cb) {
    kitChangeCallbacks.add(cb);
  }
  void removeOnKitChangeCallback(md_task_callback_t cb) {
    kitChangeCallbacks.remove(cb);
  }
  void addOnGlobalChangeCallback(md_task_callback_t cb) {
    globalChangeCallbacks.add(cb);
  }
  void removeOnGlobalChangeCallback(md_task_callback_t cb) {
    globalChangeCallbacks.remove(cb);
  }
  void addOnPatternChangeCallback(md_task_callback_t cb) {
    patternChangeCallbacks.add(cb);
  }
  void removeOnPatternChangeCallback(md_task_callback_t cb) {
    patternChangeCallbacks.remove(cb);
  }

  void callKitCallbacks() {
    kitChangeHandler.callback(NULL);
    
    for (int i = 0; i < kitChangeCallbacks.size; i++) {
      if (kitChangeCallbacks.arr[i] != NULL) {
	kitChangeCallbacks.arr[i]();
      }
    }
  }

  void callGlobalCallbacks() {
    for (int i = 0; i < globalChangeCallbacks.size; i++) {
      if (globalChangeCallbacks.arr[i] != NULL) {
	globalChangeCallbacks.arr[i]();
      }
    }
  }
  
  void callPatternCallbacks() {
    for (int i = 0; i < patternChangeCallbacks.size; i++) {
      if (patternChangeCallbacks.arr[i] != NULL) {
	patternChangeCallbacks.arr[i]();
      }
    }
  }
  
  
  void setup(uint16_t interval = 3000, bool autoLoadKit = false, bool autoLoadGlobal = true,
	     bool reloadGlobal = true);

  virtual void run() {
    MD.sendRequest(MD_STATUS_REQUEST_ID,
		   MD_CURRENT_KIT_REQUEST);
    MD.sendRequest(MD_STATUS_REQUEST_ID, MD_CURRENT_GLOBAL_SLOT_REQUEST);
    MD.sendRequest(MD_STATUS_REQUEST_ID, MD_CURRENT_PATTERN_REQUEST);
  }

  void onStatusResponse(uint8_t type, uint8_t value);

  virtual void destroy();
};


extern MDTaskClass MDTask;
#endif /* MD_TASK_H__ */
