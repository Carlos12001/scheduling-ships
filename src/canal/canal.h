// canal.h
#ifndef CANAL_H
#define CANAL_H
#include <cethread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "../calendar/calendar.h"

#define MAX_LINE_LENGTH 256

typedef struct {
  boat *waiting;
  int maxcapacity;
  int capacity;
} waitline;

typedef struct {

  int managed_boats;  // De momento uso los ids de aqui
  int boats_in;
  int size;
  boat *canal;

  int W;
  int time;
  int boatspeeds[3];

  int canal_scheduling; // 1 igual
                        // 2 semaforo
                        // 3 tico

  int thread_scheduling;// 1 FCFS
                        // 2 SJF
                        // 3 Prioridad
                        // 4 RR
                        // 3 RealTime
                        
  bool direction;  // True derecha, false izquierda
  bool running;

  bool Yellowlight;  // Esta variable es sobre todo para interfaz, una especie
                     // de alerta de luz amarilla

  bool TiempoReal;
  int RRiter;  
  int RRID;

} canal;

extern canal Canal;
extern waitline left_sea;
extern waitline right_sea;

void canal_tryout();

void Canal_init(const char *nombre_archivo);

void create_canal();
void waitline_create();
void destroy_canal();

void waitline_init(bool right, char *list);

void addboatdummy(bool right, int type);

void *boatmover(void *arg);

void entercanal();

void canalcontent();

void *Canal_Schedule(void *arg);

void BoatGUI();

void YellowCanal();

int EnterCanal(int Waitpos, bool queue);

boat GetEnterBoat(int index, bool queue);

boat GetSlowestBoat();

void Canal_RR();

void CheckRealTime();
#endif  // CANAL_H;