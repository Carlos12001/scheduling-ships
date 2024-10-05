#include <stdio.h>

#include "cethread.h"

// Function that will be run by the thread
void *thread_function(void *arg) {
  int *num = (int *)arg;
  printf("The number is: %d\n", *num);
  cethread_end(
      (void *)(long)(*num * 2));  // Convert to long to avoid casting warnings
  // At this point, the thread is finished and the return value has been set
  printf("This line will not be executed\n");
  return NULL;
}

int main() {
  cethread_t thread;
  int arg = 10;
  void *retval;

  // Create the thread
  if (cethread_create(&thread, thread_function, &arg) != 0) {
    perror("Error creating the thread");
    return -1;
  }

  // Wait for the thread to finish and get the return value
  if (cethread_join(thread, &retval) != 0) {
    perror("Error joining the thread");
    return -1;
  }

  // Get the return value
  if (retval == NULL) {
    perror("Error getting the return value");
    return -1;
  }

  // Check the return value
  if ((long)retval != arg * 2) {
    printf("Wrong return value: %ld\n", (long)retval);
    return -1;
  }

  // Print the return value
  printf("The thread returned: %ld\n", (long)retval);
  return 0;
}
