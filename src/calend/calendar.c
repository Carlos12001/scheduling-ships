#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../lib/cethread.h"

#define NUM_THREADS 3
#define QUANTUM_SEC 1  // Duración del quantum en segundos
#define SLEEP_USEC 100000  // Tiempo de espera en microsegundos (0.1 segundos)

typedef struct {
  int id;
  int tiempo_total;
  int tiempo_restante;
  int prioridad;  // Para algoritmos de prioridad.
} Proceso;

// Estructura para pasar información a cada hilo
typedef struct {
    int thread_id;
    int work_units; // Simula trabajo a realizar
} thread_info_t;

cemutex mutex;
int current_thread = 0;
int threads_done = 0;
int thread_finished[NUM_THREADS] = {0}; // Arreglo para rastrear hilos terminados


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
        cemutex_lock(&mutex);

        // Verifica si es su turno y si aún tiene trabajo
        if (current_thread == info->thread_id && !thread_finished[info->thread_id]) {
            // Simula la ejecución del hilo
            printf("Hilo %d está ejecutando. Unidades de trabajo restantes: %d\n", 
                   info->thread_id, info->work_units);
            cemutex_unlock(&mutex); // Libera el mutex antes de dormir
            sleep(QUANTUM_SEC); // Simula el quantum

            cemutex_lock(&mutex); // Re-adquiere el mutex después de dormir
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

            cemutex_unlock(&mutex);
        } else {
            cemutex_unlock(&mutex);
            // Espera brevemente antes de reintentar
            usleep(SLEEP_USEC);
        }

        // Verifica si todos los hilos han terminado
        cemutex_lock(&mutex);
        if (threads_done >= NUM_THREADS) {
            cemutex_unlock(&mutex);
            break; // Sale del bucle si todo ha terminado
        }
        cemutex_unlock(&mutex);
    }

    cethread_end(NULL);
}

// Función para validar el entero positivo
int leerEnteroPositivo(const char *mensaje) {
  int valor;
  char linea[100];
  while (1) {
    printf("%s", mensaje);
    if (fgets(linea, sizeof(linea), stdin) == NULL) {
      printf("Error leyendo la entrada. Intente de nuevo.\n");
      continue;
    }
    if (sscanf(linea, "%d", &valor) != 1 || valor <= 0) {
      printf(
          "Entrada inválida. Por favor, ingrese un número entero positivo.\n");
    } else {
      break;  // Entrada válida
    }
  }
  return valor;
}

int comparar_por_tiempo(const void *a, const void *b) {
  Proceso *p1 = (Proceso *)a;
  Proceso *p2 = (Proceso *)b;
  return p1->tiempo_total - p2->tiempo_total;
}

int comparar_por_prioridad(const void *a, const void *b) {
  Proceso *p1 = (Proceso *)a;
  Proceso *p2 = (Proceso *)b;
  return p1->prioridad - p2->prioridad;
}

void *rutina(void *arg) {
  Proceso *p = (Proceso *)arg;
  printf("Iniciando proceso %d con tiempo de ejecución %d\n", p->id,
         p->tiempo_total);
  sleep(p->tiempo_total);  // Simula tiempo de ejecución.
  printf("Proceso %d completado\n", p->id);
  return NULL;
}



// Función principal main
int menu() {
  // Crear algunos hilos de ejemplo
  cethread_t hilos[4];  // Instead of pthread_t hilos[4];
  Proceso procesos[4] = {
      {1, 7, 7, 3},  // ID, tiempo de ejecución, tiempo restante, prioridad
      {2, 1, 1, 2},
      {3, 4, 4, 1},
      {4, 3, 3, 4}};

  int opcion;
  char linea[100];
  do {
    printf("\nSeleccione el algoritmo de calendarización:\n");
    printf("1. FCFS\n");
    printf("2. SJF\n");
    printf("3. Prioridad\n");
    printf("4. Round Robin\n");
    printf("5. Salir\n");
    printf("Ingrese su opción: ");
    if (fgets(linea, sizeof(linea), stdin) == NULL) {
      printf("Error leyendo la entrada. Intente de nuevo.\n");
      continue;
    }
    if (sscanf(linea, "%d", &opcion) != 1 || opcion < 1 || opcion > 6) {
      printf("Opción no válida. Por favor, ingrese un número entre 1 y 6.\n");
      continue;
    }

    switch (opcion) {
      case 1: {
        // Crear hilos
        for (int i = 0; i < 4; i++) {
          if (cethread_create(
                  &hilos[i], rutina,
                  (void *)&procesos[i])) {  // Instead of if
                                            // (pthread_create(&hilos[i], NULL,
                                            // rutina, (void *)&procesos[i])) {
            fprintf(stderr, "Error al crear el hilo %d\n", i);
            return 1;
          }
          // Esperar a que el hilo actual termine antes de crear el siguiente
          cethread_join(hilos[i],
                        NULL);  // Instead of pthread_join(hilos[i], NULL);
        }
        break;
      }
      case 2: {
        // Ordenar procesos por tiempo de ejecución
        qsort(procesos, 4, sizeof(Proceso), comparar_por_tiempo);

        // Crear hilos
        for (int i = 0; i < 4; i++) {
          if (cethread_create(
                  &hilos[i], rutina,
                  (void *)&procesos[i])) {  // Instead of if
                                            // (pthread_create(&hilos[i], NULL,
                                            // rutina, (void *)&procesos[i])) {
            fprintf(stderr, "Error al crear el hilo %d\n", i);
            return 1;
          }
          // Esperar a que el hilo actual termine antes de crear el siguiente
          cethread_join(hilos[i],
                        NULL);  // Instead of pthread_join(hilos[i], NULL);
        }
        break;
      }
      case 3: {
        // Ordenar procesos por tiempo de ejecución
        qsort(procesos, 4, sizeof(Proceso), comparar_por_prioridad);

        // Crear hilos
        for (int i = 0; i < 4; i++) {
          if (cethread_create(
                  &hilos[i], rutina,
                  (void *)&procesos[i])) {  // Instead of if
                                            // (pthread_create(&hilos[i], NULL,
                                            // rutina, (void *)&procesos[i])) {
            fprintf(stderr, "Error al crear el hilo %d\n", i);
            return 1;
          }
          // Esperar a que el hilo actual termine antes de crear el siguiente
          cethread_join(hilos[i],
                        NULL);  // Instead of pthread_join(hilos[i], NULL);
        }
        break;
      }
      case 4: {
        cethread_t threads[NUM_THREADS];
        thread_info_t thread_info[NUM_THREADS];
        int i, rc;

        // Inicializa mutex
        cemutex_init(&mutex);

        // Crea los hilos
        for (i = 0; i < NUM_THREADS; i++) {
            thread_info[i].thread_id = i;
            thread_info[i].work_units = 3 + i; // Diferentes unidades de trabajo para cada hilo
            rc = cethread_create(&threads[i], thread_function, (void*)&thread_info[i]);
            if (rc) {
                fprintf(stderr, "Error al crear el hilo %d, código de error: %d\n", i, rc);
                exit(EXIT_FAILURE);
            }
        }

        // Espera a que todos los hilos terminen
        for (i = 0; i < NUM_THREADS; i++) {
            cethread_join(threads[i], NULL);
        }

        // Destruye mutex
        cemutex_destroy(&mutex);

        printf("Todos los hilos han finalizado.\n");
        cethread_end(NULL);
        break;
      }
      case 5: {
        printf("Saliendo del programa.\n");
        break;
      }
      default:
        break;
    }
  } while (opcion != 5);

  return 0;
}
