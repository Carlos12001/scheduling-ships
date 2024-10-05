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
int cemutex_init(cemutex *cm) {
  atomic_flag_clear(&cm->flag); // De stdatomic.h
  return 0;
}

int cemutex_destroy(cemutex *cm) {
  if (atomic_flag_test_and_set(&cm->flag)) {
    printf("Error: intento de destruir un mutex aún bloqueado.\n");
    return 1;
  }
  return 0;
}

int cemutex_lock(cemutex *cm) {
    while (atomic_flag_test_and_set(&cm->flag)) {
        // Spinlock - Espera activa
    }
    return 0;
}

int cemutex_unlock(cemutex *cm) {
  atomic_flag_clear(&cm->flag);
  return 0;
}