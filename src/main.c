#include <stdio.h>

#include "lib/cethread.h"

int main() {
  printf("Hello scheduling-ships!\n");
  cethread_init();
  return 0;
}
