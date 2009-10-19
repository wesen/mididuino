#include <CppUnitLite2.h>
#include <TestResultStdErr.h>

#include <MD.h>

struct MDPatternFixture {
	MDPatternFixture() {
		pattern.init();
		pattern2.init();
	}

	MDPattern pattern;
	MDPattern pattern2;

	uint8_t buf[8192];
	uint8_t buf2[8192];
};

TEST_F (MDPatternFixture, MDPatternTestInit) {
	for (uint8_t i = 0; i < 16; i++) {
		CHECK(pattern.isTrackEmpty(i));
	}
}

#define MDPATTERN_EQUAL(p1, p2) { \
	CHECK_EQUAL((p1).origPosition, (p2).origPosition);                         \
	CHECK_EQUAL((p1).accentPattern, (p2).accentPattern);                       \
	CHECK_EQUAL((p1).slidePattern, (p2).slidePattern);                         \
	CHECK_EQUAL((p1).swingPattern, (p2).swingPattern);                         \
	CHECK_EQUAL((p1).swingAmount, (p2).swingAmount);                           \
	CHECK_EQUAL((p1).accentAmount, (p2).accentAmount);                         \
	CHECK_EQUAL((p1).patternLength, (p2).patternLength);                       \
	CHECK_EQUAL((p1).doubleTempo, (p2).doubleTempo);                           \
	CHECK_EQUAL((p1).scale, (p2).scale);                                       \
	CHECK_EQUAL((p1).kit, (p2).kit);                                           \
                                                                           \
	for (uint8_t track = 0; track < 16; track++) {                           \
		CHECK_EQUAL((p1).trigPatterns[track], (p2).trigPatterns[track]);         \
		CHECK_EQUAL((p1).lockPatterns[track], (p2).lockPatterns[track]);         \
		CHECK_EQUAL((p1).accentPatterns[track], (p2).accentPatterns[track]);     \
		CHECK_EQUAL((p1).slidePatterns[track], (p2).slidePatterns[track]);       \
		CHECK_EQUAL((p1).swingPatterns[track], (p2).swingPatterns[track]);       \
  \
		for (uint8_t param = 0; param < 24; param++) {                         \
			CHECK_EQUAL((p1).isParamLocked(track, param),                         \
									(p2).isParamLocked(track, param));                        \
			for (uint8_t step = 0; step < 32; step++) {                          \
				CHECK_EQUAL((p1).getLock(track, step, param),                       \
										(p2).getLock(track, step, param));                      \
			}                               \
		} \
	} \
}

TEST_F (MDPatternFixture, MDPatternEmptyToFromSysex) {
	uint16_t len = pattern.toSysex(buf, sizeof(buf));
	pattern2.fromSysex(buf + 7, len - 7);
	uint16_t len2 = pattern2.toSysex(buf2, sizeof(buf2));
	CHECK_EQUAL(len, len2);
	for (uint16_t i = 0; i < len; i++) {
		CHECK_EQUAL(buf[i], buf2[i]);
	}

	MDPATTERN_EQUAL(pattern, pattern2);
}




