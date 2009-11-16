#include <CppUnitLite2.h>
#include <TestResultStdErr.h>

#include <MNM.h>

#include "../pattern-tests/PrintMNMPattern.h"

struct MNMPatternFixture {
	MNMPatternFixture() {
		pattern.init();
		pattern2.init();
	}

	MNMPattern pattern;
	MNMPattern pattern2;
};

TEST_F (MNMPatternFixture, MNMPatternTestInit) {
	for (uint8_t i = 0; i < 16; i++) {
		CHECK(pattern.isTrackEmpty(i));
	}
}

bool reimportSysex(MNMPattern *p) {
	uint8_t buf[8192];
	uint16_t len = p->toSysex(buf, sizeof(buf));
	return p->fromSysex(buf + 6, len - 7);
}

TEST_F (MNMPatternFixture, MNMPatternEmptyToFromSysex) {
	MNMPattern p2;
	uint8_t buf[8192], buf2[8192];
	uint16_t len = pattern.toSysex(buf, sizeof(buf));
	bool ret = p2.fromSysex(buf, len);
	CHECK(ret);

	CHECK_EQUAL(pattern.origPosition, (p2).origPosition);
	CHECK_EQUAL(pattern.swingAmount, (p2).swingAmount);
	CHECK_EQUAL(pattern.length, (p2).length);
	CHECK_EQUAL(pattern.doubleTempo, (p2).doubleTempo);
	CHECK_EQUAL(pattern.doubleTempo, (p2).doubleTempo);
	CHECK_EQUAL(pattern.kit, (p2).kit);

	for (uint8_t track = 0; track < 6; track++) {
		CHECK_EQUAL(pattern.ampTrigs[track], (p2).ampTrigs[track]);
		CHECK_EQUAL(pattern.filterTrigs[track], (p2).filterTrigs[track]);
		CHECK_EQUAL(pattern.lfoTrigs[track], (p2).lfoTrigs[track]);
		CHECK_EQUAL(pattern.offTrigs[track], (p2).offTrigs[track]);
		CHECK_EQUAL(pattern.triglessTrigs[track], (p2).triglessTrigs[track]);
		CHECK_EQUAL(pattern.chordTrigs[track], (p2).chordTrigs[track]);
		CHECK_EQUAL(pattern.slidePatterns[track], (p2).slidePatterns[track]);
		CHECK_EQUAL(pattern.swingPatterns[track], (p2).swingPatterns[track]);

		CHECK_EQUAL(pattern.midiNoteOnTrigs[track], (p2).midiNoteOnTrigs[track]);
		CHECK_EQUAL(pattern.midiNoteOffTrigs[track], (p2).midiNoteOffTrigs[track]);
		CHECK_EQUAL(pattern.midiTriglessTrigs[track], (p2).midiTriglessTrigs[track]);
		CHECK_EQUAL(pattern.midiSlidePatterns[track], (p2).midiSlidePatterns[track]);
		CHECK_EQUAL(pattern.midiSwingPatterns[track], (p2).midiSwingPatterns[track]);

		for (uint8_t param = 0; param < 72; param++) {
			//			CHECK_EQUAL(pattern.isParamLocked(track, param),
			//									(p2).isParamLocked(track, param));
			for (uint8_t step = 0; step < 64; step++) {
				//				CHECK_EQUAL(pattern.getLock(track, step, param),
				//										(p2).getLock(track, step, param));
			}
		}
	}

	uint16_t len2 = p2.toSysex(buf2, sizeof(buf2));
	CHECK_EQUAL(len, len2);
}
