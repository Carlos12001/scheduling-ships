#ifndef CETHREAD_H
#define CETHREAD_H

#include <sys/types.h>

#ifndef STACK_SIZE
#define STACK_SIZE (1024 * 1024)  // 1MiB
#endif

typedef struct {
  pid_t tid;    // ID del hilo
  void *stack;  // Puntero a la pila del hilo
} cethread_t;

typedef struct {
  volatile int lock;  // Variable de bloqueo simple
} cemutex;

// Funciones CEThreads que envuelven las de pthread
int cethread_create(cethread_t *thread, void *(*start_routine)(void *),
                    void *arg);
int cethread_join(cethread_t thread, void **retval);
int cemutex_init(cemutex *mutex);
int cemutex_destroy(cemutex *mutex);
int cemutex_unlock(cemutex *mutex);

#endif  // CETHREAD_H
