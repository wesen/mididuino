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
  bool looping;
  uint32_t start16th;
  uint8_t rec16th_counter;
  uint8_t play16th_counter;
  uint8_t recordLength;

  void setup();
  
  ListWithPool<md_recorder_event_t, 128> eventList;
  ListElt<md_recorder_event_t> *playPtr;

  void startRecord(uint8_t length);
  void stopRecord();
  void startPlayback();
  void stopPlayback();

  void onNoteOnCallback(uint8_t *msg);
  void onCCCallback(uint8_t *msg);
  void on16Callback();
};

extern MDRecorderClass MDRecorder;

#endif /* MDRECORDER_H__ */
