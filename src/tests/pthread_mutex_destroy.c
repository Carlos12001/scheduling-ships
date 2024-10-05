#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    pthread_mutex_t lock;
    int status;

    // Inicializar el mutex primero
    status = pthread_mutex_init(&lock, NULL);
    if (status != 0) {
        printf("Error al inicializar el mutex: %d\n", status);
        return 1; // Retorna un código de error
    }


    // Destruir el mutex
    status = pthread_mutex_destroy(&lock);
    if (status != 0) {
        printf("Error al destruir el mutex: %d\n", status);
        return 1; // Retorna un código de error
    }

    printf("Mutex destruido correctamente\n");

    return 0;
}
