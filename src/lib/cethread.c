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


/**********************
******** Mutex ********
***********************/
int cemutex_init(my_mutex_t *mutex) {
  atomic_flag_clear(&mutex->flag); // De stdatomic.h
}

int cemutex_destroy(cemutex *mutex) {
  return pthread_mutex_destroy(&mutex->mutex);
}

// Implementar lock

int cemutex_unlock(my_mutex_t *mutex) {
  //return pthread_mutex_unlock(&mutex->mutex);
  atomic_flag_clear(&mutex->flag);
}