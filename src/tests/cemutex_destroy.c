#include <cethread.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    cemutex lock;
    int status;

    // Inicializar el mutex primero
    status = cemutex_init(&lock);
    if (status != 0) {
        printf("Error al inicializar el mutex: %d\n", status);
        return 1; // Retorna un código de error
    }


    // Destruir el mutex
    status = cemutex_destroy(&lock);
    if (status != 0) {
        printf("Error al destruir el mutex: %d\n", status);
        return 1; // Retorna un código de error
    }

    printf("Mutex destruido correctamente\n");

    return 0;
}
