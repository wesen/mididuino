#ifndef MDRECORDER_H__
#define MDRECORDER_H__

#include "WProgram.h"
#include "ListPool.hh"

typedef struct md_recorder_event_s {
  uint8_t channel;
  uint8_t pitch;
  uint8_t value;
  uint8_t step;
} md_recorder_event_t;

class MDRecorderClass {
 public:
  MDRecorderClass();

  bool recording;
  bool playing;
  uint32_t start16th;
  uint8_t rec16th_counter;
  uint8_t recordLength;
  
  ListWithPool<md_recorder_event_t, 128> eventList;

  void startRecord(uint8_t length);
  void playback();

  void onNoteOnCallback(uint8_t *msg);
  void onCCCallback(uint8_t *msg);
  void on16Callback();
};

#endif /* MDRECORDER_H__ */
