#ifndef CETHREAD_H
#define CETHREAD_H

#include <pthread.h>

#include <stdatomic.h>
#include <unistd.h>

typedef struct {
  pthread_t thread;
} cethread_t;

typedef struct {
  pthread_mutex_t mutex;
} cemutex;

typedef struct {
  atomic_flag flag;
} my_mutex_t;

// Funciones CEThreads que envuelven las de pthread
int cethread_create(cethread_t *thread, void *(*start_routine)(void *),
                    void *arg);
int cethread_join(cethread_t thread, void **retval);
int cemutex_init(my_mutex_t *mutex);
int cemutex_destroy(cemutex *mutex);
int cemutex_unlock(my_mutex_t *mutex);

#endif  // CETHREAD_H
