#include <CppUnitLite2.h>
#include <TestResultStdErr.h>

#include <GUI.h>

/*
 * use:
 * - CHECK (condition)
 * - CHECK_EQUAL(expected, actual)
 */

TEST (TestSingleEncoder) {
  CHECK(false);

  RangeEncoder enc;
  EncoderPage page(&enc);

  Encoders.turnEncoder(0, 1);
  page.update();
}


TEST (TestCurrentPageEncoders) {
  RangeEncoder enc;
  EncoderPage page(&enc);

  Encoders.turnEncoder(0, 1);
  page.update();
}
