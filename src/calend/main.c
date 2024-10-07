#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//#include <pthread.h>

#include "../lib/cethread.h"

typedef struct {
    int id;
    int tiempo_de_ejecucion;
    int prioridad; // Para algoritmos de prioridad.
} Proceso;

void *rutina(void *arg) {
    Proceso *p = (Proceso *)arg;
    printf("Iniciando proceso %d con tiempo de ejecución %d\n", p->id, p->tiempo_de_ejecucion);
    sleep(p->tiempo_de_ejecucion); // Simula tiempo de ejecución.
    printf("Proceso %d completado\n", p->id);
    return NULL;
}

int main() {
    //pthread_t hilos[4];

    cethread_t hilos[4];

    Proceso procesos[4] = {
        {1, 2, 3},  // ID, tiempo de ejecución, prioridad
        {2, 1, 2},
        {3, 3, 1},
        {4, 4, 4}
    };

    // Crear hilos
    for (int i = 0; i < 4; i++) {
        //if (pthread_create(&hilos[i], NULL, rutina, (void *)&procesos[i])) {
        if (cethread_create(&hilos[i], rutina, (void *)&procesos[i])) {
            fprintf(stderr, "Error al crear el hilo %d\n", i);
            return 1;
        }
        // Esperar a que el hilo actual termine antes de crear el siguiente
        //pthread_join(hilos[i], NULL);
        cethread_join(hilos[i], NULL);
    }


    printf("Todos los procesos han completado.\n");
    return 0;
}
