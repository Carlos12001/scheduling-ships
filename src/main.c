#include <stdio.h>

#include "calendar/calendar.h"
#include "canal/canal.h"
#include "lib/cethread.h"

int main() {
  printf("Hello scheduling-ships!\n");
  canal_tryout();
  return 0;
}