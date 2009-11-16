#include <CppUnitLite2.h>
#include <TestResultStdErr.h>

#include <MNM.h>

#include "../pattern-tests/PrintMNMPattern.h"

#define MAX_TRACKS 6

struct MNMPatternFixture {
	MNMPatternFixture() {
		pattern.init();
		pattern2.init();
	}

	MNMPattern pattern;
	MNMPattern pattern2;
};

TEST_F (MNMPatternFixture, MNMPatternTestInit) {
	for (uint8_t i = 0; i < 6; i++) {
		CHECK(pattern.isTrackEmpty(i));
	}
}

void dumpMonoSysex(uint8_t *buf, uint16_t len) {
	uint8_t buftmp[8192];
	MNMSysexDecoder decoder(DATA_ENCODER_INIT(buf + 10, len - 10));
	decoder.get(buftmp + 1, len - 10);
	hexdump(buftmp, len - 9);
}

bool compareMonoSysex(uint8_t *buf, uint8_t *buf2) {
	uint8_t buftmp[8192];
	uint8_t buftmp2[8192];
	uint16_t len = 0x1978;
	MNMSysexDecoder decoder(DATA_ENCODER_INIT(buf + 10, len - 10));
	decoder.get(buftmp + 1, len - 10);
	MNMSysexDecoder decoder2(DATA_ENCODER_INIT(buf2 + 10,  - 10));
	decoder2.get(buftmp2 + 1, len - 10);
	for (uint16_t i = 1; i < len - 10; i++) {
		if (buftmp[i] != buftmp2[i]) {
			printf("mono sysex different at 0x%x, %x != %x\n", i, buftmp[i], buftmp2[i]);
			return false;
		}
	}
	return true;
}

size_t readFile(const char *name, uint8_t *buf, uint32_t len) {
	FILE *f = fopen(name, "r");
	if (f == NULL) {
		return 0;
	}
	size_t ret = fread(buf, 1, len, f);
	fclose(f);
	return ret;
}

TEST_F (MNMPatternFixture, MNMPatternReadElektronPattern) {
	uint8_t buf[8192];
	size_t len = readFile("syx/mnm-pat.syx", buf, sizeof(buf));
	CHECK(len != 0);
	bool ret = pattern.fromSysex(buf + 6, len - 7);
	CHECK(ret);
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
	//	dumpMonoSysex(buf, 0x2a5);

	bool ret = p2.fromSysex(buf + 6, len - 7);
	CHECK(ret);

	CHECK_EQUAL(pattern.origPosition, (p2).origPosition);
	CHECK_EQUAL(pattern.swingAmount, (p2).swingAmount);
	CHECK_EQUAL(pattern.patternLength, (p2).patternLength);
	CHECK_EQUAL(pattern.doubleTempo, (p2).doubleTempo);
	CHECK_EQUAL(pattern.doubleTempo, (p2).doubleTempo);
	CHECK_EQUAL(pattern.kit, (p2).kit);

	for (uint8_t track = 0; track < 1; track++) {
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

		for (uint8_t param = 0; param < 1; param++) {
			printf("param: %d\n", param);
			CHECK_EQUAL(pattern.isParamLocked(track, param),
									(p2).isParamLocked(track, param));
			if (pattern.isParamLocked(track, param)) {
				for (uint8_t step = 0; step < 64; step++) {
					CHECK_EQUAL((int)pattern.getLock(track, step, param),
											(int)(p2).getLock(track, step, param));
				}
			}
		}
	}

	uint16_t len2 = p2.toSysex(buf2, sizeof(buf2));
	CHECK_EQUAL(len, len2);

	ret = compareMonoSysex(buf, buf2);
	CHECK(ret);
	if (!ret) {
		printf("old one\n");
		dumpMonoSysex(buf, 0x2a5);
		printf("new one\n");
		dumpMonoSysex(buf2, 0x2a5);
	}
}

TEST_F (MNMPatternFixture, MNMPatternReadElektronPatternReimport) {
	uint8_t buf[8192];
	size_t len = readFile("syx/mnm-pat-full-a01.syx", buf, sizeof(buf));
	CHECK(len != 0);
	bool ret = pattern.fromSysex(buf + 6, len - 7);
	CHECK(ret);

	uint8_t buf2[8192];
	uint16_t len2 = pattern.toSysex(buf2, sizeof(buf2));
	//	dumpMonoSysex(buf, 0x2a5);

	MNMPattern p2;
	ret = p2.fromSysex(buf2 + 6, len2 - 7);
	CHECK(ret);

	ret = compareMonoSysex(buf, buf2);
	CHECK(ret);
	if (!ret) {
		printf("old one\n");
		dumpMonoSysex(buf, 0x2a5);
		printf("new one\n");
		dumpMonoSysex(buf2, 0x2a5);
	}

	CHECK_EQUAL(pattern.origPosition, (p2).origPosition);
	CHECK_EQUAL(pattern.swingAmount, (p2).swingAmount);
	CHECK_EQUAL(pattern.patternLength, (p2).patternLength);
	CHECK_EQUAL(pattern.doubleTempo, (p2).doubleTempo);
	CHECK_EQUAL(pattern.doubleTempo, (p2).doubleTempo);
	CHECK_EQUAL(pattern.kit, (p2).kit);

	for (uint8_t track = 0; track < 1; track++) {
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

		for (uint8_t param = 0; param < 1; param++) {
			printf("param: %d\n", param);
			CHECK_EQUAL(pattern.isParamLocked(track, param),
									(p2).isParamLocked(track, param));
			if (pattern.isParamLocked(track, param)) {
				for (uint8_t step = 0; step < 64; step++) {
					CHECK_EQUAL((int)pattern.getLock(track, step, param),
											(int)(p2).getLock(track, step, param));
				}
			}
		}
	}

}

TEST_F (MNMPatternFixture, MNMPatternSetTrig) {
	for (uint8_t track = 0; track < MAX_TRACKS; track++) {
		pattern.setTrig(track, 0);
		CHECK(pattern.isTrigSet(track, 0));
		CHECK(!pattern.isTrackEmpty(track));
		for (uint8_t i = 0; i < MAX_TRACKS; i++) {
			if (i != track) {
				CHECK(pattern.isTrackEmpty(i));
			}
		}
		
		pattern.clearTrig(track, 0);
		CHECK(!pattern.isTrigSet(track, 0));
		CHECK(pattern.isTrackEmpty(track));
		for (uint8_t i = 0; i < MAX_TRACKS; i++) {
			if (i != track) {
				CHECK(pattern.isTrackEmpty(i));
			}
		}
	
		pattern.setTrig(track, 0);
		CHECK(pattern.isTrigSet(track, 0));
		CHECK(!pattern.isTrackEmpty(track));
	
		pattern.clearTrack(track);
		CHECK(!pattern.isTrigSet(track, 0));
		CHECK(pattern.isTrackEmpty(track));
	}
}

TEST_F (MNMPatternFixture, MNMPatternSetTrigSysex) {
	for (uint8_t track = 0; track < MAX_TRACKS; track++) {
		pattern.setTrig(track, 0);
		bool ret = reimportSysex(&pattern);
		CHECK(ret);
		CHECK(pattern.isTrigSet(track, 0));
		CHECK(!pattern.isTrackEmpty(track));
		for (uint8_t i = 0; i < MAX_TRACKS; i++) {
			if (i != track) {
				CHECK(pattern.isTrackEmpty(i));
			}
		}
		
		pattern.clearTrig(track, 0);
		ret = reimportSysex(&pattern);
		CHECK(ret);
		CHECK(!pattern.isTrigSet(track, 0));
		CHECK(pattern.isTrackEmpty(track));
		for (uint8_t i = 0; i < MAX_TRACKS; i++) {
			if (i != track) {
				CHECK(pattern.isTrackEmpty(i));
			}
		}
	
		pattern.setTrig(track, 0);
		ret = reimportSysex(&pattern);
		CHECK(ret);
		CHECK(pattern.isTrigSet(track, 0));
		CHECK(!pattern.isTrackEmpty(track));
	
		pattern.clearTrack(track);
		ret = reimportSysex(&pattern);
		CHECK(ret);
		CHECK(!pattern.isTrigSet(track, 0));
		CHECK(pattern.isTrackEmpty(track));
	}
}

TEST_F (MNMPatternFixture, MNMPatternSingleLock) {
	for (uint8_t track = 0 ; track < MAX_TRACKS ; track++) {
		CHECK_EQUAL(255, pattern.getLock(track, 0, 0));
		pattern.setTrig(track, 0);
		pattern.addLock(track, 0, 0, 100);
		CHECK_EQUAL(100, pattern.getLock(track, 0, 0));
		bool ret = reimportSysex(&pattern);
		CHECK(ret);
		CHECK_EQUAL(100, (int)pattern.getLock(track, 0, 0));

		pattern.clearLock(track, 0, 0);
		CHECK_EQUAL(255, pattern.getLock(track, 0, 0));
		ret = reimportSysex(&pattern);
		CHECK(ret);
		CHECK_EQUAL(255, (int)pattern.getLock(track, 0, 0));
	}
}

#ifdef DEBUG
TEST_F (MNMPatternFixture, MNMPatternSingleLockDebug) {
	PrintMNMPattern pattern;
	pattern.init();
	
	for (uint8_t track = 0; track < 2 ; track++) {
		printf("\n\ntrack: %d\n", track);
		pattern.addLock(track, 0, 0, 100);
		printf("add\n");
		pattern.printLocks(track);
		pattern.recalculateLockPatterns();
		printf("recalculate\n");
		pattern.printLocks(track);
		
		bool ret = reimportSysex(&pattern);
		printf("reimport\n");
		pattern.printLocks(track);

		pattern.clearLock(track, 0, 0);
		printf("clear\n");
		pattern.printLocks(track);

		pattern.recalculateLockPatterns();
		printf("recalculate\n");
		pattern.printLocks(track);

		ret = reimportSysex(&pattern);
		printf("reimport\n");
		pattern.printLocks(track);

		CHECK_EQUAL(255, (int)pattern.getLock(track, 0, 0));
	}
}
#endif

TEST_F (MNMPatternFixture, MNMPatternTwoParamLocks) {
	for (uint8_t track = 0 ; track < MAX_TRACKS ; track++) {
		CHECK_EQUAL(255, pattern.getLock(track, 0, 0));
		CHECK_EQUAL(255, pattern.getLock(track, 0, 1));

		pattern.addLock(track, 0, 0, 100);
		pattern.addLock(track, 0, 1, 101);
		CHECK_EQUAL(100, pattern.getLock(track, 0, 0));
		CHECK_EQUAL(101, pattern.getLock(track, 0, 1));
		
		pattern.clearLock(track, 0, 0);
		pattern.clearLock(track, 0, 1);
		CHECK_EQUAL(255, pattern.getLock(track, 0, 0));
		CHECK_EQUAL(255, pattern.getLock(track, 0, 1));
	}
}

TEST_F (MNMPatternFixture, MNMPatternTwoParamLocksNoClear) {
	for (uint8_t track = 0 ; track < MAX_TRACKS ; track++) {
		CHECK_EQUAL(255, pattern.getLock(track, 0, 0));
		CHECK_EQUAL(255, pattern.getLock(track, 0, 1));

		pattern.addLock(track, 0, 0, 100);
		pattern.addLock(track, 0, 1, 101);
		CHECK_EQUAL(100, pattern.getLock(track, 0, 0));
		CHECK_EQUAL(101, pattern.getLock(track, 0, 1));
		
	}

	for (uint8_t track = 0 ; track < MAX_TRACKS ; track++) {
		CHECK_EQUAL(100, pattern.getLock(track, 0, 0));
		CHECK_EQUAL(101, pattern.getLock(track, 0, 1));
	}
}

TEST_F (MNMPatternFixture, MNMPatternOneParamMoreLocks) {
	for (uint8_t track = 0 ; track < MAX_TRACKS ; track++) {
		for (uint8_t step = 0; step < 32; step++) {
			CHECK_EQUAL(255, pattern.getLock(track, step, 0));

			pattern.addLock(track, step, 0, step * 2);
			CHECK_EQUAL(step * 2, pattern.getLock(track, step, 0));
		}

		for (uint8_t step = 0; step < 32; step++) {
			CHECK_EQUAL(step * 2, pattern.getLock(track, step, 0));
		}

		for (uint8_t step = 0; step < 32; step++) {
			pattern.clearLock(track, step, 0);
			CHECK_EQUAL(255, pattern.getLock(track, step, 0));
		}
	}
}

TEST_F (MNMPatternFixture, MNMPatternOneParamMoreLocksNoClear) {
	for (uint8_t track = 0 ; track < MAX_TRACKS ; track++) {
		for (uint8_t step = 0; step < 32; step++) {
			CHECK_EQUAL(255, pattern.getLock(track, step, 0));

			pattern.addLock(track, step, 0, step * 2);
			CHECK_EQUAL(step * 2, pattern.getLock(track, step, 0));
		}

		for (uint8_t step = 0; step < 32; step++) {
			CHECK_EQUAL(step * 2, pattern.getLock(track, step, 0));
		}
	}

	for (uint8_t track = 0 ; track < MAX_TRACKS ; track++) {
		for (uint8_t step = 0; step < 32; step++) {
			CHECK_EQUAL(step * 2, pattern.getLock(track, step, 0));
		}
	}
}

TEST_F (MNMPatternFixture, MNMPatternAllParameters) {
	uint8_t maxTrack = 2;
	for (uint8_t track = 0 ; track < maxTrack ; track++) {
		for (uint8_t step = 0; step < 32; step++) {
			CHECK_EQUAL(255, (int)pattern.getLock(track, step, 0));

			pattern.addLock(track, step, 0, step * 2);
			pattern.setTrig(track, step);
			CHECK_EQUAL(step * 2, (int)pattern.getLock(track, step, 0));
		}

		for (uint8_t step = 0; step < 32; step++) {
			CHECK_EQUAL(step * 2, (int)pattern.getLock(track, step, 0));
		}
	}

	for (uint8_t track = 0 ; track < maxTrack ; track++) {
		for (uint8_t step = 0; step < 32; step++) {
			CHECK_EQUAL(step * 2, (int)pattern.getLock(track, step, 0));
		}
	}

		
	bool ret = reimportSysex(&pattern);
	CHECK(ret);
	for (uint8_t track = 0 ; track < maxTrack ; track++) {
		for (uint8_t step = 0; step < 32; step++) {
			CHECK_EQUAL(step * 2, (int)pattern.getLock(track, step, 0));
		}
	}

}

TEST_F (MNMPatternFixture, MNMLongPatternTrig) {
	pattern.patternLength = 64;
	pattern.setTrig(0, 32);
	CHECK(pattern.isTrigSet(0, 32));

	bool ret = reimportSysex(&pattern);
	CHECK(ret);
	CHECK(pattern.isTrigSet(0, 32));
}

TEST_F (MNMPatternFixture, MNMPatternAllParameters64) {
	pattern.patternLength = 64;
	uint8_t maxTrack = 2;
	for (uint8_t track = 0 ; track < maxTrack ; track++) {
		for (uint8_t step = 0; step < pattern.patternLength; step++) {
			CHECK_EQUAL(255, (int)pattern.getLock(track, step, 0));

			pattern.addLock(track, step, 0, step + track);
			pattern.setTrig(track, step);
			CHECK_EQUAL(step + track, (int)pattern.getLock(track, step, 0));
		}

		for (uint8_t step = 0; step < pattern.patternLength; step++) {
			CHECK_EQUAL(step + track, (int)pattern.getLock(track, step, 0));
		}
	}

	for (uint8_t track = 0 ; track < maxTrack ; track++) {
		for (uint8_t step = 0; step < pattern.patternLength; step++) {
			CHECK_EQUAL(step + track, (int)pattern.getLock(track, step, 0));
		}
	}

	//	printf("reimport\n");
		
	bool ret = reimportSysex(&pattern);
	CHECK(ret);
	for (uint8_t track = 0 ; track < maxTrack ; track++) {
		//		printf("track: %d\n", track);
		for (uint8_t step = 0; step < pattern.patternLength; step++) {
			CHECK_EQUAL(step + track, (int)pattern.getLock(track, step, 0));
		}
	}
}

TEST_F (MNMPatternFixture, MNMPatternClearLock) {
	pattern.patternLength = 64;

	uint8_t maxTrack = MAX_TRACKS;
	for (uint8_t track = 0 ; track < maxTrack ; track++) {
		for (uint8_t step = 0; step < pattern.patternLength; step++) {
			CHECK_EQUAL(255, (int)pattern.getLock(track, step, 0));

			pattern.addLock(track, step, 0, step + track);
			pattern.setTrig(track, step);
			CHECK_EQUAL(step + track, (int)pattern.getLock(track, step, 0));
		}

		for (uint8_t step = 0; step < pattern.patternLength; step++) {
			CHECK_EQUAL(step + track, (int)pattern.getLock(track, step, 0));
		}
	}

	for (uint8_t track = 0 ; track < maxTrack ; track++) {
		pattern.clearTrack(track);
		for (uint8_t step = 0; step < pattern.patternLength; step++) {
			CHECK(!pattern.isTrigSet(track, step));
			CHECK_EQUAL(255, (int)pattern.getLock(track, step, 0));
		}
	}

	bool ret = reimportSysex(&pattern);
	CHECK(ret);
	for (uint8_t track = 0 ; track < maxTrack ; track++) {
		for (uint8_t step = 0; step < pattern.patternLength; step++) {
			CHECK(!pattern.isTrigSet(track, step));
			CHECK_EQUAL(255, (int)pattern.getLock(track, step, 0));
		}
	}
}


TEST_F (MNMPatternFixture, MNMPatternClearLockSkip) {
	pattern.patternLength = 64;

	uint8_t maxTrack = MAX_TRACKS;
	for (uint8_t track = 0 ; track < maxTrack ; track += 2) {
		for (uint8_t step = 0; step < pattern.patternLength; step++) {
			CHECK_EQUAL(255, (int)pattern.getLock(track, step, 0));

			pattern.addLock(track, step, 0, step + track);
			pattern.setTrig(track, step);
			CHECK_EQUAL(step + track, (int)pattern.getLock(track, step, 0));
		}

		for (uint8_t step = 0; step < pattern.patternLength; step++) {
			CHECK_EQUAL(step + track, (int)pattern.getLock(track, step, 0));
		}
	}

	for (uint8_t track = 0 ; track < maxTrack ; track += 2) {
		pattern.clearTrack(track);
		for (uint8_t step = 0; step < pattern.patternLength; step++) {
			CHECK(!pattern.isTrigSet(track, step));
			CHECK_EQUAL(255, (int)pattern.getLock(track, step, 0));
		}
	}

	bool ret = reimportSysex(&pattern);
	CHECK(ret);
	for (uint8_t track = 0 ; track < maxTrack ; track += 2) {
		for (uint8_t step = 0; step < pattern.patternLength; step++) {
			CHECK(!pattern.isTrigSet(track, step));
			CHECK_EQUAL(255, (int)pattern.getLock(track, step, 0));
		}
	}
}

TEST_F (MNMPatternFixture, MNMPatternClearLockSkipClearOne) {
	PrintMNMPattern pattern;
	pattern.init();
	pattern.patternLength = 64;

	uint8_t maxTrack = 4;
	for (uint8_t track = 0 ; track < maxTrack ; track += 2) {
		for (uint8_t step = 0; step < pattern.patternLength; step++) {
			CHECK_EQUAL(255, (int)pattern.getLock(track, step, 0));

			pattern.addLock(track, step, 0, step + track);
			pattern.setTrig(track, step);
			CHECK(pattern.isTrigSet(track, step));
			CHECK_EQUAL(step + track, (int)pattern.getLock(track, step, 0));
		}

		for (uint8_t step = 0; step < pattern.patternLength; step++) {
			CHECK_EQUAL(step + track, (int)pattern.getLock(track, step, 0));
		}
	}

	uint8_t clearTrack = 0;
	pattern.clearTrack(clearTrack);
	for (uint8_t step = 0; step < pattern.patternLength; step++) {
		CHECK(!pattern.isTrigSet(clearTrack, step));
		CHECK_EQUAL(255, (int)pattern.getLock(clearTrack, step, 0));
	}

	pattern.recalculateLockPatterns();
	for (uint8_t track = 0 ; track < maxTrack ; track += 2) {
		for (uint8_t step = 0; step < pattern.patternLength; step++) {
			if (track == clearTrack) {
				CHECK(!pattern.isTrigSet(clearTrack, step));
				CHECK_EQUAL(255, (int)pattern.getLock(clearTrack, step, 0));
			} else {
				CHECK(pattern.isTrigSet(track, step));
				CHECK_EQUAL(step + track, (int)pattern.getLock(track, step, 0));
			}
		}
	}
	//	pattern.print();

	//	printf("reimport\n");
	bool ret = reimportSysex(&pattern);
	CHECK(ret);
	//	pattern.print();
	for (uint8_t track = 0 ; track < maxTrack ; track += 2) {
		for (uint8_t step = 0; step < pattern.patternLength; step++) {
			if (track == clearTrack) {
				CHECK(!pattern.isTrigSet(clearTrack, step));
				CHECK_EQUAL(255, (int)pattern.getLock(clearTrack, step, 0));
			} else {
				CHECK(pattern.isTrigSet(track, step));
				CHECK_EQUAL(step + track, (int)pattern.getLock(track, step, 0));
			}
		}
	}
}


#ifdef DEBUG
TEST_F (MNMPatternFixture, MNMPatternAllParametersDebug) {
	PrintMNMPattern pattern;
	pattern.init();
	
	uint8_t maxTrack = 2;
	for (uint8_t track = 0 ; track < maxTrack ; track++) {
		for (uint8_t step = 0; step < 32; step++) {
			CHECK_EQUAL(255, (int)pattern.getLock(track, step, 0));

			pattern.addLock(track, step, 0, step * 2 + track);
			pattern.setTrig(track, step);
			CHECK_EQUAL(step * 2 + track, (int)pattern.getLock(track, step, 0));
		}

		for (uint8_t step = 0; step < 32; step++) {
			CHECK_EQUAL(step * 2 + track, (int)pattern.getLock(track, step, 0));
		}
	}

	for (uint8_t track = 0 ; track < maxTrack ; track++) {
		for (uint8_t step = 0; step < 32; step++) {
			CHECK_EQUAL(step * 2 + track, (int)pattern.getLock(track, step, 0));
		}
	}

	printf("before\n");
	for (uint8_t track = 0 ; track < maxTrack ; track++) {
		printf("track: %d\n", track);
		pattern.printLocks(track);
	}
	pattern.recalculateLockPatterns();

	printf("recalc\n");
	for (uint8_t track = 0 ; track < maxTrack ; track++) {
		printf("track: %d\n", track);
		pattern.printLocks(track);
	}
	
	printf("after\n");
	bool ret = reimportSysex(&pattern);
	for (uint8_t track = 0 ; track < maxTrack ; track++) {
		printf("track: %d\n", track);
		pattern.printLocks(track);
	}
}
#endif
