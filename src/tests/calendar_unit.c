#include <cethread.h>
#include <stdio.h>
#include <stdlib.h>

#include "../calendar/calendar.h"
void *rutina(void *arg) {
  boat *p = (boat *)arg;
  sleep(p->tiempo_total);  // Simula tiempo de ejecución.
  return NULL;
}

void print_processes(boat *procesos, int num_procesos) {
  for (int i = 0; i < num_procesos; i++) {
    printf("ID: %d, T.Time: %.2f, Reaming: %.2f, P: %d\n", procesos[i].ID,
           procesos[i].tiempo_total, procesos[i].tiempo_restante,
           procesos[i].typeboat);
  }
  printf("\n\n");
}

// Función principal main
int main(int argc, char *argv[]) {
  // Crear algunos hilos de ejemplo
  cethread_t hilos[4];
  boat procesos[4] = {{hilos[0], 10, -2, -1, 2, 2, 3},
                      {hilos[1], 20, -2, -1, 1, 1, 2},
                      {hilos[2], 30, -2, -1, 4, 4, 1},
                      {hilos[3], 40, -2, -1, 3, 3, 4}};
  const int num_procesos = 4;



  for (int option = 1; option < 5; option++) {
    calendar(option, procesos, num_procesos,procesos[0]);
    printf("------------OPCION = %d------------\n", option);

    // Crear hilos
    for (int i = 0; i < 4; i++) {
      if (cethread_create(&hilos[i], rutina, (void *)&procesos[i])) {
        fprintf(stderr, "Error al crear el hilo %d\n", i);
        return 1;
      }
      // Esperar a que el hilo actual termine antes de crear el siguiente
      cethread_join(hilos[i], NULL);
    }

    print_processes(procesos, num_procesos);
  }
  return 0;
}