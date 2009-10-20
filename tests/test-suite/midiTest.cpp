#include <CppUnitLite2.h>
#include <TestResultStdErr.h>

#include <ListPool.hh>
#include <Midi.h>

class Foobar : public MidiCallback {
public:
  void midiCallback(uint8_t *msg) {
    printf("callbac\n");
  }
};

struct MidiCallbackFixture {
  MidiCallbackFixture() {
  }
  
  CallbackList<MidiCallback, 4> callbackList;
  Foobar foobar;
};

TEST_F (MidiCallbackFixture, TestAdd) {
  CHECK(callbackList.add(&foobar));
  CHECK_EQUAL(1, callbackList.size());
}

TEST_F (MidiCallbackFixture, TestAddRemove) {
  CHECK(callbackList.add(&foobar));
  CHECK_EQUAL(1, callbackList.size());
  CHECK(callbackList.remove(&foobar));
  CHECK_EQUAL(0, callbackList.size());
}

TEST_F (MidiCallbackFixture, TestCall) {
  CHECK(callbackList.add(&foobar));
  CHECK_EQUAL(1, callbackList.size());
  //  callbackMidi(callbackList, NULL);
}
