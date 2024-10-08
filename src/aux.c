#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_THREADS 3
#define QUANTUM_SEC 1  // Duración del quantum en segundos
#define SLEEP_USEC 100000  // Tiempo de espera en microsegundos (0.1 segundos)

pthread_mutex_t mutex;
int current_thread = 0;
int threads_done = 0;
int thread_finished[NUM_THREADS] = {0}; // Arreglo para rastrear hilos terminados

// Estructura para pasar información a cada hilo
typedef struct {
    int thread_id;
    int work_units; // Simula trabajo a realizar
} thread_info_t;

// Función para encontrar el siguiente hilo activo
int find_next_thread(int current) {
    for(int i=1; i<=NUM_THREADS; i++) {
        int next = (current + i) % NUM_THREADS;
        if (!thread_finished[next]) {
            return next;
        }
    }
    return -1; // Indica que todos los hilos han terminado
}

// Función que cada hilo ejecutará
void* thread_function(void* arg) {
    thread_info_t* info = (thread_info_t*)arg;

    while (1) {
        pthread_mutex_lock(&mutex);

        // Verifica si es su turno y si aún tiene trabajo
        if (current_thread == info->thread_id && !thread_finished[info->thread_id]) {
            // Simula la ejecución del hilo
            printf("Hilo %d está ejecutando. Unidades de trabajo restantes: %d\n", 
                   info->thread_id, info->work_units);
            pthread_mutex_unlock(&mutex); // Libera el mutex antes de dormir
            sleep(QUANTUM_SEC); // Simula el quantum

            pthread_mutex_lock(&mutex); // Re-adquiere el mutex después de dormir
            info->work_units--;
            if (info->work_units == 0) {
                printf("Hilo %d ha terminado.\n", info->thread_id);
                threads_done++;
                thread_finished[info->thread_id] = 1;
            }

            // Asigna el turno al siguiente hilo activo
            int next_thread = find_next_thread(current_thread);
            if (next_thread != -1) {
                current_thread = next_thread;
            } else {
                // Si no hay hilos activos, finalizar la planificación
                threads_done = NUM_THREADS;
            }

            pthread_mutex_unlock(&mutex);
        } else {
            pthread_mutex_unlock(&mutex);
            // Espera brevemente antes de reintentar
            usleep(SLEEP_USEC);
        }

        // Verifica si todos los hilos han terminado
        pthread_mutex_lock(&mutex);
        if (threads_done >= NUM_THREADS) {
            pthread_mutex_unlock(&mutex);
            break; // Sale del bucle si todo ha terminado
        }
        pthread_mutex_unlock(&mutex);
    }

    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];
    thread_info_t thread_info[NUM_THREADS];
    int i, rc;

    // Inicializa mutex
    pthread_mutex_init(&mutex, NULL);

    // Crea los hilos
    for (i = 0; i < NUM_THREADS; i++) {
        thread_info[i].thread_id = i;
        thread_info[i].work_units = 3 + i; // Diferentes unidades de trabajo para cada hilo
        rc = pthread_create(&threads[i], NULL, thread_function, (void*)&thread_info[i]);
        if (rc) {
            fprintf(stderr, "Error al crear el hilo %d, código de error: %d\n", i, rc);
            exit(EXIT_FAILURE);
        }
    }

    // Espera a que todos los hilos terminen
    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Destruye mutex
    pthread_mutex_destroy(&mutex);

    printf("Todos los hilos han finalizado.\n");
    pthread_exit(NULL);
}
