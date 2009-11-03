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
	bool ret = p2.fromSysex(buf + 6, len - 7);
	CHECK(ret);
}
