#include <stdio.h>
#include <stdlib.h>

#include "cethread.h"
int mails = 0;
cemutex mutex;

void* routine(void* arg) {
  for (size_t i = 0; i < 10000000; i++) {
    cemutex_lock(&mutex);    // read mails
    mails++;                 // increment
    cemutex_unlock(&mutex);  // write mails
  }

  cethread_end(NULL);
  printf("THIS SHOULD NEVER BE PRINTED\n");
  return NULL;
}

int main(int argc, char* argv[]) {
  cethread_t p1, p2, p3, p4;
  cemutex_init(&mutex);
  if (cethread_create(&p1, &routine, NULL) != 0) {
    return 1;
  }
  if (cethread_create(&p2, &routine, NULL) != 0) {
    return 2;
  }
  if (cethread_create(&p3, &routine, NULL) != 0) {
    return 3;
  }
  if (cethread_create(&p4, &routine, NULL) != 0) {
    return 4;
  }

  // Wait for the thread to finish
  if (cethread_join(p1, NULL) != 0) {
    perror("Error joining the thread p1.\n");
    return 5;
  }

  // Wait for the thread to finish
  if (cethread_join(p2, NULL) != 0) {
    perror("Error joining the thread p2.\n");
    return 6;
  }

  // Wait for the thread to finish
  if (cethread_join(p3, NULL) != 0) {
    printf("Error joining the thread p3.\n");
    return 7;
  }

  // Wait for the thread to finish
  if (cethread_join(p4, NULL) != 0) {
    perror("Error joining the thread p4.\n");
    return 8;
  }

  cemutex_destroy(&mutex);
  printf("Number of mails: %d\n", mails);
  return 0;
}
