#include "calendar.h"

#define NUM_THREADS 3
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
  boat *info = (boat *)arg;

  while (1) {
    cemutex_lock(&mutex);

    // Verifica si es su turno y si aún tiene trabajo
    if (current_thread == info->ID && !thread_finished[info->ID]) {
      // Simula la ejecución del hilo
      printf("Hilo %d está ejecutando. Unidades de trabajo restantes: %f\n",
             info->ID, info->tiempo_restante);
      cemutex_unlock(&mutex);  // Libera el mutex antes de dormir
      sleep(QUANTUM_mSEC);      // Simula el quantum

      cemutex_lock(&mutex);  // Re-adquiere el mutex después de dormir
      info->tiempo_restante--;
      if (info->tiempo_restante == 0) {
        printf("Hilo %d ha terminado.\n", info->ID);
        threads_done++;
        thread_finished[info->ID] = 1;
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
  boat *p1 = (boat *)a;
  boat *p2 = (boat *)b;
  return p1->tiempo_total - p2->tiempo_total;
}

int comparar_por_prioridad(const void *a, const void *b) {
  boat *p1 = (boat *)a;
  boat *p2 = (boat *)b;
  return p1->typeboat - p2->typeboat;
}

void *rutina(void *arg) {
  boat *p = (boat *)arg;
  printf("Iniciando proceso %d con tiempo de ejecución %f\n", p->ID,
         p->tiempo_total);
  sleep(p->tiempo_total);  // Simula tiempo de ejecución.
  printf("boat %d completado\n", p->ID);
  return NULL;
}

int round_robin(boat *procesos, int num_procesos, size_t size_struct) {
  // Ensure there are at least two processes to rotate
  if (num_procesos <= 1) {
    return 0;  // No rotation needed
  }

  // Temporary storage for the first process
  boat temp;

  // Copy the first process into the temporary variable
  memcpy(&temp, procesos, size_struct);

  // Shift all processes one position forward
  memmove(procesos, procesos + 1, (num_procesos - 1) * size_struct);

  // Place the first process at the end of the array
  memcpy(procesos + (num_procesos - 1), &temp, size_struct);

  return 0;  // Indicate successful operation
}

void adjustPatrol(boat * procesos,int num_procesos){
  int i =0;
  for(int j=0;j<num_procesos;j++){
    if(procesos[j].typeboat==3){//El barco es una patrulla
      boat tempboat=procesos[j];
      for(int k=j;i<k;k--){//Corrimiento de la lista
        procesos[k]=procesos[k-1];
      }
      procesos[i]=tempboat;//Colocacion de la patrulla en el top
      i++;
    }
  }
}

bool RealTime(boat * procesos, int num_procesos, boat slowestboat){
  if(1<num_procesos){//La cantidad de barcos significa que al menos uno esperara 
    return false;
  }if(procesos[0].tiempo_total<=slowestboat.tiempo_restante&&slowestboat.ID!=-1){//No le da tiempo de cruzar y cumplir el quantum
    printf("Tiempo real incumplido por barco en canal (%f,%f)\n",procesos[0].tiempo_total,slowestboat.tiempo_restante);
    return false;
  }
  return true;
}

bool calendar(int option, boat *procesos, int num_procesos,boat slowestboat) {
  switch (option) {
    case 1: {
      break;
    }
    case 2: {
      // Ordenar procesos por tiempo de ejecución
      qsort(procesos, num_procesos, sizeof(boat), comparar_por_tiempo);
      break;
    }
    case 3: {
      // Ordenar procesos por tiempo de ejecución
      qsort(procesos, num_procesos, sizeof(boat), comparar_por_prioridad);
      break;
    }
    case 4: {
      round_robin(procesos, num_procesos, sizeof(boat));
      break;
    }
    case 5: {
      bool response = (RealTime(procesos,num_procesos,slowestboat));
      return response;
    }
    default:
      break;
  }
  adjustPatrol(procesos,num_procesos);
  return true;
}
