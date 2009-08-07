#include <CppUnitLite2.h>
#include <TestResultStdErr.h>

#include <ListPool.hh>

TEST (MyFirstTest) {
  int a = 102;
  CHECK_EQUAL(102, a);
}

struct ListPoolFixture {
  ListPoolFixture() {
  }

  ListPool<int, 8> pool;
};

TEST_F (ListPoolFixture, ListPoolTestAlloc) {
  ListElt<int> *elt = pool.alloc();
  CHECK(elt != NULL);
}

TEST_F (ListPoolFixture, ListPoolTestAllocValue) {
  ListElt<int> *elt = pool.alloc(0);
  CHECK(elt != NULL);
  CHECK_EQUAL(0, elt->obj);

  elt = pool.alloc(1);
  CHECK(elt != NULL);
  CHECK_EQUAL(1, elt->obj);
}

TEST_F (ListPoolFixture, ListPoolTestAllocFail) {
  for (int i = 0; i < 8; i++) {
    ListElt<int> *elt = pool.alloc();
    CHECK(elt != NULL);
  }
  ListElt<int> *elt = pool.alloc();
  CHECK(elt == NULL);
}

TEST_F (ListPoolFixture, ListPoolTestFree) {
  ListElt<int> *elts[8];
  for (int i = 0; i < 8; i++) {
    elts[i] = pool.alloc();
    CHECK(elts[i] != NULL);
  }
  ListElt<int> *elt = pool.alloc();
  CHECK(elt == NULL);
  pool.free(elts[0]);
  elt = pool.alloc();
  CHECK(elt != NULL);
  CHECK(elt == elts[0]);

  elt = pool.alloc();
  CHECK(elt == NULL);
}

struct ListFixture {
  ListFixture() {
  }

  List<int> list;
  ListPool<int, 8> pool;
};

TEST_F (ListFixture, ListPush) {
  ListElt<int> *elt = pool.alloc();
  elt->obj = 0;
  list.push(elt);
  ListElt<int> *eltRet = list.head;
  CHECK_EQUAL(eltRet, elt);
  CHECK_EQUAL(0, eltRet->obj);
}

TEST_F (ListFixture, ListPushPop) {
  ListElt<int> *elt = pool.alloc();
  elt->obj = 0;
  list.push(elt);
  ListElt<int> *eltRet = list.pop();
  CHECK_EQUAL(eltRet, elt);
  CHECK_EQUAL(0, eltRet->obj);
}

TEST_F (ListFixture, ListPushPop2) {
  ListElt<int> *elt = pool.alloc();
  elt->obj = 0;
  list.push(elt);
  ListElt<int> *eltRet = list.pop();
  CHECK_EQUAL(eltRet, elt);
  CHECK_EQUAL(0, eltRet->obj);

  eltRet = list.pop();
  CHECK(eltRet == NULL);
}

TEST_F (ListFixture, ListPushPop3) {
  ListElt<int> *elt = pool.alloc(0);
  list.push(elt);
  elt = pool.alloc(1);
  CHECK(elt != NULL);
  list.push(elt);
  
  ListElt<int> *eltRet = list.pop();
  CHECK(eltRet != NULL);
  CHECK_EQUAL(eltRet, elt);
  CHECK_EQUAL(1, eltRet->obj);
  pool.free(eltRet);

  eltRet = list.pop();
  CHECK(eltRet != NULL);
  CHECK_EQUAL(0, eltRet->obj);
  pool.free(eltRet);

  elt = pool.alloc(0);
  CHECK(elt != NULL);
  list.push(elt);

  elt = pool.alloc(1);
  CHECK(elt != NULL);
  list.push(elt);

  elt = pool.alloc(2);
  CHECK(elt != NULL);
  list.push(elt);

  eltRet = list.pop();
  CHECK(eltRet != NULL);
  CHECK_EQUAL(2, eltRet->obj);
  pool.free(eltRet);

  elt = pool.alloc(3);
  CHECK(elt != NULL);
  list.push(elt);

  eltRet = list.pop();
  CHECK(eltRet != NULL);
  CHECK_EQUAL(3, eltRet->obj);
  pool.free(eltRet);

  eltRet = list.pop();
  CHECK(eltRet != NULL);
  CHECK_EQUAL(1, eltRet->obj);
  pool.free(eltRet);
}

/* XXX test remove */

struct ListDataFixture {
  List<int> list;
  ListElt<int> elts[8];
  ListDataFixture() {
    for (int i = 0; i < 8; i++) {
      elts[i].obj = i;
    }
    for (int i = 0; i < 8; i++) {
      list.push(&elts[i]);
    }
  }
};

TEST_F (ListDataFixture, ListDataPush) {
  for (int i = 0; i < 8; i++) {
    ListElt<int> *elt = list.pop();
    CHECK_EQUAL(&elts[7 - i], elt);
    CHECK_EQUAL(7 - i, elt->obj);
  }
}

TEST_F (ListDataFixture, ListDataFind) {
  for (int i = 0; i < 8; i++) {
    ListElt<int> *elt = list.findFirst(i);
    CHECK_EQUAL(&elts[i], elt);
  }
  ListElt<int> *elt = list.findFirst(10);
  CHECK(elt == NULL);
}

TEST_F (ListDataFixture, ListDataFindAfter) {
  ListElt<int> *elt = NULL;

  elt = list.findFirstAfter(0, &elts[7]);
  CHECK_EQUAL(&elts[0], elt);
  elt = list.findFirstAfter(1, &elts[3]);
  CHECK_EQUAL(&elts[1], elt);
  elt = list.findFirstAfter(3, &elts[3]);
  CHECK_EQUAL(&elts[3], elt);
  elt = list.findFirstAfter(3, &elts[2]);
  CHECK(elt == NULL);
}

TEST_F (ListDataFixture, ListDataNext) {
  ListElt<int> *elt = list.head;
  CHECK_EQUAL(elt, &elts[7]);
  CHECK(elt->prev == NULL);
  CHECK_EQUAL(&elts[6], elt->next);
}

TEST_F (ListDataFixture, ListDataPrev) {
  ListElt<int> *elt = list.findFirst(5);
  CHECK_EQUAL(&elts[5], elt);
  CHECK_EQUAL(&elts[6], elt->prev);
  CHECK_EQUAL(&elts[4], elt->next);
}

TEST_F (ListDataFixture, ListDataFindBefore) {
  ListElt<int> *elt = NULL;
  ListElt<int> *elt2 = NULL;

  elt = list.getLast();
  CHECK_EQUAL(&elts[0], elt);
  elt2 = list.findFirstBefore(3, elt);
  CHECK_EQUAL(&elts[3], elt2);
  elt = list.findFirstBefore(0, elt2);
  CHECK(elt == NULL);
}

TEST_F (ListDataFixture, ListDataSize) {
  CHECK_EQUAL(8, list.size());

  list.pop();
  CHECK_EQUAL(7, list.size());
  list.pop();
  CHECK_EQUAL(6, list.size());
  list.pop();
  CHECK_EQUAL(5, list.size());
  list.pop();
  CHECK_EQUAL(4, list.size());
  ListElt<int> elt;
  elt.obj = 9;
  list.push(&elt);
  CHECK_EQUAL(5, list.size());
  for (int i = 0; i < 5; i++) {
    list.pop();
    CHECK_EQUAL(4 - i, list.size());
  }
  list.pop();
  CHECK_EQUAL(0, list.size());
  list.pop();
  CHECK_EQUAL(0, list.size());
  list.pop();
  CHECK_EQUAL(0, list.size());
  list.push(&elts[0]);
  CHECK_EQUAL(1, list.size());
}

TEST_F (ListDataFixture, ListDataRemove) {
  ListElt<int> *elt = NULL;

  for (int i = 0; i < 8; i++) {
    elt = list.getAt(i);
    CHECK_EQUAL(&elts[7-i], elt);
  }
  elt = list.pop();
  CHECK_EQUAL(&elts[7], elt);
}

TEST_F (ListDataFixture, ListReverse) {
  list.reverse();
  for (int i = 0; i < 8; i++) {
    ListElt<int> *elt = list.getAt(i);
    CHECK_EQUAL(&elts[i], elt);
  }
  CHECK_EQUAL(&elts[0], list.head);
  CHECK_EQUAL(&elts[7], list.getLast());
}

/* insertAt */
/* removeAt */
/* remove */
/* swap */

