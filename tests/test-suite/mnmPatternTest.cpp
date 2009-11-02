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
