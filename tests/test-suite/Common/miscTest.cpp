#include <CppUnitLite2.h>
#include <TestResultStdErr.h>

#include "WProgram.h"
#include <helpers.h>

TEST (bitTest32) {
  CHECK(IS_BIT_SET32(1, 0));
  CHECK(IS_BIT_SET32(_BV(24), 24));
  CHECK(IS_BIT_SET32(_BV(24) | _BV(19), 24));
  CHECK(IS_BIT_SET32(_BV(24) | _BV(19), 19));

  uint32_t foo = 0xFFFFFFFF;

  for (uint8_t i = 0; i < 32; i++) {
    CHECK(IS_BIT_SET32(foo, i));
  }
}

TEST (interpolate) {
  for (uint8_t i = 0; i <= 128; i++) {
    //    printf("should: %d, is: %d\n", i, interpolate_8(0, 128, i));
    CHECK_EQUAL(i, interpolate_8(0, 128, i));
  }

  for (uint8_t i = 0; i <= 128; i++) {
    //    printf("should: %d, is: %d\n", 128 - i, interpolate_8(128, 0, i));
    CHECK_EQUAL(128 - i, interpolate_8(128, 0, i));
  }
}

struct PrintfFixture {
	PrintfFixture() {
		for (uint8_t i = 0; i < sizeof(buf); i++) {
			buf[i] = 0;
		}
	}
	char buf[128];
};

TEST_F (PrintfFixture, snprintf) {
	m_snprintf(buf, sizeof(buf), "foo");
	CHECK(!strcmp("foo", buf));

	m_snprintf(buf, 2, "foo");
	CHECK(!strcmp("f", buf));

	m_snprintf(buf, sizeof(buf), "foo %b", 5);
	CHECK(!strcmp("foo 005", buf));

	m_snprintf(buf, sizeof(buf), "foo %B", 10000);
	CHECK(!strcmp("foo 10000", buf));

	m_snprintf(buf, sizeof(buf), "foo %x", 0xFF);
	CHECK(!strcmp("foo ff", buf));

	m_snprintf(buf, sizeof(buf), "foo %X", 0xFF);
	CHECK(!strcmp("foo 00ff", buf));

	m_snprintf(buf, sizeof(buf), "foo %s", "foo");
	CHECK(!strcmp("foo foo", buf));
}
