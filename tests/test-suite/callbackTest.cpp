#include <CppUnitLite2.h>
#include <TestResultStdErr.h>

#include <Callback.hh>

class Foobar {
public:
  bool callbackNoneCalled;
  bool callbackNone2Called;

  bool callbackIntCalled;
  int callbackIntValue;

  bool callbackInt2Called;
  int callbackInt2Value1;
  int callbackInt2Value2;

  Foobar() {
    reset();
  }
  
  void reset() {
    callbackNoneCalled = false;
    callbackNone2Called = false;
    callbackIntCalled = false;
    callbackInt2Called = false;

    callbackIntValue = 0;
    callbackInt2Value1 = 0;
    callbackInt2Value2 = 0;
  }
  
  void callbackNone() {
		//    printf("call none\n");
    callbackNoneCalled = true;
  }

  void callbackNone2() {
		//    printf("call none2 \n");
    callbackNone2Called = true;
  }
  
  void callbackInt(int i) {
    callbackIntCalled = true;
    callbackIntValue = i;
  }

  void callbackInt2(int i1, int i2) {
    callbackInt2Called = true;
    callbackInt2Value1 = i1;
    callbackInt2Value2 = i2;
  }
};

struct CallbackFixture {
  CallbackVector<Foobar, 4>                   callbackNoneList;
  CallbackVector1<Foobar, 4, int>           callbackIntList;
  CallbackVector2<Foobar, 4, int, int> callbackInt2List;
  Foobar foobar;
};

TEST_F (CallbackFixture, CallbackTestAdd) {
  CHECK_EQUAL(0, (int)callbackNoneList.size);
  callbackNoneList.add(&foobar, &Foobar::callbackNone);
  CHECK_EQUAL(1, (int)callbackNoneList.size);
  callbackNoneList.add(&foobar, &Foobar::callbackNone);
  CHECK_EQUAL(1, (int)callbackNoneList.size);
  callbackNoneList.add(&foobar, &Foobar::callbackNone2);
  CHECK_EQUAL(2, (int)callbackNoneList.size);
  callbackNoneList.remove(&foobar, &Foobar::callbackNone);
  CHECK_EQUAL(1, (int)callbackNoneList.size);
  callbackNoneList.add(&foobar, &Foobar::callbackNone);
  CHECK_EQUAL(2, (int)callbackNoneList.size);
  callbackNoneList.remove(&foobar, &Foobar::callbackNone2);
  CHECK_EQUAL(1, (int)callbackNoneList.size);
  callbackNoneList.add(&foobar, &Foobar::callbackNone2);
  CHECK_EQUAL(2, (int)callbackNoneList.size);
  callbackNoneList.remove(&foobar);
  CHECK_EQUAL(0, (int)callbackNoneList.size);
  
  CHECK_EQUAL(0, (int)callbackIntList.size);
  callbackIntList.add(&foobar, &Foobar::callbackInt);
  CHECK_EQUAL(1, (int)callbackIntList.size);
  callbackIntList.add(&foobar, &Foobar::callbackInt);
  CHECK_EQUAL(1, (int)callbackIntList.size);


  CHECK_EQUAL(0, (int)callbackInt2List.size);
  callbackInt2List.add(&foobar, &Foobar::callbackInt2);
  CHECK_EQUAL(1, (int)callbackInt2List.size);
  callbackInt2List.add(&foobar, &Foobar::callbackInt2);
  CHECK_EQUAL(1, (int)callbackInt2List.size);
}

TEST_F (CallbackFixture, CallbackTestCall) {
  foobar.reset();
  
  callbackNoneList.add(&foobar, &Foobar::callbackNone);
  CHECK(!foobar.callbackNoneCalled);
  CHECK(!foobar.callbackNone2Called);
  callbackNoneList.call();
  CHECK(foobar.callbackNoneCalled);
  CHECK(!foobar.callbackNone2Called);

  foobar.reset();
  callbackNoneList.add(&foobar, &Foobar::callbackNone2);
  CHECK(!foobar.callbackNoneCalled);
  CHECK(!foobar.callbackNone2Called);
  callbackNoneList.call();
  CHECK(foobar.callbackNoneCalled);
  CHECK(foobar.callbackNone2Called);

  foobar.reset();
  callbackNoneList.remove(&foobar, &Foobar::callbackNone);
  CHECK(!foobar.callbackNoneCalled);
  CHECK(!foobar.callbackNone2Called);
  callbackNoneList.call();
  CHECK(!foobar.callbackNoneCalled);
  CHECK(foobar.callbackNone2Called);

  foobar.reset();
  callbackIntList.add(&foobar, &Foobar::callbackInt);
  CHECK(!foobar.callbackIntCalled);
  callbackIntList.call(1);
  CHECK(foobar.callbackIntCalled);
  CHECK_EQUAL(1, foobar.callbackIntValue);

  foobar.reset();
  CHECK(!foobar.callbackIntCalled);
  callbackIntList.call(2);
  CHECK(foobar.callbackIntCalled);
  CHECK_EQUAL(2, foobar.callbackIntValue);
}
