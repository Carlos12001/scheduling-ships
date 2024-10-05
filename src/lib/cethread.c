#include "cethread.h"

#include <errno.h>
#include <sched.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>  // For waitpid
#include <unistd.h>

// Thread-local variable to store the current thread's argument
static __thread thread_arg_t *current_thread_arg = NULL;

// Wrapper function that runs the thread routine
int thread_start(void *arg) {
  thread_arg_t *thread_arg = (thread_arg_t *)arg;
  current_thread_arg = thread_arg;

  // Execute the thread function
  void *retval = thread_arg->start_routine(thread_arg->arg);
  thread_arg->retval = retval;

  // Properly terminate the thread
  _exit(0);
}

int cethread_create(cethread_t *thread, void *(*start_routine)(void *),
                    void *arg) {
  // Allocate memory for the stack
  thread->stack = malloc(STACK_SIZE);
  if (thread->stack == NULL) {
    return -1;  // Error allocating the stack
  }

  // Allocate memory for the thread arguments
  thread->thread_arg = malloc(sizeof(thread_arg_t));
  if (thread->thread_arg == NULL) {
    free(thread->stack);
    return -1;  // Error allocating thread_arg
  }
  thread->thread_arg->start_routine = start_routine;
  thread->thread_arg->arg = arg;
  thread->thread_arg->retval = NULL;

  // Calculate the top of the stack (it grows downward)
  void *stack_top = (char *)thread->stack + STACK_SIZE;

  // Create the thread using clone
  thread->tid =
      clone(thread_start, stack_top,
            SIGCHLD | CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND,
            thread->thread_arg);
  if (thread->tid == -1) {
    free(thread->stack);
    free(thread->thread_arg);
    return -1;  // Error creating the thread
  }

  return 0;  // Success
}

int cethread_join(cethread_t thread, void **retval) {
  // Wait for the thread to finish
  pid_t pid = waitpid(thread.tid, NULL, __WALL);
  if (pid == -1) return -1;  // Error waiting for the thread

  // Retrieve the return value
  if (retval != NULL) *retval = thread.thread_arg->retval;

  // Release the allocated resources
  free(thread.stack);
  free(thread.thread_arg);

  return 0;  // Success
}

void cethread_end(void *retval) {
  if (current_thread_arg != NULL) current_thread_arg->retval = retval;

  _exit(0);  // Terminate the current thread
}
