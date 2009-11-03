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
	uint8_t buf[8192], buf2[8192], buf3[8192];
	uint16_t len = pattern.toSysex(buf, sizeof(buf));
	uint8_t *ptr = buf2;
	MNMSysexToDataEncoder encoder(buf2, sizeof(buf2));
	for (uint16_t i = 6; i < len - 5; i++) {
		encoder.pack8(buf[i]);
	}
	uint16_t len2 = encoder.finish();
	bool ret = p2.fromSysex(buf2, len2);
	CHECK(ret);
	uint16_t len3 = p2.toSysex(buf3, sizeof(buf3));
	CHECK_EQUAL(len, len2);                                   
	CHECK_EQUAL(pattern.origPosition, (p2).origPosition);                         
	CHECK_EQUAL(pattern.accentPattern, (p2).accentPattern);                       
	CHECK_EQUAL(pattern.slidePattern, (p2).slidePattern);                         
	CHECK_EQUAL(pattern.swingPattern, (p2).swingPattern);                         
	CHECK_EQUAL(pattern.swingAmount, (p2).swingAmount);                           
	CHECK_EQUAL(pattern.accentAmount, (p2).accentAmount);                         
	CHECK_EQUAL(pattern.patternLength, (p2).patternLength);                       
	CHECK_EQUAL(pattern.doubleTempo, (p2).doubleTempo);                           
	CHECK_EQUAL(pattern.scale, (p2).scale);                                       
	CHECK_EQUAL(pattern.kit, (p2).kit);                                           
                                                                           
	for (uint8_t track = 0; track < 16; track++) {                           
		CHECK_EQUAL(pattern.trigPatterns[track], (p2).trigPatterns[track]);         
		CHECK_EQUAL(pattern.lockPatterns[track], (p2).lockPatterns[track]);         
		CHECK_EQUAL(pattern.accentPatterns[track], (p2).accentPatterns[track]);     
		CHECK_EQUAL(pattern.slidePatterns[track], (p2).slidePatterns[track]);       
		CHECK_EQUAL(pattern.swingPatterns[track], (p2).swingPatterns[track]);       
  
		for (uint8_t param = 0; param < 24; param++) {                         
			CHECK_EQUAL(pattern.isParamLocked(track, param),                         
									(p2).isParamLocked(track, param));                        
			for (uint8_t step = 0; step < 32; step++) {                          
				CHECK_EQUAL(pattern.getLock(track, step, param),                       
										(p2).getLock(track, step, param));                      
			}                               
		} 
	} 
	
}
