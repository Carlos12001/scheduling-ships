#ifndef CETHREAD_H
#define CETHREAD_H

#include <pthread.h>

#include <stdatomic.h>
#include <unistd.h>
#include <stdio.h>

typedef struct {
  pthread_t thread;
} cethread_t;

typedef struct {
  atomic_flag flag;
} cemutex;

// Funciones CEThreads que envuelven las de pthread
int cethread_create(cethread_t *thread, void *(*start_routine)(void *),
                    void *arg);
int cethread_join(cethread_t thread, void **retval);

// Funciones CEMutex
int cemutex_init(cemutex *cm);
int cemutex_destroy(cemutex *cm);
int cemutex_lock(cemutex *cm);
int cemutex_unlock(cemutex *cm);

#endif  // CETHREAD_H
