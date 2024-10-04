#ifndef CETHREAD_H
#define CETHREAD_H

#include <pthread.h>

typedef struct {
  pthread_t thread;
} cethread_t;

typedef struct {
  pthread_mutex_t mutex;
} cemutex;

// Funciones CEThreads que envuelven las de pthread
int cethread_create(cethread_t *thread, void *(*start_routine)(void *),
                    void *arg);
int cethread_join(cethread_t thread, void **retval);
int cemutex_init(cemutex *mutex);
int cemutex_destroy(cemutex *mutex);
int cemutex_unlock(cemutex *mutex);

#endif  // CETHREAD_H
