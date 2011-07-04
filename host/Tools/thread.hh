/*
 * MidiCtrl - Host-side thread safe objects
 *
 * (c) 2009 - 2011 - Manuel Odendahl - wesen@ruinwesen.com
 */

/* XXX not sure if this is working, I don't remember actually writing it. */

#ifndef THREAD_H__
#define THREAD_H__

#include <assert.h>
#include <pthread.h>

#include <string>
#include <queue>
#include <list>

using namespace std;

#define DEBUG

#ifdef DEBUG
#define DEBUG_PRINTF(str, args...)                              \
  if (ThreadSynchronization::debug) printf(str, ## args)
#else
#define DEBUG_PRINTF(stder, args...)
#endif

/***************************************************************************
 *
 * Thread synchronization class
 *
 ***************************************************************************/

class ThreadSynchronization {
protected:
  pthread_mutex_t mutex;
  pthread_cond_t cond;
  pthread_t owner;
  int level;

public:
  static bool debug;

  ThreadSynchronization();
  virtual ~ThreadSynchronization();

  void lock();
  void unlock();
  void wait();
  void signal();
};

/***************************************************************************
 *
 * Safe wrapper around object
 *
 ***************************************************************************/

template<class C> class ThreadSafeObject : public ThreadSynchronization {
protected:
  C *obj;

public:
  ThreadSafeObject(C *_obj) : ThreadSynchronization(), obj(_obj) {
  }

  virtual ~ThreadSafeObject() {
  }

  C *getLockedObject() {
    lock();
    return obj;
  }
};

/***************************************************************************
 *
 * Abstract parent for thread safe classes
 *
 ***************************************************************************/

class ThreadedClass {
public:
  ThreadedClass() : stopRequested(false), running(false) {
  }

  ~ThreadedClass() {
  }

  virtual void start();
  virtual void stop();

  bool isRunning() {
    return running;
  }

protected:
  pthread_t thread;
  
  volatile bool stopRequested;
  volatile bool running;

  static void *startThread(void *obj) {
    reinterpret_cast<ThreadedClass *>(obj)->doWork();
    return NULL;
  }

  virtual void doWork() {
  }
};

/***************************************************************************
 *
 * Thread-safe queue
 *
 ***************************************************************************/

template<class C> class ThreadSafeQueue : public ThreadSafeObject<queue<C> > {
protected:
  queue<C> myQueue;

public:
  ThreadSafeQueue() : ThreadSafeObject<queue<C> >(&myQueue) {
  }

  bool isDataAvailable() {
    queue<C> *q = ThreadSafeObject<queue<C> >::getLockedObject();
    bool result = !q->empty();
    ThreadSafeObject<queue<C> >::unlock();
    return result;
  }

  const C *getData() {
    queue<C> *q = ThreadSafeObject<queue<C> >::getLockedObject();
    C *result = new C(q->front());
    q->pop();
    ThreadSafeObject<queue<C> >::unlock();
    return result;
  }

  void putData(const C &data) {
    queue<C> *q = ThreadSafeObject<queue<C> >::getLockedObject();
    q->push(data);
    ThreadSafeObject<queue<C> >::unlock();
  }
};

/***************************************************************************
 *
 * Thread safe list
 *
 ***************************************************************************/

template<class C> class ThreadSafeList : public ThreadSafeObject<list<C> > {
protected:
  list<C> myList;

public:
  ThreadSafeList() : ThreadSafeObject<list<C> > (&myList) {
  }

  void remove(const C &c) {
    list<C> *l = ThreadSafeObject<list<C> >::getLockedObject();
    l->remove(c);
    ThreadSafeObject<list<C> >::unlock();
  }

  void push_front(const C &c) {
    list<C> *l = ThreadSafeObject<list<C> >::getLockedObject();
    l->push_front(c);
    ThreadSafeObject<list<C> >::unlock();
  }
};

#endif /* THREAD_H__ */
