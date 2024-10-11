#include "calendar.h"

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

void adjustPatrol(boat *procesos, int num_procesos) {
  int i = 0;
  for (int j = 0; j < num_procesos; j++) {
    if (procesos[j].typeboat == 3) {  // El barco es una patrulla
      boat tempboat = procesos[j];
      for (int k = j; i < k; k--) {  // Corrimiento de la lista
        procesos[k] = procesos[k - 1];
      }
      procesos[i] = tempboat;  // Colocacion de la patrulla en el top
      i++;
    }
  }
}

bool RealTime(boat *procesos, int num_procesos, boat slowestboat) {
  if (1 < num_procesos) {  // La cantidad de barcos significa que al menos uno
                           // esperara
    return false;
  }
  if (procesos[0].tiempo_total <= slowestboat.tiempo_restante &&
      slowestboat.ID != -1) {  // No le da tiempo de cruzar y cumplir el quantum
    printf("Tiempo real incumplido por barco en canal (%f,%f)\n",
           procesos[0].tiempo_total, slowestboat.tiempo_restante);
    return false;
  }
  return true;
}

bool calendar(int option, boat *procesos, int num_procesos, boat slowestboat) {
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
      bool response = (RealTime(procesos, num_procesos, slowestboat));
      return response;
    }
    default:
      break;
  }
  adjustPatrol(procesos, num_procesos);
  return true;
}
