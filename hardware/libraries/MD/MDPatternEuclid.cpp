#include "MDPatternEuclid.h"

static uint8_t patData[5500];

MDPatternEuclid::MDPatternEuclid() : MDPitchEuclid() {
	track.setEuclid(3, 8, 0);
	randomizePitches();
}

void MDPatternEuclid::makeTrack(uint8_t trackNum) {
	pattern.clearTrack(trackNum);
	for (uint8_t i = 0; i < pattern.patternLength; i++) {
		if (track.isHit(i)) {
			pattern.setTrig(trackNum, i);
			if (MD.isMelodicTrack(trackNum)) {
				uint8_t pitch = basePitch + pitches[pitches_idx];
				pitches_idx = (pitches_idx + 1) % pitches_len;
				uint8_t realPitch = MD.trackGetPitch(trackNum, pitch);
				if (realPitch < 128) {
					pattern.addLock(trackNum, i, 0, realPitch);
				}
			}
		}
	}
	uint16_t len = pattern.toSysex(patData, sizeof(patData));
	MidiUart.sendRaw(patData, len);
  
	char name[5];
	MD.getPatternName(pattern.origPosition, name);
	GUI.flash_strings_fill("SENT PATTERN", name);
}


