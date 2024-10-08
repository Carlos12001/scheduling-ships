// canal.h
#ifndef CANAL_H
#define CANAL_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define MAX_LINE_LENGTH 256



typedef struct {
    pthread_t thread;
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
    int boats_in;
    int size;
    int W;
    int time;
    boat *canal;
    int boatspeeds[3];
    int scheduling;   //0 igual
                        //1 semaforo
                        //2 tico
    bool direction;//True derecha, false izquierda
    bool running;
    bool Yellowlight;//Esta variable es sobre todo para interfaz, una especie de alerta de luz amarilla
    
    bool LeftEmergency;
    bool RightEmergency;
    bool Emergencyswitch;
    int EmergencyAmount;

}canal;




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

void *Canal_Schedule(void *arg);

void BoatGUI();

void YellowCanal();

void EmergencyYellowCanal();

int EnterCanal(int Waitpos,bool queue);

boat GetEnterBoat(int index,bool queue);

void * SoundEmergency(void * arg);

void EmergencyProtocol(bool side,int index);


void WaitRealTime(boat Emergencyboat);
#endif // CANAL_H;