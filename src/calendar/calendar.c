#include "calendar.h"

#define NUM_THREADS 3
#define QUANTUM_SEC 1      // Duración del quantum en segundos
#define SLEEP_USEC 100000  // Tiempo de espera en microsegundos (0.1 segundos)

cemutex mutex;
int current_thread = 0;
int threads_done = 0;
int thread_finished[NUM_THREADS] = {
    0};  // Arreglo para rastrear hilos terminados

// Función para encontrar el siguiente hilo activo
int find_next_thread(int current) {
  for (int i = 1; i <= NUM_THREADS; i++) {
    int next = (current + i) % NUM_THREADS;
    if (!thread_finished[next]) {
      return next;
    }
  }
  return -1;  // Indica que todos los hilos han terminado
}

// Función que cada hilo ejecutará
// Función que cada hilo ejecutará
void *thread_function(void *arg) {
  Proceso *info = (Proceso *)arg;

  while (1) {
    cemutex_lock(&mutex);

    // Verifica si es su turno y si aún tiene trabajo
    if (current_thread == info->id && !thread_finished[info->id]) {
      // Simula la ejecución del hilo
      printf("Hilo %d está ejecutando. Unidades de trabajo restantes: %d\n",
             info->id, info->tiempo_restante);
      cemutex_unlock(&mutex);  // Libera el mutex antes de dormir
      sleep(QUANTUM_SEC);      // Simula el quantum

      cemutex_lock(&mutex);  // Re-adquiere el mutex después de dormir
      info->tiempo_restante--;
      if (info->tiempo_restante == 0) {
        printf("Hilo %d ha terminado.\n", info->id);
        threads_done++;
        thread_finished[info->id] = 1;
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
      break;  // Sale del bucle si todo ha terminado
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

int round_robin(Proceso *procesos, int num_procesos, size_t size_struct) {
  // Ensure there are at least two processes to rotate
  if (num_procesos <= 1) {
    return 0;  // No rotation needed
  }

  // Temporary storage for the first process
  Proceso temp;

  // Copy the first process into the temporary variable
  memcpy(&temp, procesos, size_struct);

  // Shift all processes one position forward
  memmove(procesos, procesos + 1, (num_procesos - 1) * size_struct);

  // Place the first process at the end of the array
  memcpy(procesos + (num_procesos - 1), &temp, size_struct);

  return 0;  // Indicate successful operation
}

void *calendar(int option, cethread_t *hilos, Proceso *procesos,
               int num_procesos) {
  switch (option) {
    case 1: {
      break;
    }
    case 2: {
      // Ordenar procesos por tiempo de ejecución
      qsort(procesos, num_procesos, sizeof(Proceso), comparar_por_tiempo);
      break;
    }
    case 3: {
      // Ordenar procesos por tiempo de ejecución
      qsort(procesos, num_procesos, sizeof(Proceso), comparar_por_prioridad);
      break;
    }
    case 4: {
      round_robin(procesos, num_procesos, sizeof(Proceso));
      break;
      // cethread_t threads[NUM_THREADS];
      // Proceso procesos[NUM_THREADS];

      // // Inicializa mutex
      // cemutex_init(&mutex);

      // // Crea los hilos
      // for (int i = 0; i < NUM_THREADS; i++) {
      //   procesos[i].id = i;
      //   procesos[i].tiempo_restante =
      //       3 + i;  // Diferentes unidades de trabajo para cada hilo
      //   int rc =
      //       cethread_create(&threads[i], thread_function, (void
      //       *)&procesos[i]);
      //   if (rc) {
      //     fprintf(stderr, "Error al crear el hilo %d, código de error: %d\n",
      //     i,
      //             rc);
      //     exit(EXIT_FAILURE);
      //   }
      // }

      // // Espera a que todos los hilos terminen
      // for (int i = 0; i < NUM_THREADS; i++) {
      //   cethread_join(threads[i], NULL);
      // }

      // // Destruye mutex
      // cemutex_destroy(&mutex);

      // printf("Todos los hilos han finalizado.\n");
      // cethread_end(NULL);
    }
    case 5: {
      printf("Saliendo del programa.\n");
      break;
    }
    default:
      break;
  }

  return NULL;
}
