#ifndef CETHREAD_H
#define CETHREAD_H

#include <sys/types.h>

#ifndef STACK_SIZE
#define STACK_SIZE (1024 * 1024)  // 1MiB
#endif

typedef struct {
  pid_t tid;    // id thread
  void *stack;  // pointer to stack
} cethread_t;

typedef struct {
  volatile int lock;  // 0 = unlocked, 1 = locked
} cemutex;

// Context from Function src/lib/cethread.c
int cethread_create(cethread_t *thread, void *(*start_routine)(void *),
                    void *arg);
int cethread_join(cethread_t thread, void **retval);
int cemutex_init(cemutex *mutex);
int cemutex_destroy(cemutex *mutex);
int cemutex_lock(cemutex *mutex);
int cemutex_unlock(cemutex *mutex);

#endif  // CETHREAD_H
