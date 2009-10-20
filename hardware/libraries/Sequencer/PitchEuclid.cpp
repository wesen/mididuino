#include "PitchEuclid.h"

void PitchEuclid::setup() {
  MidiClock.addOn16Callback(this, (midi_clock_callback_ptr_t)&PitchEuclid::on16Callback);
}

void PitchEuclid::setPitchLength(uint8_t len)  {
  pitches_len = len;
  randomizePitches();
}

void PitchEuclid::randomizePitches()  {
  for (uint8_t i = 0; i < pitches_len; i++) {
    pitches[i] = randomScalePitch(currentScale, octaves);
  }
}

void PitchEuclid::on16Callback(uint32_t counter)  {
  if (track.isHit(MidiClock.div16th_counter)) {
    uint8_t pitch = basePitch + pitches[pitches_idx];
    if (pitch <= 127) {
      if (!muted) {
				MidiUart.sendNoteOn(mdTrack, pitch, 100);
      }
    }
    pitches_idx = (pitches_idx + 1) % pitches_len;
  }
}
