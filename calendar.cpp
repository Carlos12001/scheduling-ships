#include <iostream>
#include <queue>
#include <vector>
#include <algorithm>
#include <chrono>
#include <thread>

// Estructura para representar un hilo (barco)
struct CEThread {
    int id;
    int priority;   // Prioridad del hilo (para el calendario por prioridad)
    int burst_time; // Tiempo estimado de ejecución (para SJF y RR)
    int arrival_time; // Tiempo de llegada (para FCFS)
    bool real_time;  // Flag para identificar si es de tiempo real
};

// Función para simular la ejecución de un hilo (barco)
void ejecutarHilo(CEThread hilo) {
    std::cout << "Ejecutando hilo " << hilo.id << " por " << hilo.burst_time << " segundos.\n";
    std::this_thread::sleep_for(std::chrono::seconds(hilo.burst_time));
    std::cout << "Hilo " << hilo.id << " terminado.\n";
}

// Algoritmo FCFS (First Come, First Served)
void fcfs(std::vector<CEThread>& hilos) {
    std::cout << "Ejecutando FCFS...\n";
    std::sort(hilos.begin(), hilos.end(), [](CEThread a, CEThread b) {
        return a.arrival_time < b.arrival_time;
    });
    for (auto& hilo : hilos) {
        ejecutarHilo(hilo);
    }
}

// Algoritmo SJF (Shortest Job First)
void sjf(std::vector<CEThread>& hilos) {
    std::cout << "Ejecutando SJF...\n";
    std::sort(hilos.begin(), hilos.end(), [](CEThread a, CEThread b) {
        return a.burst_time < b.burst_time;
    });
    for (auto& hilo : hilos) {
        ejecutarHilo(hilo);
    }
}

// Algoritmo de Prioridad
void prioridad(std::vector<CEThread>& hilos) {
    std::cout << "Ejecutando Prioridad...\n";
    std::sort(hilos.begin(), hilos.end(), [](CEThread a, CEThread b) {
        return a.priority > b.priority;  // Mayor prioridad ejecuta primero
    });
    for (auto& hilo : hilos) {
        ejecutarHilo(hilo);
    }
}

// Algoritmo Round Robin
void roundRobin(std::vector<CEThread>& hilos, int quantum) {
    std::cout << "Ejecutando Round Robin...\n";
    std::queue<CEThread> colaRR;
    for (auto& hilo : hilos) {
        colaRR.push(hilo);
    }

    while (!colaRR.empty()) {
        CEThread hiloActual = colaRR.front();
        colaRR.pop();

        // Si el tiempo de burst es mayor que el quantum, se divide
        if (hiloActual.burst_time > quantum) {
            std::cout << "Ejecutando hilo " << hiloActual.id << " por " << quantum << " segundos.\n";
            std::this_thread::sleep_for(std::chrono::seconds(quantum));
            hiloActual.burst_time -= quantum;  // Reducir el tiempo restante
            colaRR.push(hiloActual);  // Reinsertar en la cola
        } else {
            ejecutarHilo(hiloActual);  // Ejecutar completamente si su tiempo es menor al quantum
        }
    }
}

// Algoritmo de Tiempo Real
void tiempoReal(std::vector<CEThread>& hilos, int tiempo_max) {
    std::cout << "Ejecutando Tiempo Real...\n";
    for (auto& hilo : hilos) {
        if (hilo.real_time) {
            if (hilo.burst_time > tiempo_max) {
                std::cout << "Error: El hilo " << hilo.id << " no se puede ejecutar en tiempo real, supera el tiempo máximo.\n";
            } else {
                ejecutarHilo(hilo);
            }
        }
    }
}

// Función principal
int main() {
    // Crear algunos hilos de ejemplo
    std::vector<CEThread> hilos = {
        {1, 2, 4, 0, false},  // id, prioridad, burst_time, arrival_time, real_time
        {2, 1, 2, 1, false},
        {3, 3, 1, 2, true},   // hilo de tiempo real
        {4, 2, 3, 3, false}
    };

    int opcion;
    std::cout << "Seleccione el algoritmo de calendarización:\n";
    std::cout << "1. FCFS\n";
    std::cout << "2. SJF\n";
    std::cout << "3. Prioridad\n";
    std::cout << "4. Round Robin\n";
    std::cout << "5. Tiempo Real\n";
    std::cin >> opcion;

    switch (opcion) {
        case 1:
            fcfs(hilos);
            break;
        case 2:
            sjf(hilos);
            break;
        case 3:
            prioridad(hilos);
            break;
        case 4: {
            int quantum;
            std::cout << "Ingrese el quantum para Round Robin: ";
            std::cin >> quantum;
            roundRobin(hilos, quantum);
            break;
        }
        case 5: {
            int tiempo_max;
            std::cout << "Ingrese el tiempo máximo para los hilos de tiempo real: ";
            std::cin >> tiempo_max;
            tiempoReal(hilos, tiempo_max);
            break;
        }
        default:
            std::cout << "Opción no válida.\n";
            break;
    }

    return 0;
}
