#include "TestSuite.h"
#include "Midi.h"
#include "GUI.h"
#include "RecordingEncoder.hh"

struct AutoEncoderFixture {
  RecordingEncoder<64> encoder;
  RangeEncoder realEncoder;
  EncoderPage page;
  uint8_t lastValue = 0;
  bool handlerCalled = false;

  AutoEncoderFixture() : page(&encoder) {
    realEncoder.initRangeEncoder(127, 0, "name", 0,
                                 [&] (Encoder *enc) {
      lastValue = enc->getValue();
      handlerCalled = true;
    });
    encoder.initRecordingEncoder(&realEncoder);
    encoder.playback(0);
  }

  void turn(int steps) {
    Encoders.turnEncoder(0, steps);
    handlerCalled = false;
    page.update();
    page.finalize();
  }
};


TEST_F (AutoEncoderFixture, TestSimpleTurn) {
  CHECK_EQUAL(0, encoder.getValue());
  CHECK_EQUAL(0, lastValue);
  turn(1);
  CHECK_EQUAL(true, handlerCalled);
  CHECK_EQUAL(1, encoder.getValue());
  CHECK_EQUAL(1, realEncoder.getValue());
  CHECK_EQUAL(1, lastValue);
}

TEST_F (AutoEncoderFixture, TestEmptyPlayback) {
  encoder.playback(0);
  CHECK_EQUAL(false, handlerCalled);
  CHECK_EQUAL(0, lastValue);
  CHECK_EQUAL(0, encoder.getValue());
  CHECK_EQUAL(0, realEncoder.getValue());

  encoder.playback(1);
  CHECK_EQUAL(false, handlerCalled);
  CHECK_EQUAL(0, lastValue);
  CHECK_EQUAL(0, encoder.getValue());
  CHECK_EQUAL(0, realEncoder.getValue());
}

TEST_F (AutoEncoderFixture, TestSimpleRecording) {
  encoder.startRecording();

  encoder.playback(0);
  turn(1);

  encoder.playback(1);
  turn(1);

  encoder.stopRecording();

  handlerCalled = false;
  encoder.playback(0);
  CHECK_EQUAL(true, handlerCalled);
  CHECK_EQUAL(1, lastValue);
  CHECK_EQUAL(1, encoder.getValue());
  CHECK_EQUAL(1, realEncoder.getValue());

  encoder.playback(1);
  CHECK_EQUAL(true, handlerCalled);
  CHECK_EQUAL(2, lastValue);
  CHECK_EQUAL(2, encoder.getValue());
  CHECK_EQUAL(2, realEncoder.getValue());
}

TEST_F (AutoEncoderFixture, TestClearRecording) {
  encoder.startRecording();
  encoder.playback(0);
  turn(1);
  encoder.playback(1);
  turn(1);
  encoder.playback(2);
  turn(1);
  encoder.stopRecording();


  handlerCalled = false;

  encoder.clearRecording();
  encoder.playback(0);
  CHECK_EQUAL(false, handlerCalled);
  encoder.playback(1);
  CHECK_EQUAL(false, handlerCalled);
  encoder.playback(2);
  CHECK_EQUAL(false, handlerCalled);
}
