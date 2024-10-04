#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

void* thread_func(void* arg) {
  for (int i = 0; i < 3; i++) {
    sleep(1);
    printf("PThread started: %ld\n", (long)arg);
  }
  return NULL;
}

int main() {
  pthread_t thread;
  pthread_create(&thread, NULL, thread_func, (void*)1);
  pthread_join(thread, NULL);
  return 0;
}
