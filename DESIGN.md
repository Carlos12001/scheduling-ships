# Idea del proyecto

- Hacer una biblioteca 'CEThreads', inspirada en PThreads (POSIX Threads) de C. Esta es una biblioteca estándar para C que permite la programación multihilo en sistemas operativos compartible con POSIX (Unix, Linux, MacOS). Proporciona un conjunto de funciones para crear y manipular hilos, lo que permite ejecutar múltiples operaciones en paralelo dentro de un mismo proceso. 
	* ```pthread_create```: Se utiliza para crear un nuevo hilo. Requiere varios parámetros, incluyendo un puntero a la fnción que el hilo ejecutará, y, opcionalmente, argumentos que se pasarán a esa función. Si la creación del hilo es exitosa, ```pthread_create``` retorna 0.
	* ```pthread_exit```: Esta función termina el hilo que la llama. Permite especificar un valor de 'retorno' que puede ser recogido por otros hilos que se sincronicen con éste. Es importante usar esta función para terminar correctamente un hilo y liberar recursos adecuadamente. 
	* ```pthread_join```: Se utiliza para bloquear el hilo que llama hasta que otro específico termine. Esto es útil para esperar a que un hilo complete su tarea antes de continuar con la ejecución del hilo principal o de otro hilo.
	* ```pthread_mutex_init```: Inicializa un mutex, que es un mecanismo de sincronización utilizado para gestionar el acceso a recursos compartidos. Un mutex puede ser utilizado para evitar que múltiples hilos accedan simultáneamente a un recurso, como una variable compartida, asegurando así que los datos no se corrompan. 
	* ```pthread_mutex_destroy```: Destruye un mutex que ha sido incializado anteriormente con ```pthread_mutex_init```. Esta función se usa para liberar cualquier recurso asociado con el mutex. Es importante destruir los mutexes que ya no se necesiten para evitar fugas de recursos.
	* ```pthread_mutex_unlock```: Libera un mutex que ha sido previamente bloqueado por el hilo que lo llama, utilizando ```pthread_mutex_lock```. Liberar el mutex permite que otros hilos que están bloqueados intentando obtener el mismo mutex puedan avanar y bloquear dicho mutex para su uso.


Además, se deberán de incorporar los siguientes algoritmos calendarizadores:

- **Round Robin**: Es un algoritmo de planificación muy común que asigna un tiempo fijo, conocido como quantum, a cada proceso en la cola de listos (ready queue). Todos los procesos se almacenan en una cola circular y cada proceso recibe el CPU por un intervalo de tiempo determinado. Si un proceso no termina en su tiempo asignado, se coloca al final de la cola y el siguiente proceso recibe el CPU.
- **Prioridad**: En la planificación por prioridad, cada proceso se asigna una prioridad. El proceso con la prioridad más alta se ejecuta primero y así sucesivamente. Si dos procesos tienen la misma prioridad, se pueden planificar según FCFS o Round Robin. Las prioridades pueden ser estáticas (asignadas al inicio y no cambian) o dinámicas (pueden cambiar durante la ejecución del proceso).
- **SJF (Shortest Job First)**: Es un algoritmo de planificación que selecciona el proceso que tiene el tiempo de ráfaga más corto (tiempo necesario para completar el proceso). Es un algoritmo no preventivo, lo que significa que una vez que un proceso comienza su ejecución, sigue hasta que termina.
- **FCFS (First Come, First Served)**: Es el algoritmo de planificación más simple, donde el proceso que llega primero es atendido primero. No hay preempción, lo que significa que los procesos se ejecutan en el orden exacto en que llegan a la cola, independientemente de su duración o prioridad.
- **Tiempo real**: Los algoritmos de planificación de tiempo real están diseñados para sistemas donde es crucial cumplir con plazos específicos. Hay dos tipos principales: tiempo real blando, donde es deseable cumplir con los plazos pero no crítico, y tiempo real duro, donde no cumplir con un plazo puede resultar en un fallo crítico del sistema.

*El calendarizador deberá de ser un parámetro.*


