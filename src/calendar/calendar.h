#ifndef CALENDAR_H
#define CALENDAR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../lib/cethread.h"

typedef struct {
  cethread_t thread;
  int ID;
  int position;
  int speed;
  float tiempo_total;
  float tiempo_restante;
  // Sirve de typeboat
  int typeboat;  // type: 1 Normal
                 //       2 Fishing
                 //       3 Patrol
} boat;

void *calendar(int option, boat *procesos, int num_procesos,
               int round_robin_flag, int slowest_boat);

#endif  // CALENDAR_H
