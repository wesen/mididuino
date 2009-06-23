#ifndef MD_TASK_H__
#define MD_TASK_H__

extern MDClass MD;

class MDTaskClass : 
  public Task {
public:
  bool autoLoadKit;
  bool autoLoadGlobal;
  bool redisplay;

  MDTaskClass(uint16_t interval) : Task(interval) {
    redisplay = false;
    autoLoadKit = false;
    autoLoadGlobal = true;
  }

  void setup();

  virtual void run() {
    MD.sendRequest(MD_STATUS_REQUEST_ID, MD_CURRENT_KIT_REQUEST);
    MD.sendRequest(MD_STATUS_REQUEST_ID, MD_CURRENT_GLOBAL_SLOT_REQUEST);
    MD.sendRequest(MD_STATUS_REQUEST_ID, MD_CURRENT_PATTERN_REQUEST);
  }

  void onStatusResponse(uint8_t type, uint8_t value);
};


extern MDTaskClass MDTask;
#endif /* MD_TASK_H__ */
