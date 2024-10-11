#ifndef CALENDAR_H
#define CALENDAR_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../lib/cethread.h"

#define QUANTUM_mSEC 3000

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
  bool Permission;

} boat;

bool calendar(int option, boat *procesos, int num_procesos, boat slowestboat);

#endif  // CALENDAR_H
