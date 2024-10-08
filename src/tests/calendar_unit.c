#include <cethread.h>
#include <stdio.h>
#include <stdlib.h>

#include "../calendar/calendar.h"
void *rutina(void *arg) {
  Proceso *p = (Proceso *)arg;
  sleep(p->tiempo_total);  // Simula tiempo de ejecución.
  return NULL;
}

void print_processes(Proceso *procesos, int num_procesos) {
  for (int i = 0; i < num_procesos; i++) {
    printf("ID: %d, T.Time: %d, Reaming: %d, P: %d\n", procesos[i].id,
           procesos[i].tiempo_total, procesos[i].tiempo_restante,
           procesos[i].prioridad);
  }
  printf("\n\n");
}

// Función principal main
int main(int argc, char *argv[]) {
  // Crear algunos hilos de ejemplo
  cethread_t hilos[4];  // Instead of pthread_t hilos[4];
  Proceso procesos[4] = {
      {1, 2, 2, 3},  // ID, tiempo de ejecución, tiempo restante, prioridad
      {2, 1, 1, 2},
      {3, 4, 4, 1},
      {4, 3, 3, 4}};
  const int num_procesos = 4;

  for (int option = 1; option < 5; option++) {
    calendar(option, hilos, procesos, num_procesos);
    printf("------------OPCION = %d------------\n", option);

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

    print_processes(procesos, num_procesos);
  }
  return 0;
}