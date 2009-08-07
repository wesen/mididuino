#ifndef LISTPOOL_H__
#define LISTPOOL_H__

#include <inttypes.h>
#ifndef NULL
#define NULL 0
#endif

template <class C> class ListElt {
public:
  C obj;
  ListElt *next;
  ListElt *prev;
};


template <class C, int N>
class ListPool {
public:

  ListElt<C> heap[N];
  ListElt<C> *freeList;

  ListPool() {
    ListElt<C> *prev = NULL;
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
  }

  ListElt<C> *alloc() {
    if (freeList == NULL)
      return NULL;
    ListElt<C> *ret = freeList;
    ListElt<C> *next = ret->next;
    if (next != NULL) {
      next->prev = NULL;
    }
    ret->next = NULL;
    freeList = next;
    return ret;
  }


  ListElt<C> *alloc(C c) {
    ListElt<C> *ret = alloc();
    if (ret != NULL) {
      ret->obj = c;
    }
    return ret;
  }
  
  ListElt<C> *alloc(C &c) {
    ListElt<C> *ret = alloc();
    if (ret != NULL) {
      ret->obj = c;
    }
    return ret;
  }

  void free(ListElt<C> *elt) {
    if (freeList == NULL) {
      freeList = elt;
      elt->prev = elt->next = NULL;
    } else {
      elt->prev = NULL;
      elt->next = freeList;
      freeList = elt;
    }
  }
};

template <class C> class List {
public:
  ListElt<C> *head;

  List() {
    head = NULL;
  }

  ListElt<C> *getAt(uint8_t idx) {
    ListElt<C> *ptr;
    uint8_t i;
    
    for (i = 0, ptr = head; ptr != NULL && i < idx; i++, ptr = ptr->next) {
      ;
    }
    return ptr;
  }

  uint8_t size() {
    uint8_t ret = 0;
    ListElt<C> *ptr;
    for (ptr = head; ptr != NULL; ptr = ptr->next) {
      ret++;
    }
    return ret;
  }

  ListElt<C> *insertAt(ListElt<C> *elt, uint8_t idx) {
    ListElt<C> *ptr = head;
    ListElt<C> *prev = NULL;
    for (uint8_t i = 0;
	 ptr != NULL != NULL && i < idx;
	 i++, prev = ptr, ptr = ptr->next) {
      ;
    }
    elt->prev = prev;
    elt->next = ptr;
    if (ptr != NULL) {
      ptr->prev = elt;
    }
    return elt;
  }

  void removeAt(uint8_t idx) {
    ListElt<C> *ptr = getAt(idx);
    if (ptr != NULL) {
      remove(ptr);
    }
  }

  ListElt<C> *getLast() {
    if (head == NULL)
      return NULL;
    ListElt<C> *ptr = head;
    while (ptr->next != NULL) {
      ptr = ptr->next;
    }
    return ptr;
  }

  ListElt<C> *popLast() {
    ListElt<C> *ret = getLast();
    if (ret != NULL) {
      ret->prev->next = NULL;
      ret->prev = NULL;
    }
    return ret;
  }

  void pushLast(ListElt<C> *elt) {
    ListElt<C> *ptr = getLast();
    if (ptr != NULL) {
      ptr->next = elt;
      elt->prev = ptr;
      elt->next = NULL;
    } else {
      addFront(elt);
    }
  }
  
  void remove(ListElt<C> *elt) {
    ListElt<C> *prev = elt->prev;
    ListElt<C> *next = elt->next;
    if (prev != NULL) {
      prev->next = next;
    } else {
      // prev == NULL, first in List
      head = next;
    }
    if (next != NULL) {
      next->prev = prev;
    }
  }

  ListElt<C> *pop() {
    ListElt<C> *ret = NULL;
    if (head != NULL) {
      ret = head;
      head = head->next;
    }

    return ret;
  }

  void push(ListElt<C> *elt) {
    elt->next = head;
    elt->prev = NULL;
    if (head != NULL) {
      head->prev = elt;
    }
    head = elt;
  }

  ListElt<C> *findFirstAfter(C c, ListElt<C> *ptr) {
    for (; ptr != NULL; ptr = ptr->next) {
      if (ptr->obj == c) {
	return ptr;
      }
    }
    return NULL;
  }

  ListElt<C> *findFirst(C c) {
    return findFirstAfter(c, head);
  }

  ListElt<C> *findFirstBefore(C c, ListElt<C> *ptr) {
    for (; ptr != NULL; ptr = ptr->prev) {
      if (ptr->obj == c) {
	return ptr;
      }
    }
    return NULL;
  }
  
  void swap(ListElt<C> *e1, ListElt<C> *e2) {
    ListElt<C> *tmp;
    tmp = e1->prev;
    e1->prev = e2->prev;
    e2->prev = tmp;
    tmp = e1->next;
    e1->next = e2->next;
    e2->next = tmp;
  }

  void reverse() {
    ListElt<C> *ptr = head;
    ListElt<C> *next = NULL;
    ListElt<C> *prev = NULL;

    for (; ptr != NULL;) {
      next = ptr->next;
      ptr->prev = next;
      ptr->next = prev;
      prev = ptr;
      ptr = next;
    }

    head = prev;
  }
};

template <class C, int N> class ListWithPool : public List<C> {
public:
  ListPool<C, N> pool;

  void freeAll() {
    for (ListElt<C> *ptr = List<C>::head; ptr != NULL; ptr = ptr->next) {
      pool.free(ptr);
    }
    List<C>::head = NULL;
  }

  bool getLastValue(C &c) {
    ListElt<C> *elt = List<C>::getLast();
    if (elt != NULL) {
      c = elt->obj;
      return true;
    } else {
      return false;
    }
  }

  bool popLastValue(C &c) {
    ListElt<C> *elt = List<C>::popLast();
    if (elt != NULL) {
      c = elt->obj;
      pool.free(elt);
      return true;
    } else {
      return false;
    }
  }

  bool popValue(C &c) {
    ListElt<C> *elt = List<C>::pop();
    if (elt != NULL) {
      c = elt->obj;
      pool.free(elt);
      return true;
    } else {
      return false;
    }
  }

  bool pushValue(C &c) {
    ListElt<C> *elt = pool.alloc(c);
    if (elt == NULL) {
      return false;
    } else {
      List<C>::push(elt);
      return true;
    }
  }

  void pushValue(C c) {
    ListElt<C> *elt = pool.alloc(c);
    if (elt == NULL) {
      return false;
    } else {
      List<C>::push(elt);
      return true;
    }
  }

  void pushLastValue(C &c) {
    ListElt<C> *elt = pool.alloc(c);
    if (elt == NULL) {
      return false;
    } else {
      List<C>::pushLast(elt);
      return true;
    }
  }

  void pushLastValue(C c) {
    ListElt<C> *elt = pool.alloc(c);
    if (elt == NULL) {
      return false;
    } else {
      List<C>::pushLast(elt);
      return true;
    }
  }
};
  


#endif /* LISTPOOL_H__ */
