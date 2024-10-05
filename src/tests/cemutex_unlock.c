#include <cethread.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    cemutex lock;
    int status;

    // Inicializar el mutex
    status = cemutex_init(&lock);
    if (status != 0) {
        printf("Error al inicializar el mutex: %d\n", status);
        return 1; // Retorna un código de error
    }

    // Bloquear el mutex
    status = cemutex_lock(&lock);
    if (status != 0) {
        printf("Error al bloquear el mutex: %d\n", status);
        cemutex_destroy(&lock);
        return 1; // Retorna un código de error
    }

    printf("Mutex bloqueado, ahora será liberado.\n");

    // Liberar el mutex
    status = cemutex_unlock(&lock);
    if (status != 0) {
        printf("Error al liberar el mutex: %d\n", status);
        cemutex_destroy(&lock);
        return 1; // Retorna un código de error
    }

    printf("Mutex liberado correctamente\n");

    // Destruir el mutex
    cemutex_destroy(&lock);

    return 0;
}
