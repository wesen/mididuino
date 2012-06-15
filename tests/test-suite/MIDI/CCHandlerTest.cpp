/*
 * CCHandlerTest.cpp
 *
 *  Created on: Jun 15, 2012
 *      Author: manuel
 */

#include "TestSuite.h"
#include "GUI.h"
#include "CCHandler.h"
#include "Midi.h"

struct CCHandlerFixture {
  CCEncoder encs[4];
  EncoderPage page;

  CCHandlerFixture() : page(&encs[0], &encs[1], &encs[2], &encs[3]) {
    Midi.init();
    Midi.initSerial();
    Midi.removeAllCallbacks();

    ccHandler.setup();

    for (uint8_t i = 0; i < countof(encs); i++) {
      encs[i].initCCEncoder(0, i);
      ccHandler.addEncoder(encs + i);
    }
  }
};

TEST_F (CCHandlerFixture, TestCCHandler) {
  uint8_t count;

  count = ccHandler.incomingCCs.size();
  CHECK_EQUAL(0, count);

  Midi.sendCC(0, 1, 100);
  count = ccHandler.incomingCCs.size();
  CHECK_EQUAL(1, count);
  incoming_cc_t cc;
  ccHandler.incomingCCs.getCopy(0, &cc);
  CHECK_EQUAL(0, cc.channel);
  CHECK_EQUAL(1, cc.cc);
  CHECK_EQUAL(100, cc.value);
}
