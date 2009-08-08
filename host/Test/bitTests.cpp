#include <CppUnitLite2.h>
#include <TestResultStdErr.h>

#include <BitArray.hh>

TEST (TestField1) {
  BitField<1> b;
  b.setBit(0);
  CHECK(b.isBitSet(0));
  b.clearBit(0);
  CHECK(!b.isBitSet(0));
  b.toggleBit(0);
  CHECK(b.isBitSet(0));
  b.toggleBit(0);
  CHECK(!b.isBitSet(0));
}
