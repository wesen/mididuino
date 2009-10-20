#include <CppUnitLite2.h>
#include <TestResultStdErr.h>

#include <Arpeggiator.hh>

struct ArpeggiatorFixture {
  ArpeggiatorFixture() {
  }

  ArpeggiatorClass arp;
};

TEST_F (ArpeggiatorFixture, ArpAddSingleNote) {
  arp.addNote(0, 1);
  CHECK_EQUAL(arp.orderedNotes[0], 0);
  CHECK_EQUAL(arp.orderedVelocities[0], 1);
  CHECK_EQUAL(arp.numNotes, 1);
}

TEST_F (ArpeggiatorFixture, ArpAddTwoNotes) {
  arp.addNote(0, 1);
  arp.addNote(2, 3);
  CHECK_EQUAL(arp.orderedNotes[0], 0);
  CHECK_EQUAL(arp.orderedVelocities[0], 1);
  CHECK_EQUAL(arp.orderedNotes[1], 2);
  CHECK_EQUAL(arp.orderedVelocities[1], 3);
  CHECK_EQUAL(arp.numNotes, 2);
}

TEST_F (ArpeggiatorFixture, ArpAddRemoveSingleNote) {
  arp.addNote(0, 1);
  arp.removeNote(0);
  CHECK_EQUAL(arp.numNotes, 0);
}

TEST_F (ArpeggiatorFixture, ArpUpdateVelocity) {
  arp.addNote(0, 1);
  arp.addNote(0, 2);
  CHECK_EQUAL(arp.numNotes, 1);
  CHECK_EQUAL(arp.orderedNotes[0], 0);
  CHECK_EQUAL(arp.orderedVelocities[0], 2);
}

TEST_F (ArpeggiatorFixture, ArpUpdateTwoVelocities) {
  arp.addNote(0, 1);
  arp.addNote(1, 2);
  arp.addNote(0, 2);
  arp.addNote(1, 3);
  CHECK_EQUAL(arp.numNotes, 2);
  CHECK_EQUAL(arp.orderedNotes[0], 0);
  CHECK_EQUAL(arp.orderedNotes[1], 1);
  CHECK_EQUAL(arp.orderedVelocities[0], 2);
  CHECK_EQUAL(arp.orderedVelocities[1], 3);
}

TEST_F (ArpeggiatorFixture, ArpAddRemoveMultipleNotes) {
  arp.addNote(0, 1);
  arp.addNote(1, 2);
  arp.addNote(2, 3);
  CHECK_EQUAL(arp.numNotes, 3);
  arp.removeNote(1);
  CHECK_EQUAL(arp.numNotes, 2);
  CHECK_EQUAL(arp.orderedNotes[0], 0);
  CHECK_EQUAL(arp.orderedNotes[1], 2);

  arp.removeNote(0);
  CHECK_EQUAL(arp.numNotes, 1);
  CHECK_EQUAL(arp.orderedNotes[0], 2);
}

