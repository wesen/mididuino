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
	for (uint8_t i = 0; i < 6; i++) {
		CHECK(pattern.isTrackEmpty(i));
	}
}

void dumpMonoSysex(uint8_t *buf, uint16_t len) {
	uint8_t buftmp[8192];
	MNMSysexDecoder decoder(buf + 10, len - 10);
	decoder.get(buftmp + 1, len - 10);
	hexdump(buftmp, len - 9);
}

bool compareMonoSysex(uint8_t *buf, uint8_t *buf2) {
	uint8_t buftmp[8192];
	uint8_t buftmp2[8192];
	uint16_t len = 0x1978;
	MNMSysexDecoder decoder(buf + 10, len - 10);
	decoder.get(buftmp + 1, len - 10);
	MNMSysexDecoder decoder2(buf2 + 10,  - 10);
	decoder2.get(buftmp2 + 1, len - 10);
	for (uint16_t i = 0; i < len - 10; i++) {
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
	pattern.init();
	MNMPattern p2;
	uint8_t buf[8192], buf2[8192];
	uint16_t len = pattern.toSysex(buf, sizeof(buf));

	bool ret = p2.fromSysex(buf + 6, len - 7);
	CHECK(ret);

	CHECK_EQUAL(pattern.origPosition, (p2).origPosition);
	CHECK_EQUAL(pattern.swingAmount, (p2).swingAmount);
	CHECK_EQUAL(pattern.length, (p2).length);
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
