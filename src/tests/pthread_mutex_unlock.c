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

    // Bloquear el mutex
    status = pthread_mutex_lock(&lock);
    if (status != 0) {
        printf("Error al bloquear el mutex: %d\n", status);
        pthread_mutex_destroy(&lock);
        return 1; // Retorna un código de error
    }

    printf("Mutex bloqueado, ahora será liberado.\n");

    // Liberar el mutex
    status = pthread_mutex_unlock(&lock);
    if (status != 0) {
        printf("Error al liberar el mutex: %d\n", status);
        pthread_mutex_destroy(&lock);
        return 1; // Retorna un código de error
    }

    printf("Mutex liberado correctamente\n");

    // Destruir el mutex
    pthread_mutex_destroy(&lock);

    return 0;
}
