#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Thread function
void* func1(void* arg) {
  size_t n = *(size_t*)arg;
  for (size_t i = 0; i < n; i++) {
    sleep(1);
    printf("Pthread_t1 started: %zu\n", i);
  }
  return NULL;
}

// Another thread function
void* func2(void* arg) {
  size_t n = *(size_t*)arg;
  for (size_t i = 0; i < n; i++) {
    sleep(1);
    printf("Pthread_t2 started: %zu\n", i);
  }
  return NULL;
}

int main(int argc, char* argv[]) {
  size_t n = (argc == 3) ? (size_t)atoi(argv[1]) : 6;
  size_t n2 = (argc == 3) ? (size_t)atoi(argv[2]) : 3;

  pthread_t thread1;
  pthread_create(&thread1, NULL, func1, (void*)&n);
  func2((void*)&n2);
  pthread_join(thread1, NULL);

  return 0;
}
