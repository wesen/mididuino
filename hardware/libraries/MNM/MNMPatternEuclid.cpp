#include "MNMPatternEuclid.h"

static uint8_t patData[8192];

MNMPatternEuclid::MNMPatternEuclid() : PitchEuclid() {
	track.setEuclid(3, 8, 0);
	randomizePitches();
}

void MNMPatternEuclid::makeTrack(uint8_t trackNum) {
	pattern.clearTrack(trackNum);
	for (uint8_t i = 0; i < pattern.patternLength; i++) {
		if (track.isHit(i)) {
			pattern.setTrig(trackNum, i);
			uint8_t pitch = basePitch + pitches[pitches_idx];
			pitches_idx = (pitches_idx + 1) % pitches_len;
			//			pattern.addLock(trackNum, i, 0, pitch);
			pattern.noteNBR[trackNum][i] = pitch;
		}
	}

	uint16_t len = pattern.toSysex(patData, sizeof(patData));
	patData[0] = 0xF0;
	patData[len-1] = 0xF7;
	MidiUart.puts(patData, len);

	char name[5];
	MNM.getPatternName(pattern.origPosition, name);
	GUI.flash_strings_fill("SENT PATTERN", name);
}
