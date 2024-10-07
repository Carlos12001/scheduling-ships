#include <cethread.h>
#include <stdio.h>
#include <unistd.h>

void* thread_func(void* arg) {
  for (int i = 0; i < 3; i++) {
    sleep(1);
    printf("CEthread started: %ld\n", (long)arg);
  }
  return NULL;
}

int main() {
  cethread_t thread;
  cethread_create(&thread, thread_func, (void*)1);
  cethread_join(thread, NULL);
  return 0;
}
