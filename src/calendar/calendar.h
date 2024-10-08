#ifndef CALENDAR_H
#define CALENDAR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../lib/cethread.h"

typedef struct {
  int id;
  int tiempo_total;
  int tiempo_restante;
  int prioridad;  // Para algoritmos de prioridad.
} Proceso;

void *calendar(int option, Proceso *procesos, int num_procesos
               // TODO: agregar velocidad del barco mas lento
               // TODO: flag para round robin que lo haga
);

#endif  // CALENDAR_H
