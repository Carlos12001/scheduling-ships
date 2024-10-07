#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "vector.h"

// Estructura para representar un hilo (barco)
typedef struct {
    int id;
    int priority;     // Prioridad del hilo (para el calendario por prioridad)
    int burst_time;   // Tiempo estimado de ejecución (para SJF y RR)
    int arrival_time; // Tiempo de llegada (para FCFS)
    int real_time;    // Flag para identificar si es de tiempo real (0 o 1)
} CEThread;

// Función para simular la ejecución de un hilo (barco)
void ejecutarHilo(const CEThread* hilo) {
    printf("Ejecutando hilo %d por %d segundos.\n", hilo->id, hilo->burst_time);
    sleep(hilo->burst_time);
    printf("Hilo %d terminado.\n", hilo->id);
}

// Funciones de comparación para qsort
int compareArrivalTime(const void* a, const void* b) {
    const CEThread* threadA = (const CEThread*)a;
    const CEThread* threadB = (const CEThread*)b;
    return threadA->arrival_time - threadB->arrival_time;
}

int compareBurstTime(const void* a, const void* b) {
    const CEThread* threadA = (const CEThread*)a;
    const CEThread* threadB = (const CEThread*)b;
    return threadA->burst_time - threadB->burst_time;
}

int comparePriority(const void* a, const void* b) {
    const CEThread* threadA = (const CEThread*)a;
    const CEThread* threadB = (const CEThread*)b;
    return threadB->priority - threadA->priority;  // Mayor prioridad ejecuta primero
}

// Función para imprimir el orden de los hilos (para depuración)
void imprimirOrden(const vector* v, const char* mensaje) {
    printf("%s\n", mensaje);
    for (int i = 0; i < vector_total((vector*)v); i++) {
        CEThread* hilo = (CEThread*)vector_get((vector*)v, i);
        printf("Hilo ID: %d, Burst Time: %d, Arrival Time: %d, Priority: %d\n",
               hilo->id, hilo->burst_time, hilo->arrival_time, hilo->priority);
    }
}

// Algoritmo FCFS (First Come, First Served)
void fcfs(vector* hilos) {
    printf("Ejecutando FCFS...\n");
    // Crear una copia local de los hilos
    vector hilos_fcfs;
    vector_init(&hilos_fcfs);
    for (int i = 0; i < vector_total(hilos); i++) {
        CEThread* hilo = (CEThread*)vector_get(hilos, i);
        CEThread* nuevo_hilo = malloc(sizeof(CEThread));
        *nuevo_hilo = *hilo;
        vector_add(&hilos_fcfs, nuevo_hilo);
    }
    // Ordenar por arrival_time
    qsort(hilos_fcfs.items, hilos_fcfs.total, sizeof(void*), compareArrivalTime);
    // Ejecutar hilos
    for (int i = 0; i < vector_total(&hilos_fcfs); i++) {
        CEThread* hilo = (CEThread*)vector_get(&hilos_fcfs, i);
        ejecutarHilo(hilo);
        free(hilo);
    }
    vector_free(&hilos_fcfs);
}

// Algoritmo SJF (Shortest Job First)
void sjf(vector* hilos) {
    printf("Ejecutando SJF...\n");
    // Crear una copia local de los hilos
    vector hilos_sjf;
    vector_init(&hilos_sjf);
    for (int i = 0; i < vector_total(hilos); i++) {
        CEThread* hilo = (CEThread*)vector_get(hilos, i);
        CEThread* nuevo_hilo = malloc(sizeof(CEThread));
        *nuevo_hilo = *hilo;
        vector_add(&hilos_sjf, nuevo_hilo);
    }

    // Imprimir antes de ordenar
    imprimirOrden(&hilos_sjf, "Antes de ordenar SJF:");

    // Ordenar por burst_time
    qsort(hilos_sjf.items, hilos_sjf.total, sizeof(void*), compareBurstTime);

    // Imprimir después de ordenar
    imprimirOrden(&hilos_sjf, "Después de ordenar SJF:");

    // Ejecutar hilos
    for (int i = 0; i < vector_total(&hilos_sjf); i++) {
        CEThread* hilo = (CEThread*)vector_get(&hilos_sjf, i);
        ejecutarHilo(hilo);
        free(hilo);
    }
    vector_free(&hilos_sjf);
}

// Algoritmo de Prioridad
void prioridad(vector* hilos) {
    printf("Ejecutando Prioridad...\n");
    // Crear una copia local de los hilos
    vector hilos_prioridad;
    vector_init(&hilos_prioridad);
    for (int i = 0; i < vector_total(hilos); i++) {
        CEThread* hilo = (CEThread*)vector_get(hilos, i);
        CEThread* nuevo_hilo = malloc(sizeof(CEThread));
        *nuevo_hilo = *hilo;
        vector_add(&hilos_prioridad, nuevo_hilo);
    }
    // Ordenar por prioridad
    qsort(hilos_prioridad.items, hilos_prioridad.total, sizeof(void*), comparePriority);
    // Ejecutar hilos
    for (int i = 0; i < vector_total(&hilos_prioridad); i++) {
        CEThread* hilo = (CEThread*)vector_get(&hilos_prioridad, i);
        ejecutarHilo(hilo);
        free(hilo);
    }
    vector_free(&hilos_prioridad);
}

// Algoritmo Round Robin
void roundRobin(vector* hilos, int quantum) {
    printf("Ejecutando Round Robin...\n");
    if (quantum <= 0) {
        printf("Error: El quantum debe ser un entero positivo.\n");
        return;
    }

    // Crear una copia local de los hilos y sus burst_time restantes
    vector hilos_rr;
    vector_init(&hilos_rr);
    for (int i = 0; i < vector_total(hilos); i++) {
        CEThread* hilo = (CEThread*)vector_get(hilos, i);
        CEThread* nuevo_hilo = malloc(sizeof(CEThread));
        *nuevo_hilo = *hilo;
        vector_add(&hilos_rr, nuevo_hilo);
    }

    int done;
    do {
        done = 1;
        for (int i = 0; i < vector_total(&hilos_rr); i++) {
            CEThread* hilo = (CEThread*)vector_get(&hilos_rr, i);
            if (hilo->burst_time > 0) {
                done = 0;
                if (hilo->burst_time > quantum) {
                    printf("Ejecutando hilo %d por %d segundos.\n", hilo->id, quantum);
                    sleep(quantum);
                    hilo->burst_time -= quantum;
                } else {
                    printf("Ejecutando hilo %d por %d segundos.\n", hilo->id, hilo->burst_time);
                    sleep(hilo->burst_time);
                    hilo->burst_time = 0;
                    printf("Hilo %d terminado.\n", hilo->id);
                }
            }
        }
    } while (!done);

    // Liberar memoria
    for (int i = 0; i < vector_total(&hilos_rr); i++) {
        CEThread* hilo = (CEThread*)vector_get(&hilos_rr, i);
        free(hilo);
    }
    vector_free(&hilos_rr);
}

// Algoritmo de Tiempo Real
void tiempoReal(vector* hilos, int tiempo_max) {
    printf("Ejecutando Tiempo Real...\n");
    if (tiempo_max <= 0) {
        printf("Error: El tiempo máximo debe ser un entero positivo.\n");
        return;
    }

    for (int i = 0; i < vector_total(hilos); i++) {
        CEThread* hilo = (CEThread*)vector_get(hilos, i);
        if (hilo->real_time) {
            if (hilo->burst_time > tiempo_max) {
                printf("Error: El hilo %d no se puede ejecutar en tiempo real, supera el tiempo máximo.\n", hilo->id);
            } else if (hilo->burst_time > 0) {
                ejecutarHilo(hilo);
            } else {
                printf("Error: El hilo %d tiene un tiempo de ejecución no válido.\n", hilo->id);
            }
        }
    }
}

// Función para validar el entero positivo
int leerEnteroPositivo(const char* mensaje) {
    int valor;
    char linea[100];
    while (1) {
        printf("%s", mensaje);
        if (fgets(linea, sizeof(linea), stdin) == NULL) {
            printf("Error leyendo la entrada. Intente de nuevo.\n");
            continue;
        }
        if (sscanf(linea, "%d", &valor) != 1 || valor <= 0) {
            printf("Entrada inválida. Por favor, ingrese un número entero positivo.\n");
        } else {
            break; // Entrada válida
        }
    }
    return valor;
}

// Función principal main
int main() {
    // Crear algunos hilos de ejemplo
    vector hilos_originales;
    vector_init(&hilos_originales);

    CEThread* hilo1 = malloc(sizeof(CEThread));
    *hilo1 = (CEThread){1, 1, 4, 0, 0}; // id, prioridad, burst_time, arrival_time, real_time
    vector_add(&hilos_originales, hilo1);

    CEThread* hilo2 = malloc(sizeof(CEThread));
    *hilo2 = (CEThread){2, 2, 2, 1, 0};
    vector_add(&hilos_originales, hilo2);

    CEThread* hilo3 = malloc(sizeof(CEThread));
    *hilo3 = (CEThread){3, 3, 1, 2, 1}; // hilo de tiempo real
    vector_add(&hilos_originales, hilo3);

    CEThread* hilo4 = malloc(sizeof(CEThread));
    *hilo4 = (CEThread){4, 4, 3, 3, 0};
    vector_add(&hilos_originales, hilo4);

    int opcion;
    char linea[100];
    do {
        printf("\nSeleccione el algoritmo de calendarización:\n");
        printf("1. FCFS\n");
        printf("2. SJF\n");
        printf("3. Prioridad\n");
        printf("4. Round Robin\n");
        printf("5. Tiempo Real\n");
        printf("6. Salir\n");
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
            case 1:
                fcfs(&hilos_originales);
                break;
            case 2:
                sjf(&hilos_originales);
                break;
            case 3:
                prioridad(&hilos_originales);
                break;
            case 4: {
                int quantum = leerEnteroPositivo("Ingrese el quantum para Round Robin: ");
                roundRobin(&hilos_originales, quantum);
                break;
            }
            case 5: {
                int tiempo_max = leerEnteroPositivo("Ingrese el tiempo máximo para los hilos de tiempo real: ");
                tiempoReal(&hilos_originales, tiempo_max);
                break;
            }
            case 6:
                printf("Saliendo del programa.\n");
                break;
            default:
                break;
        }
    } while (opcion != 6);

    // Liberar memoria de hilos originales
    for (int i = 0; i < vector_total(&hilos_originales); i++) {
        CEThread* hilo = (CEThread*)vector_get(&hilos_originales, i);
        free(hilo);
    }
    vector_free(&hilos_originales);

    return 0;
}