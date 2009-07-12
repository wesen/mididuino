#ifndef MNM_TASK_H__
#define MNM_TASK_H__

#include "Vector.hh"

extern MNMClass MNM;

typedef void (*mnm_task_callback_t)();

class MNMTaskClass : public Task {
public:
  bool autoLoadKit;
  bool autoLoadGlobal;
  bool reloadKit;
  bool reloadGlobal;
  bool redisplay;

  MNMTaskClass(uint16_t interval) : Task(interval) {
    redisplay = false;
    autoLoadKit = reloadKit = false;
    autoLoadGlobal = true;
    reloadGlobal = false;
  }

  Vector<mnm_task_callback_t, 4>kitChangeCallbacks;
  Vector<mnm_task_callback_t, 4>globalChangeCallbacks;
  Vector<mnm_task_callback_t, 4>patternChangeCallbacks;
  Vector<mnm_task_callback_t, 4>currentTrackChangeCallbacks;

  void addOnKitChangeCallback(mnm_task_callback_t cb) {
    kitChangeCallbacks.add(cb);
  }
  void removeOnKitChangeCallback(mnm_task_callback_t cb) {
    kitChangeCallbacks.remove(cb);
  }
  void addOnGlobalChangeCallback(mnm_task_callback_t cb) {
    globalChangeCallbacks.add(cb);
  }
  void removeOnGlobalChangeCallback(mnm_task_callback_t cb) {
    globalChangeCallbacks.remove(cb);
  }
  void addOnPatternChangeCallback(mnm_task_callback_t cb) {
    patternChangeCallbacks.add(cb);
  }
  void removeOnPatternChangeCallback(mnm_task_callback_t cb) {
    patternChangeCallbacks.remove(cb);
  }
  void addOnCurrentTrackChangeCallback(mnm_task_callback_t cb) {
    currentTrackChangeCallbacks.add(cb);
  }
  void removeOnCurrentTrackChangeCallback(mnm_task_callback_t cb) {
    currentTrackChangeCallbacks.remove(cb);
  }

  void callKitCallbacks() {
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

  void callCurrentTrackCallbacks() {
    for (int i = 0; i < currentTrackChangeCallbacks.size; i++) {
      if (currentTrackChangeCallbacks.arr[i] != NULL) {
	currentTrackChangeCallbacks.arr[i]();
      }
    }
  }
  
  
  void setup(uint16_t interval = 3000, bool autoLoadKit = false, bool autoLoadGlobal = true,
	     bool reloadGlobal = true);

  virtual void run() {
    MNM.sendRequest(MNM_STATUS_REQUEST_ID,
		    MNM_CURRENT_KIT_REQUEST);
    MNM.sendRequest(MNM_STATUS_REQUEST_ID, MNM_CURRENT_GLOBAL_SLOT_REQUEST);
    MNM.sendRequest(MNM_STATUS_REQUEST_ID, MNM_CURRENT_PATTERN_REQUEST);
    MNM.sendRequest(MNM_STATUS_REQUEST_ID, MNM_CURRENT_AUDIO_TRACK_REQUEST);
  }

  void onStatusResponse(uint8_t type, uint8_t value);

  virtual void destroy();
};

extern MNMTaskClass MNMTask;

#endif /* MNM_TASK_H__ */
