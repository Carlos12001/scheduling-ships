#include <cethread.h>

#define NUM_THREADS 5
int contador = 0;  // Variable compartida
my_mutex_t mutex;

// Función que será ejecutada por cada hilo
void* incrementar_contador(void* id) {
    long hilo_id = (long)id;

    // Bloquea el mutex antes de acceder a la variable compartida
    my_mutex_lock(&mutex);

    // Sección crítica
    int temp = contador;
    printf("Hilo %ld lee contador: %d\n", hilo_id, temp);
    temp++;
    printf("Hilo %ld incrementa contador a: %d\n", hilo_id, temp);
    contador = temp;

    // Desbloquea el mutex después de acceder a la variable compartida
    my_mutex_unlock(&mutex);

    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];
    int i;

    // Inicializa el mutex personalizado
    my_mutex_init(&mutex);

    // Crea los hilos
    for (i = 0; i < NUM_THREADS; i++) {
        printf("Creando hilo %d\n", i);
        if (pthread_create(&threads[i], NULL, incrementar_contador, (void*)(long)i)) {
            printf("Error al crear el hilo %d\n", i);
        }
    }

    return 0;
}
