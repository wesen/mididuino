/*
 * MidiCtrl - Host-side thread implementation
 *
 * (c) July 2011 - Manuel Odendahl - wesen@ruinwesen.com
 */

#include "thread.hh"

bool ThreadSynchronization::debug = true;

/***************************************************************************
 *
 * Thread synchronization
 *
 ***************************************************************************/

ThreadSynchronization::ThreadSynchronization() {
  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&cond, NULL);
  owner = (pthread_t)-1;
  level = 0;
}

ThreadSynchronization::~ThreadSynchronization() {
  pthread_cond_destroy(&cond);
  pthread_mutex_destroy(&mutex);
}

void ThreadSynchronization::lock() {
  DEBUG_PRINTF("lock %p from thread %p (owner: %p, level %d)\n", this, pthread_self(), owner, level);

#ifdef __APPLE__
  /* apparently can't relock an already self owned lock */
  if (owner == pthread_self()) {
    level++;
    return;
  }
#endif
		
  pthread_mutex_lock(&mutex);
  DEBUG_PRINTF("acquired lock %p from thread %p\n", this, pthread_self());
  level++;
  if (owner != (pthread_t)-1) {
    assert(owner == pthread_self());
  } else {
    owner = pthread_self();
  }
}

void ThreadSynchronization::unlock() {
  DEBUG_PRINTF("unlock %p from thread %p (owner: %p, level: %d)\n", this, pthread_self(), owner, level);
  assert(owner == pthread_self());
  level--;
  if (level == 0) {
    owner = (pthread_t)-1;
    pthread_mutex_unlock(&mutex);
  }
}

void ThreadSynchronization::wait() {
  DEBUG_PRINTF("wait %p from thread %p (owner: %p)\n", this, pthread_self(), owner);
  assert(owner == pthread_self());
  pthread_cond_wait(&cond, &mutex);
}
  
void ThreadSynchronization::signal() {
  DEBUG_PRINTF("signal %p from thread %p (owner: %p)\n", this, pthread_self(), owner);
  assert(owner == pthread_self());
  pthread_cond_signal(&cond);
}

/***************************************************************************
 *
 * Thread safe class
 *
 ***************************************************************************/

void ThreadedClass::start() {
  assert(running == false);
  running = true;
  pthread_create(&thread, NULL, &ThreadedClass::startThread, this);
}

void ThreadedClass::stop() {
  assert(running == true);
  running = false;
  stopRequested = true;
  //    pthread_join(&thread, 0);
}
