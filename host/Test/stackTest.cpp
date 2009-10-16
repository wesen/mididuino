#include <CppUnitLite2.h>
#include <TestResultStdErr.h>

#include <Stack.h>


struct SimpleStackFixture {
  SimpleStackFixture() {
  }

  Stack<uint8_t, 8> stack;
};

TEST_F (SimpleStackFixture, SimpleStackInit) {
  CHECK_EQUAL(0, stack.size());
  CHECK(stack.isEmpty());
  CHECK(!stack.isFull());
}

TEST_F (SimpleStackFixture, SimpleStacKPush) {
  bool ret = stack.push((uint8_t)0);
  CHECK(ret);
  CHECK(!stack.isFull());
  CHECK(!stack.isEmpty());
  CHECK_EQUAL(1, stack.size());
  
  uint8_t num;
  ret = stack.peek(&num);
  CHECK(ret);
  CHECK_EQUAL(0, num);
  CHECK(!stack.isEmpty());
  CHECK(!stack.isFull());
  CHECK_EQUAL(1, stack.size());
  
  ret = stack.pop(&num);
  CHECK(ret);
  CHECK_EQUAL(0, num);
  CHECK(stack.isEmpty());
  CHECK(!stack.isFull());
  CHECK_EQUAL(0, stack.size());
}

TEST_F (SimpleStackFixture, SimpleStackMultiPush) {
  bool ret;
  uint8_t num;

  for (uint8_t i = 0; i < 7; i++) {
    ret = stack.push(i);
    CHECK(ret);
    CHECK_EQUAL(i + 1, stack.size());

    ret = stack.peek(&num);
    CHECK_EQUAL(i, num);
  }

  CHECK(stack.isFull());
  
  CHECK_EQUAL(7, stack.size());

  for (int i = 6; i >= 0; i--) {
    CHECK_EQUAL(i + 1, stack.size());
    
    ret = stack.pop(&num);
    CHECK(ret);
    CHECK_EQUAL(i, num);
  }

  CHECK(stack.isEmpty());
  CHECK_EQUAL(0, stack.size());

  ret = stack.pop(&num);
  CHECK(!ret);
  CHECK_EQUAL(0, stack.size());
  CHECK(stack.isEmpty());
}

struct DataStackFixture {
  DataStackFixture() {
    for (uint8_t i = 0; i < 7; i++) {
      stack.push(i);
    }
  }

  Stack<uint8_t, 8> stack;
};

TEST_F (DataStackFixture, DataPeekAt) {
  bool ret;
  uint8_t num;
  
  for (uint8_t i = 0; i < 7; i++) {
    ret = stack.peekAt(&num, i);
    CHECK(ret);
    //    printf("i: %d, num: %d\n", i, num);
    CHECK_EQUAL(6-i, num);
  }
  ret = stack.peekAt(&num, 8);
  CHECK(!ret);
}


struct UndoStackFixture {
  UndoStackFixture() {
  }

  Stack<uint8_t [24], 8> stack;
};

TEST_F (UndoStackFixture, UndoStackPush) {
	uint8_t data[24];
	uint8_t data2[24];

	stack.reset();

	for (uint8_t i = 0; i < countof(data); i++) {
		data[i] = 0;
		data2[i] = 1;
	}

	stack.push(&data);
	CHECK(!stack.isFull());
	CHECK_EQUAL(1, stack.size());

	bool ret = stack.pop(&data2);
	CHECK(ret);
	for (uint8_t i = 0; i < countof(data2); i++) {
		CHECK_EQUAL(data[i], data2[i]);
	}
}
