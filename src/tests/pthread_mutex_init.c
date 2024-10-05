#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    pthread_mutex_t lock;
    int status;

    // Inicializar el mutex
    status = pthread_mutex_init(&lock, NULL);
    if (status != 0) {
        printf("Error al inicializar el mutex: %d\n", status);
        return 1; // Retorna un código de error
    }

    printf("Mutex inicializado correctamente\n");


    // Se destruye el mutex al finalizar el código
    pthread_mutex_destroy(&lock);

    return 0;
}
