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

    printf("Mutex inicializado correctamente\n");


    // Se destruye el mutex al finalizar el código
    cemutex_destroy(&lock);

    return 0;
}
