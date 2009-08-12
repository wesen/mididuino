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

typedef enum {
  MD_PLAYBACK_NONE = 0,
  MD_PLAYBACK_HITS = 1,
  MD_PLAYBACK_CCS = 2
} md_playback_phase_t;

class MDRecorderClass : public MidiCallback {
 public:
  MDRecorderClass();

  uint8_t rec16th_counter;
  uint8_t recordLength;
  bool recordingTriggered;
  uint8_t recordingBoundary;
  bool recording;

  bool looping;
  uint8_t play16th_counter;
  bool playbackTriggered;
  uint8_t playbackBoundary;
  bool playing;
  md_playback_phase_t md_playback_phase;
  
  void setup();
  
  ListWithPool<md_recorder_event_t, 128> eventList;
  ListElt<md_recorder_event_t> *playPtr;

  void startRecord(uint8_t length, uint8_t boundary = 0);
  void stopRecord();
  void startPlayback(uint8_t boundary = 0);
  void startMDPlayback(uint8_t boundary = 0);
  void stopPlayback();

  void onNoteOnCallback(uint8_t *msg);
  void onCCCallback(uint8_t *msg);
  void on16Callback();
};

extern MDRecorderClass MDRecorder;

#endif /* MDRECORDER_H__ */
