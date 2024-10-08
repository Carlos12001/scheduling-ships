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

void *calendar(int option, boat *procesos, int num_procesos,
               int round_robin_flag, int slowest_boat) {
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
      if (round_robin_flag) round_robin(procesos, num_procesos, sizeof(boat));
      break;
    }
    case 5: {
      if (slowest_boat == -1) {
        // Ordenar procesos por tiempo de ejecución
        qsort(procesos, num_procesos, sizeof(boat), comparar_por_tiempo);
      } else {
        // Pendiente: Sino
      }

      // TODO: ordenar a tiempo real si velocidad -1 entonces
      // ordenar SJF
      // SINO ordenar primero los de igual velocidad
      // luego los los mas lentos y por ultimo los mas rapidos
      break;
    }
    default:
      break;
  }

  return NULL;
}
