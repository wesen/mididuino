/*
 * MidiCtrl - MD Pattern unit tests
 *
 * (c) 2010 - Manuel Odendahl - wesen@ruinwesen.com
 */


#include <CppUnitLite2.h>
#include <TestResultStdErr.h>

#include "WProgram.h"
#include <MD.h>

#include "test_helpers.h"

/***************************************************************************
 *
 * Comparison and initialization functions
 *
 ***************************************************************************/

void print_pattern_track(MDPattern &pat, uint8_t track) {
  for (uint32_t i = 0; i < pat.patternLength; i++) {
    if (IS_BIT_SET64(pat.trigPatterns[track], i)) {
      printf("X   ");
    } else {
      printf(".   ");
    }
  }
  printf("\n");
}

void print_pattern_locks(MDPattern &pat, uint8_t track) {
  for (uint8_t param = 0; param < 24; param++) {
    if (pat.isParamLocked(track, param)) {
      printf("P%.2d (idx: %d): ", param, pat.paramLocks[track][param]);
      for (uint8_t i = 0; i < pat.patternLength; i++) {
        uint8_t lock = pat.getLock(track, i, param);
        if (lock != 255) {
          printf("%.3d ", lock);
        } else {
          printf("    ");
        }
      }
      printf("\n");
    }
  }
}

bool cmp_patterns(MDPattern &pat, MDPattern &pat2) {
  M_CHECK_EQUAL(pat.origPosition, pat2.origPosition);
  M_CHECK_EQUAL_L(pat.accentPattern, pat2.accentPattern);                       
  M_CHECK_EQUAL_L(pat.slidePattern, pat2.slidePattern);                         
  M_CHECK_EQUAL_L(pat.swingPattern, pat2.swingPattern);                         
  M_CHECK_EQUAL(pat.swingAmount, pat2.swingAmount);                           
  M_CHECK_EQUAL(pat.accentAmount, pat2.accentAmount);                         
  M_CHECK_EQUAL(pat.patternLength, pat2.patternLength);                       
  M_CHECK_EQUAL(pat.doubleTempo, pat2.doubleTempo);                           
  M_CHECK_EQUAL(pat.scale, pat2.scale);                                       
  M_CHECK_EQUAL(pat.kit, pat2.kit);                                           
  
  for (uint8_t track = 0; track < 16; track++) {
    M_CHECK_EQUAL_L(pat.trigPatterns[track], pat2.trigPatterns[track]);         
    M_CHECK_EQUAL(pat.lockPatterns[track], pat2.lockPatterns[track]);         
    M_CHECK_EQUAL_L(pat.accentPatterns[track], pat2.accentPatterns[track]);     
    M_CHECK_EQUAL_L(pat.slidePatterns[track], pat2.slidePatterns[track]);       
    M_CHECK_EQUAL_L(pat.swingPatterns[track], pat2.swingPatterns[track]);       

    for (uint8_t param = 0; param < 24; param++) {                         
      M_CHECK_EQUAL(pat.isParamLocked(track, param),                         
                    pat2.isParamLocked(track, param));                        
      for (uint8_t step = 0; step < pat.patternLength; step++) {                          
        M_CHECK_EQUAL(pat.getLock(track, step, param),                       
                      pat2.getLock(track, step, param));                      
      }                               
    } 
  }

  
  return true;
}

bool cmp_pattern_tracks(MDPattern &pat, uint8_t idx1,
                        MDPattern &pat2, uint8_t idx2) {
  M_CHECK_EQUAL_L(pat.trigPatterns[idx1], pat2.trigPatterns[idx2]);
  M_CHECK_EQUAL(pat.lockPatterns[idx1], pat2.lockPatterns[idx2]);
  M_CHECK_EQUAL_L(pat.accentPatterns[idx1], pat2.accentPatterns[idx2]);
  M_CHECK_EQUAL_L(pat.slidePatterns[idx1], pat2.slidePatterns[idx2]);
  M_CHECK_EQUAL_L(pat.swingPatterns[idx1], pat2.slidePatterns[idx2]);

  for (uint8_t param = 0; param < 24; param++) {
    M_CHECK_EQUAL(pat.isParamLocked(idx1, param),
                  pat2.isParamLocked(idx2, param));
    for (uint8_t step = 0; step < pat.patternLength; step++) {
      M_CHECK_EQUAL(pat.getLock(idx1, step, param),
                    pat2.getLock(idx1, step, param));
    }
  }

  return true;
}

/***************************************************************************
 *
 * Test Fixture
 *
 ***************************************************************************/

struct MDPatternFixture {
  MDPatternFixture() {
    pattern.clearPattern();
  }

  MDPattern pattern;
  MDPattern pattern2;
};

/***************************************************************************
 *
 * MD Pattern tests
 *
 ***************************************************************************/

/** Check that a pattern is initially empty. **/
TEST_F (MDPatternFixture, MDPatternTestInit) {
  pattern.clearPattern();
  for (uint8_t i = 0; i < 16; i++) {
    CHECK(pattern.isTrackEmpty(i));
  }
}


/**
 * Check that an empty pattern is converted to and from sysex correctly.
 **/
TEST_F (MDPatternFixture, MDPatternEmptyToFromSysex) {
  MDPattern p2;
  uint8_t buf[8192], buf2[8192]; 
  uint16_t len = pattern.toSysex(buf, sizeof(buf));         
  bool ret = (p2).fromSysex(buf + 6, len - 7);          
  CHECK(ret);                                               
  uint16_t len2 = (p2).toSysex(buf2, sizeof(buf2));     
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

bool reimportSysex(MDPattern *p) {
  uint8_t buf[8192];
  uint16_t len = p->toSysex(buf, sizeof(buf));
  return p->fromSysex(buf + 6, len - 7);
}

/**
 * Check that setting and clearing triggers in a track pattern works correctly.
 **/
TEST_F (MDPatternFixture, MDPatternSetTrig) {
  for (uint8_t track = 0; track < 16; track++) {
    pattern.setTrig(track, 0);
    CHECK(pattern.isTrigSet(track, 0));
    CHECK(!pattern.isTrackEmpty(track));
    for (uint8_t i = 0; i < 16; i++) {
      if (i != track) {
        CHECK(pattern.isTrackEmpty(i));
      }
    }
		
    pattern.clearTrig(track, 0);
    CHECK(!pattern.isTrigSet(track, 0));
    CHECK(pattern.isTrackEmpty(track));
    for (uint8_t i = 0; i < 16; i++) {
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

/**
 * Check that exporting and reimporting triggers set in a pattern works correctly.
 **/
TEST_F (MDPatternFixture, MDPatternSetTrigSysex) {
  for (uint8_t track = 0; track < 16; track++) {
    pattern.setTrig(track, 0);
    bool ret = reimportSysex(&pattern);
    CHECK(ret);
    CHECK(pattern.isTrigSet(track, 0));
    CHECK(!pattern.isTrackEmpty(track));
    for (uint8_t i = 0; i < 16; i++) {
      if (i != track) {
        CHECK(pattern.isTrackEmpty(i));
      }
    }
		
    pattern.clearTrig(track, 0);
    ret = reimportSysex(&pattern);
    CHECK(ret);
    CHECK(!pattern.isTrigSet(track, 0));
    CHECK(pattern.isTrackEmpty(track));
    for (uint8_t i = 0; i < 16; i++) {
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

/**
 * Check that setting a single param lock works correctly.
 **/
TEST_F (MDPatternFixture, MDPatternSingleLock) {
  for (uint8_t track = 0 ; track < 16 ; track++) {
    CHECK_EQUAL(255, pattern.getLock(track, 0, 0));
    pattern.setTrig(track, 0);
    pattern.addLock(track, 0, 0, 100);
    CHECK_EQUAL(100, (int)pattern.getLock(track, 0, 0));
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
TEST_F (MDPatternFixture, MDPatternSingleLockDebug) {
  PrintMDPattern pattern;
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

TEST_F (MDPatternFixture, MDPatternTwoParamLocks) {
  for (uint8_t track = 0 ; track < 16 ; track++) {
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

TEST_F (MDPatternFixture, MDPatternTwoParamLocksNoClear) {
  for (uint8_t track = 0 ; track < 16 ; track++) {
    CHECK_EQUAL(255, pattern.getLock(track, 0, 0));
    CHECK_EQUAL(255, pattern.getLock(track, 0, 1));

    pattern.addLock(track, 0, 0, 100);
    pattern.addLock(track, 0, 1, 101);
    CHECK_EQUAL(100, pattern.getLock(track, 0, 0));
    CHECK_EQUAL(101, pattern.getLock(track, 0, 1));
		
  }

  for (uint8_t track = 0 ; track < 16 ; track++) {
    CHECK_EQUAL(100, pattern.getLock(track, 0, 0));
    CHECK_EQUAL(101, pattern.getLock(track, 0, 1));
  }
}

TEST_F (MDPatternFixture, MDPatternOneParamMoreLocks) {
  for (uint8_t track = 0 ; track < 16 ; track++) {
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

TEST_F (MDPatternFixture, MDPatternOneParamMoreLocksNoClear) {
  for (uint8_t track = 0 ; track < 16 ; track++) {
    for (uint8_t step = 0; step < 32; step++) {
      CHECK_EQUAL(255, pattern.getLock(track, step, 0));

      pattern.addLock(track, step, 0, step * 2);
      CHECK_EQUAL(step * 2, pattern.getLock(track, step, 0));
    }

    for (uint8_t step = 0; step < 32; step++) {
      CHECK_EQUAL(step * 2, pattern.getLock(track, step, 0));
    }
  }

  for (uint8_t track = 0 ; track < 16 ; track++) {
    for (uint8_t step = 0; step < 32; step++) {
      CHECK_EQUAL(step * 2, pattern.getLock(track, step, 0));
    }
  }
}

TEST_F (MDPatternFixture, MDPatternAllParameters) {
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

TEST_F (MDPatternFixture, MDLongPatternTrig) {
  pattern.patternLength = 64;
  pattern.setTrig(0, 32);
  CHECK(pattern.isTrigSet(0, 32));

  bool ret = reimportSysex(&pattern);
  CHECK(ret);
  CHECK(pattern.isTrigSet(0, 32));
}

TEST_F (MDPatternFixture, MDPatternAllParameters64) {
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

TEST_F (MDPatternFixture, MDPatternClearLock) {
  pattern.patternLength = 64;

  uint8_t maxTrack = 16;
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


TEST_F (MDPatternFixture, MDPatternClearLockSkip) {
  pattern.patternLength = 64;

  uint8_t maxTrack = 16;
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

TEST_F (MDPatternFixture, MDPatternClearLockSkipClearOne) {
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
TEST_F (MDPatternFixture, MDPatternAllParametersDebug) {
  PrintMDPattern pattern;
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
