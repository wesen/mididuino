/*
 * MidiCtrl - MDGlobal unit tests
 *
 * (c) 2010 - Manuel Odendahl - wesen@ruinwesen.com
 */

#include "TestSuite.h"

#include <MD.h>

/***************************************************************************
 *
 * Initialization functions for MD global tests
 *
 ***************************************************************************/

void initMDGlobal(MDGlobal &global) {
  global.origPosition = 0;
  for (uint8_t i = 0; i < 16; i++) {
    global.drumRouting[i] = i;
    global.drumMapping[i] = i;
  }
  for (uint8_t i = 0; i < 128; i++) {
    global.keyMap[i] = i;
  }

  global.baseChannel = 2;
  global.tempo = 200;
  global.extendedMode = false;
  global.clockIn = true;
  global.clockOut = false;
  global.transportIn = true;
  global.transportOut = false;
  global.localOn = true;

  global.drumLeft = 3;
  global.drumRight = 4;
  global.gateLeft = 5;
  global.gateRight = 6;
  global.senseLeft = 7;
  global.senseRight = 8;
  global.minLevelLeft = 9;
  global.minLevelRight = 10;
  global.maxLevelLeft = 11;
  global.maxLevelRight = 12;

  global.programChange = 13;
  global.trigMode = 14;
}

bool cmp_globals(MDGlobal &global, MDGlobal &global2) {
  M_CHECK_EQUAL(global.origPosition, global2.origPosition);
  for (uint8_t i = 0; i < 16; i++) {
    M_CHECK_EQUAL(global.drumRouting[i], global2.drumRouting[i]);
    M_CHECK_EQUAL(global.drumMapping[i], global2.drumMapping[i]);
  }
  for (uint8_t i = 0; i < 128; i++) {
    M_CHECK_EQUAL(global.keyMap[i], global2.keyMap[i]);
  }

  M_CHECK_EQUAL(global.baseChannel, global2.baseChannel);

  M_CHECK_EQUAL(global.tempo, global2.tempo);
  M_CHECK_EQUAL(global.extendedMode, global2.extendedMode);
  M_CHECK_EQUAL(global.clockIn, global2.clockIn);
  M_CHECK_EQUAL(global.clockOut, global2.clockOut);
  M_CHECK_EQUAL(global.transportIn, global2.transportIn);
  M_CHECK_EQUAL(global.transportOut, global2.transportOut);
  M_CHECK_EQUAL(global.localOn, global2.localOn);
  M_CHECK_EQUAL(global.drumLeft, global2.drumLeft);
  M_CHECK_EQUAL(global.drumRight, global2.drumRight);
  M_CHECK_EQUAL(global.gateLeft, global2.gateLeft);
  M_CHECK_EQUAL(global.gateRight, global2.gateRight);
  M_CHECK_EQUAL(global.senseLeft, global2.senseLeft);
  M_CHECK_EQUAL(global.senseRight, global2.senseRight);
  M_CHECK_EQUAL(global.minLevelLeft, global2.minLevelLeft);
  M_CHECK_EQUAL(global.minLevelRight, global2.minLevelRight);
  M_CHECK_EQUAL(global.maxLevelLeft, global2.maxLevelLeft);
  M_CHECK_EQUAL(global.maxLevelRight, global2.maxLevelRight);
  M_CHECK_EQUAL(global.programChange, global2.programChange);
  M_CHECK_EQUAL(global.trigMode, global2.trigMode);

  return true;
}

/***************************************************************************
 *
 * MD Global tests
 *
 ***************************************************************************/

struct MDGlobalFixture {
  MDGlobalFixture() {
    initMDGlobal(global);
  }

  MDGlobal global;
  MDGlobal global2;
};

TEST_F (MDGlobalFixture, MDGlobalSysex) {
  uint8_t data[4092];
  uint16_t len = global.toSysex(data, sizeof(data));
  bool ret = global2.fromSysex(data + 6, len - 7);
  CHECK(ret);
  ret = cmp_globals(global, global2);
  CHECK(ret);
}

