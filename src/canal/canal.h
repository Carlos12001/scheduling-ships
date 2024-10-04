// canal.h
#ifndef CANAL_H
#define CANAL_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_LINE_LENGTH 256



typedef struct {
    int ID;
    int position;
    int speed;    //default speed
    int typeboat; // type: 1 Normal
                  //       2 Fishing
                  //       3 Patrol
}boat;

typedef struct {
    boat waiting[5];
    int maxcapacity;
    int capacity;
}waitline;


typedef struct {
    int managed_boats;//De momento uso los ids de aqui
    int size;
    int W;
    int time;
    int *canal;
    int boatspeeds[3];
    bool rightdir;
    int scheduling;   //0 igual
                        //1 semaforo
                        //2 tico
}canal;

typedef struct {
    boat b; // El barco
} BoatWrapper;



extern canal Canal;
extern waitline left_sea;
extern waitline right_sea;

void canal_tryout();

void Canal_init(const char *nombre_archivo);

void create_canal();
void destroy_canal();

void waitline_init(bool right,char *list);

void addboatdummy(bool right,int type);

void *boatmover(void *arg);

void entercanal();

void canalcontent();

#endif // CANAL_H;