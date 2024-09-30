#include <iostream>
#include <queue>
#include <vector>
#include <algorithm>
#include <chrono>
#include <thread>
#include <limits>
#include <sstream>  // Necesario para std::stringstream

// Estructura para representar un hilo (barco)
struct CEThread {
    int id;
    int priority;     // Prioridad del hilo (para el calendario por prioridad)
    int burst_time;   // Tiempo estimado de ejecución (para SJF y RR)
    int arrival_time; // Tiempo de llegada (para FCFS)
    bool real_time;   // Flag para identificar si es de tiempo real
};

// Función para simular la ejecución de un hilo (barco)
void ejecutarHilo(const CEThread& hilo) {
    std::cout << "Ejecutando hilo " << hilo.id << " por " << hilo.burst_time << " segundos.\n";
    std::this_thread::sleep_for(std::chrono::seconds(hilo.burst_time));
    std::cout << "Hilo " << hilo.id << " terminado.\n";
}

// Algoritmo FCFS (First Come, First Served)
void fcfs(const std::vector<CEThread>& hilos) {
    std::cout << "Ejecutando FCFS...\n";
    // Crear una copia local de los hilos
    std::vector<CEThread> hilos_fcfs = hilos;
    std::sort(hilos_fcfs.begin(), hilos_fcfs.end(), [](const CEThread& a, const CEThread& b) {
        return a.arrival_time < b.arrival_time;
    });
    for (const auto& hilo : hilos_fcfs) {
        ejecutarHilo(hilo);
    }
}

// Algoritmo SJF (Shortest Job First)
void sjf(const std::vector<CEThread>& hilos) {
    std::cout << "Ejecutando SJF...\n";
    // Crear una copia local de los hilos
    std::vector<CEThread> hilos_sjf = hilos;
    std::sort(hilos_sjf.begin(), hilos_sjf.end(), [](const CEThread& a, const CEThread& b) {
        return a.burst_time < b.burst_time;
    });
    for (const auto& hilo : hilos_sjf) {
        ejecutarHilo(hilo);
    }
}

// Algoritmo de Prioridad
void prioridad(const std::vector<CEThread>& hilos) {
    std::cout << "Ejecutando Prioridad...\n";
    // Crear una copia local de los hilos
    std::vector<CEThread> hilos_prioridad = hilos;
    std::sort(hilos_prioridad.begin(), hilos_prioridad.end(), [](const CEThread& a, const CEThread& b) {
        return a.priority > b.priority;  // Mayor prioridad ejecuta primero
    });
    for (const auto& hilo : hilos_prioridad) {
        ejecutarHilo(hilo);
    }
}

// Algoritmo Round Robin
void roundRobin(const std::vector<CEThread>& hilos, int quantum) {
    std::cout << "Ejecutando Round Robin...\n";
    if (quantum <= 0) {
        std::cout << "Error: El quantum debe ser un entero positivo.\n";
        return;
    }

    // Crear una copia local de los hilos con burst_time modificable
    std::vector<CEThread> hilos_rr = hilos;
    std::queue<CEThread> colaRR;
    for (auto& hilo : hilos_rr) {
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
            colaRR.push(hiloActual);           // Reinsertar en la cola
        } else if (hiloActual.burst_time > 0) {
            ejecutarHilo(hiloActual);  // Ejecutar completamente si su tiempo es menor al quantum
        }
    }
}

// Algoritmo de Time Real
void tiempoReal(const std::vector<CEThread>& hilos, int tiempo_max) {
    std::cout << "Ejecutando Tiempo Real...\n";
    if (tiempo_max <= 0) {
        std::cout << "Error: El tiempo máximo debe ser un entero positivo.\n";
        return;
    }

    for (const auto& hilo : hilos) {
        if (hilo.real_time) {
            if (hilo.burst_time > tiempo_max) {
                std::cout << "Error: El hilo " << hilo.id << " no se puede ejecutar en tiempo real, supera el tiempo máximo.\n";
            } else if (hilo.burst_time > 0) {
                ejecutarHilo(hilo);
            } else {
                std::cout << "Error: El hilo " << hilo.id << " tiene un tiempo de ejecución no válido.\n";
            }
        }
    }
}

// Función para validar el entero positivo ;)
int leerEnteroPositivo(const std::string& mensaje) {
    int valor;
    while (true) {
        std::cout << mensaje;
        std::string linea;
        std::getline(std::cin, linea);
        std::stringstream ss(linea);
        if (!(ss >> valor) || (valor <= 0)) {
            std::cout << "Entrada inválida. Por favor, ingrese un número entero positivo.\n";
        } else {
            break; // Entrada válida
        }
    }
    return valor;
}

// Función principal main xd
int main() {
    // Crear algunos hilos de ejemplo
    std::vector<CEThread> hilos_originales = {
        {1, 2, 4, 0, false},  // id, prioridad, burst_time, arrival_time, real_time
        {2, 1, 2, 1, false},
        {3, 3, 1, 2, true},   // hilo de tiempo real
        {4, 2, 3, 3, false}
    };

    int opcion;
    do {
        std::cout << "\nSeleccione el algoritmo de calendarización:\n";
        std::cout << "1. FCFS\n";
        std::cout << "2. SJF\n";
        std::cout << "3. Prioridad\n";
        std::cout << "4. Round Robin\n";
        std::cout << "5. Tiempo Real\n";
        std::cout << "6. Salir\n";
        std::cout << "Ingrese su opción: ";
        std::string linea;
        std::getline(std::cin, linea);
        std::stringstream ss(linea);
        if (!(ss >> opcion) || opcion < 1 || opcion > 6) {
            std::cout << "Opción no válida. Por favor, ingrese un número entre 1 y 6.\n";
            continue;
        }

        switch (opcion) {
            case 1:
                fcfs(hilos_originales);
                break;
            case 2:
                sjf(hilos_originales);
                break;
            case 3:
                prioridad(hilos_originales);
                break;
            case 4: {
                int quantum = leerEnteroPositivo("Ingrese el quantum para Round Robin: ");
                roundRobin(hilos_originales, quantum);
                break;
            }
            case 5: {
                int tiempo_max = leerEnteroPositivo("Ingrese el tiempo máximo para los hilos de tiempo real: ");
                tiempoReal(hilos_originales, tiempo_max);
                break;
            }
            case 6:
                std::cout << "Saliendo del programa.\n";
                break;
            default:
                // break
                break;
        }
    } while (opcion != 6);

    return 0;
}
