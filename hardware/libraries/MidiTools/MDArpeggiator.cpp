#include "WProgram.h"
#include "helpers.h"
#include "Arpeggiator.hh"
#include <MD.h>

void MDArpeggiatorClass::recordNote(int pos, uint8_t track, uint8_t note, uint8_t velocity) {
  uint8_t realPitch = MD.trackGetPitch(track, note);
  if (realPitch == 128)
    return;  
  MD.triggerTrack(track, velocity);
  recordPitches[pos] = note;
}
  
void MDArpeggiatorClass::recordNoteSecond(int pos, uint8_t track) {
  pos -= recordLength;
  if (recordPitches[pos + 1] != 128) {
    uint8_t realPitch = MD.trackGetPitch(track, recordPitches[pos + 1]);
    if (realPitch == 128)
      return;  
    MD.setTrackParam(track, 0, realPitch - 5);
    delay(3);
    MD.setTrackParam(track, 0, realPitch);
  }
}
  
void MDArpeggiatorClass::playNext(bool recording) {
  int pos = MidiClock.div16th_counter - recordStart;
  if (pos == 0 && recording)
    retrigger();
      
  if (recording && (pos >= (recordLength - 1))) {
    recordNoteSecond(pos, arpTrack);
  }
    
  if (arpLen == 0 || (arpTimes != 0 && arpCount >= arpTimes))
    return;
      
  if (arpRetrig == RETRIG_BEAT && (MidiClock.div16th_counter % retrigSpeed) == 0)
    retrigger();
  if (++speedCounter >= arpSpeed) {
    speedCounter = 0;
    if (arpStyle == ARP_STYLE_RANDOM) {
      uint8_t i = random(numNotes);
      if (recording && (pos < recordLength)) {
	recordNote(pos, arpTrack, orderedNotes[i] + random(arpOctaves) * 12, orderedVelocities[i]);
      } else if (!recording) {
	MD.sendNoteOn(arpTrack, orderedNotes[i] + random(arpOctaves) * 12, orderedVelocities[i]);
      }
    } else {
      if (recording && (pos < recordLength)) {
	recordNote(pos, arpTrack, arpNotes[arpStep] + 12 * arpOctaveCount, arpVelocities[arpStep]);
      } else if (!recording) {
	MD.sendNoteOn(arpTrack, arpNotes[arpStep] + 12 * arpOctaveCount, arpVelocities[arpStep]);
      }
      if (++arpStep == arpLen) {
	if (arpOctaveCount < arpOctaves) {
	  arpStep = 0;
	  arpOctaveCount++;
	} else {
	  arpStep = 0;
	  arpOctaveCount = 0;
	  arpCount++;
	}
      }
    }
  } 
}

