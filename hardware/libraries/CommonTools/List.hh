#ifndef LIST_H__
#define LIST_H__

#include <inttypes.h>
#ifndef NULL
#define NULL 0
#endif

template <class C, int N>
class ListPool {
public:

  class ListElt {
  public:
    C obj;
    ListElt *next;
    ListElt *prev;
  };

  ListElt heap[N];
  ListElt *freeList;

  ListPool() {
    ListElt *prev = NULL;
    for (int i = 0; i < N; i++) {
      heap[i].prev = NULL;
      heap[i].next = NULL;

      if (prev != NULL) {
	prev->next = &heap[i];
	heap[i].prev = prev;
      }

      prev = &heap[i];
    }
    freeList = &heap[0];
    usedList = NULL;
  }

  ListElt *new() {
    if (usedList == NULL)
      return NULL;
    ListElt *ret = usedList;
    ListElt *next = ret->next;
    if (next != NULL) {
      next->prev = NULL;
    }
    ret->next = NULL;
    useDList = next;
    return ret;
  }

  void free(ListElt *elt) {
    if (usedList == NULL) {
      usedList = elt;
      elt->prev = elt->next = NULL;
    } else {
      elt->prev = NULL;
      elt->next = usedList;
      usedList = elt;
    }
  }
};

#endif /* LIST_H__ */
