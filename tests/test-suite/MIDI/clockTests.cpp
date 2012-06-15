#include "TestSuite.h"

#include <MidiClock.h>
#include <helpers.h>

TEST (clockDiff1) {
  for (uint16_t i = 0; i < 65535; i++) {
    for (uint16_t j = 1; j < 5; j++) {
      if (clock_diff(i, i+j) != j) {
        printf("%d failed for %d, clock_diff(%d, %d) = %d\n", i, j, i, i+j, clock_diff(i, i+j));
      }
      CHECK_EQUAL(j, clock_diff(i, i + j));
    }
    for (uint16_t j = 65200; j < 65535; j++) {
      if (clock_diff(i, i+j) != j) {
        printf("%d failed for %d, clock_diff(%d, %d) = %d\n", i, j, i, i+j, clock_diff(i, i+j));
      }
      CHECK_EQUAL(j, clock_diff(i, i + j));
    }
  }
}

class CallbackTester : public ClockCallback {
public:
  uint32_t last16Counter;
  uint32_t last32Counter;

  CallbackTester() {
    reset();
  }

  void setup(MidiClockClass *clock) {
    clock->addOn16Callback(this, (midi_clock_callback_ptr_t)(&CallbackTester::on16Callback));
    clock->addOn32Callback(this, (midi_clock_callback_ptr_t)(&CallbackTester::on32Callback));
  }

  void on16Callback(uint32_t cnt) {
    last16Counter = cnt;
  }

  void on32Callback(uint32_t cnt) {
    last32Counter = cnt;
  }

  void reset() {
    last16Counter = 0;
    last32Counter = 0;
  }
};

struct MidiClockInternalFixture {
  MidiClockInternalFixture() {
    clock.init();
    clock.mode = clock.INTERNAL;
    clock.useImmediateClock = false;
    clock.start();
    tester.setup(&clock);
  }

  CallbackTester tester;
  MidiClockClass clock;
};

TEST_F (MidiClockInternalFixture, MidiClockTest) {
  clock.handleMidiStart();
  CHECK_EQUAL((uint32_t)0, clock.div96th_counter);
  clock.handleTimerInt();
  CHECK_EQUAL((uint32_t)0, clock.div96th_counter);
  clock.handleTimerInt();
  CHECK_EQUAL((uint32_t)1, clock.div96th_counter);
}

TEST_F (MidiClockInternalFixture, MidiClockCallbackTest) {
  for (uint8_t i = 0; i < 126; i++) {
    clock.handleTimerInt();
    CHECK_EQUAL(clock.div16th_counter, tester.last16Counter);
    CHECK_EQUAL(clock.div32th_counter, tester.last32Counter);
    CHECK_EQUAL((i + 1) / 6, tester.last16Counter);
    CHECK_EQUAL((i + 1) / 3, tester.last32Counter);
  }

}

TEST_F (MidiClockInternalFixture, MidiClockCallbackTestImmediate) {
  clock.useImmediateClock = true;

  for (uint8_t i = 0; i < 126; i++) {
    clock.handleClock();
    CHECK_EQUAL(clock.div16th_counter, tester.last16Counter);
    CHECK_EQUAL((i + 1) / 6, tester.last16Counter);
    CHECK_EQUAL(clock.div32th_counter, tester.last32Counter);
    CHECK_EQUAL((i + 1) / 3, tester.last32Counter);
  }
}

TEST_F (MidiClockInternalFixture, MidiClockOverflowTest) {
  clock.useImmediateClock = true;
  clock.outdiv96th_counter = clock.div96th_counter = clock.indiv16th_counter = 4294967295;
  clock.div16th_counter = uint64_t(clock.div96th_counter) / (uint64_t)6;
  clock.div32th_counter = (clock.div96th_counter) / 3;
  clock.mod6_counter = clock.outdiv96th_counter % 6;

  // make sure that we reach the next 16th and 32th
  clock.handleClock();
  clock.handleClock();
  clock.handleClock();
  clock.handleClock();

  CHECK_EQUAL(3, clock.div96th_counter);
  CHECK_EQUAL(((4294967296) / 6) + 1, clock.div16th_counter);
  CHECK_EQUAL(((4294967296) / 3) + 1, clock.div32th_counter);
  CHECK_EQUAL(((4294967296) / 6) + 1, tester.last16Counter);
  CHECK_EQUAL(((4294967296) / 3) + 1, tester.last32Counter);
}
