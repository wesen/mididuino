#include <CppUnitLite2.h>
#include <TestResultStdErr.h>

#include <GUI.h>

/*
 * use:
 * - CHECK (condition)
 * - CHECK_EQUAL(expected, actual)
 */

struct EncoderPageFixture {
  EncoderPageFixture() : page(&encs[0], &encs[1], &encs[2], &encs[3]) {
  }

  ~EncoderPageFixture() {
  }

  RangeEncoder encs[4];
  EncoderPage page;
};

TEST_F (EncoderPageFixture, TestSingleEncoder) {
  CHECK_EQUAL(0, encs[0].getValue());
  CHECK_EQUAL(0, encs[1].getValue());
  CHECK_EQUAL(0, encs[2].getValue());
  CHECK_EQUAL(0, encs[3].getValue());
  
  Encoders.turnEncoder(0, 1);
  page.update();

  CHECK_EQUAL(1, encs[0].getValue());
  CHECK_EQUAL(0, encs[1].getValue());
  CHECK_EQUAL(0, encs[2].getValue());
  CHECK_EQUAL(0, encs[3].getValue());

  Encoders.turnEncoder(0, 3);
  page.update();

  CHECK_EQUAL(4, encs[0].getValue());

  Encoders.turnEncoder(0, -3);
  page.update();

  CHECK_EQUAL(1, encs[0].getValue());

  Encoders.turnEncoder(0, 100);
  page.update();

  CHECK_EQUAL(101, encs[0].getValue());

  Encoders.turnEncoder(0, 30);
  page.update();

  CHECK_EQUAL(127, encs[0].getValue());

  Encoders.turnEncoder(0, -70);
  page.update();

  CHECK_EQUAL(57, encs[0].getValue());

  Encoders.turnEncoder(0, -70);
  page.update();

  CHECK_EQUAL(0, encs[0].getValue());
}
