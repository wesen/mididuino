#ifndef PITCH_EUCLID_H__
#define PITCH_EUCLID_H__

#include <inttypes.h>
#include <Sequencer.h>
#include <Scales.h>
#include <MidiClock.h>

class PitchEuclid : public ClockCallback {
public:
  EuclidDrumTrack track;
  scale_t *currentScale;


  uint8_t pitches[32];
  uint8_t pitches_len;
  uint8_t pitches_idx;

  uint8_t mdTrack;
  bool muted;

  uint8_t octaves;
  uint8_t basePitch;

  PitchEuclid(scale_t *scale) : 
  track(3, 8, 0) {
    currentScale = scale;
    octaves = 0;

    pitches_len = 0;
    pitches_idx = 0;
    setPitchLength(4);
    mdTrack = 0;
    muted = false;
  }

  void setup();
  void setPitchLength(uint8_t len);
  void randomizePitches();
  void on16Callback(uint32_t counter);
};


#endif /* PITCH_EUCLID_H__ */
