#include <stdio.h>

#include "lib/cethread.h"
#include "canal/canal.h"

int main(){
  printf("Hello scheduling-ships!\n");

  cethread_init();
  canal_tryout();
  return 0;
}