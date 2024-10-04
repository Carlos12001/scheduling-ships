#include "cethread.h"

struct cethread_t {
  pthread_t thread;
};

struct cemutex {
  pthread_mutex_t mutex;
};

int cethread_create(cethread_t *thread, void *(*start_routine)(void *),
                    void *arg) {
  return pthread_create(&thread->thread, NULL, start_routine, arg);
}

int cethread_join(cethread_t thread, void **retval) {
  return pthread_join(thread.thread, retval);
}

int cemutex_init(cemutex *mutex) {
  return pthread_mutex_init(&mutex->mutex, NULL);
}

int cemutex_destroy(cemutex *mutex) {
  return pthread_mutex_destroy(&mutex->mutex);
}

int cemutex_unlock(cemutex *mutex) {
  return pthread_mutex_unlock(&mutex->mutex);
}