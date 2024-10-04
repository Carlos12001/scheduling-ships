#include <stdio.h>
#include <unistd.h>

#include "cethread.h"

void* thread_func(void* arg) {
  for (int i = 0; i < 3; i++) {
    sleep(1);
    printf("PThread started: %ld\n", (long)arg);
  }
  return NULL;
}

int main() {
  cethread thread;
  cethread_create(&thread, thread_func, (void*)1);
  cethread_join(thread, NULL);
  return 0;
}
