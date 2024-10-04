#include "cethread.h"

#define _GNU_SOURCE
#include <errno.h>
#include <sched.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

int cethread_create(cethread_t *thread, void *(*start_routine)(void *),
                    void *arg) {
  // Create a new STACK_SIZE bytes stack
  thread->stack = malloc(STACK_SIZE);
  if (thread->stack == NULL) {
    return -1;  // Error al asignar la pila
  }

  // Apuntar al final de la pila (las pilas crecen hacia abajo)
  void *stack_top = (char *)thread->stack + STACK_SIZE;

  // Crear el nuevo hilo
  thread->tid =
      clone((int (*)(void *))start_routine, stack_top,
            SIGCHLD | CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND, arg);
  if (thread->tid == -1) {
    free(thread->stack);
    return -1;  // Error al crear el hilo
  }

  return 0;  // Éxito
}

int cethread_join(cethread_t thread, void **retval) {
  pid_t pid = waitpid(thread.tid, NULL, __WALL);
  if (pid == -1) return -1;  // Error to create the thread

  // Liberar la pila del hilo
  free(thread.stack);

  if (retval != NULL) *retval = NULL;  //  We don't have a return value

  return 0;  // Success
}
