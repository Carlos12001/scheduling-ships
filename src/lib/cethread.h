#ifndef CETHREAD_H
#define CETHREAD_H

#include <stdatomic.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#ifndef STACK_SIZE
#define STACK_SIZE (1024 * 1024)  // 1MiB
#endif // STACK_SIZE

typedef struct {
  void *(*start_routine)(void *);
  void *arg;
  void *retval;
} thread_arg_t;

typedef struct {
  pid_t tid;                 // id thread
  void *stack;               // pointer to stack
  thread_arg_t *thread_arg;  // argument
} cethread_t;

typedef struct {
  atomic_flag flag;
} cemutex;

// Context from Function src/lib/cethread.c
int cethread_create(cethread_t *thread, void *(*start_routine)(void *),
                    void *arg);
int cethread_join(cethread_t thread, void **retval);
void cethread_end(void *retval);

int cemutex_init(cemutex *cm);
int cemutex_destroy(cemutex *cm);
int cemutex_lock(cemutex *cm);
int cemutex_unlock(cemutex *cm);

#endif  // CETHREAD_H
