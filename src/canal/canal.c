#include "canal.h"

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/socket.h>

int server_fd = -1;
int client_fd = -1;
int client_fd2 = -1;
static cemutex socket_mutex;
static size_t message_id = 0;

canal Canal;
waitline left_sea;
waitline right_sea;

cemutex canal_mutex;

boat emptyboat = {{-1, NULL, NULL}, -1, -1, -1, -1, -1, -1, -1};

void canal_tryout() {
  if (cemutex_init(&canal_mutex) != 0) {
    perror("Error to initialize canal_mutex");
    return;
  }
  start_server();
  Canal_init("canal/canal.config");
  canalcontent();
  BoatGUI();
  destroy_canal();
}

void init_server_socket() {
  struct sockaddr_in address;
  int opt = 1;
  int addrlen = sizeof(address);

  // create a socket object
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("Error to create socket");
    exit(EXIT_FAILURE);
  }

  // set socket option SO_REUSEADDR and SO_REUSEPORT
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                 sizeof(opt))) {
    perror("Error in setsockopt");
    exit(EXIT_FAILURE);
  }

  // set server address and port
  address.sin_family = AF_INET;
  address.sin_addr.s_addr =
      INADDR_ANY;  //  accept connections from any interface
  address.sin_port = htons(SOCKET_PORT);

  // link server_fd with address
  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("Error on bind");
    exit(EXIT_FAILURE);
  }

  // listening for entries connections
  if (listen(server_fd, 3) < 0) {
    perror("Error on listen");
    exit(EXIT_FAILURE);
  }
  const int port = SOCKET_PORT;
  printf("Server listening on port %d, waiting for connections...\n", port);
}

void *accept_connections(void *arg) {
  struct sockaddr_in address;
  int addrlen = sizeof(address);

  while (1) {
    // accept the incoming connection
    int new_client_fd =
        accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
    if (new_client_fd < 0) {
      perror("Error in accept");
      continue;
    }

    printf("Client connected with fd %d\n", new_client_fd);

    // protect access to client_fd and client_fd2 with the mutex
    cemutex_lock(&socket_mutex);

    if (client_fd == -1) {
      client_fd = new_client_fd;
      printf("Assigned to client_fd\n");
    } else if (client_fd2 == -1) {
      client_fd2 = new_client_fd;
      printf("Assigned to client_fd2\n");
    } else {
      // The server only accepts 2 clients at a time
      printf("Server is full. Closing new connection fd %d\n", new_client_fd);
      close(new_client_fd);
    }

    cemutex_unlock(&socket_mutex);
  }
  return NULL;
}

void start_server() {
  if (cemutex_init(&socket_mutex) != 0) {
    perror("Error to initialize socket_mutex");
    return;
  }
  // init_server_socket();
  init_server_socket();

  // ignore SIGPIPE
  signal(SIGPIPE, SIG_IGN);

  // create the accept thread
  cethread_t accept_thread;
  if (cethread_create(&accept_thread, accept_connections, NULL) != 0) {
    fprintf(stderr, "Error to create accept_thread\n");
    exit(EXIT_FAILURE);
  }
}

int send_data() {
  // ------------------- Send message to GUI ---------------------
  char buffer[BUFFER_SIZE];
  int offset = 0;
  int n;
  // build the content to send
  offset += snprintf(buffer + offset, sizeof(buffer) - offset, "Canal: ");
  for (int i = 0; i < Canal.size; i++) {
    boat boatprinter = Canal.canal[i];
    offset += snprintf(buffer + offset, sizeof(buffer) - offset, "%d ",
                       boatprinter.typeboat);
  }
  offset +=
      snprintf(buffer + offset, sizeof(buffer) - offset, "\nDirection: %s",
               (Canal.direction) ? ("Right") : ("Left"));
  offset +=
      snprintf(buffer + offset, sizeof(buffer) - offset, "\nTiempoReal: %s",
               (Canal.TiempoReal) ? ("true") : ("false"));
  offset +=
      snprintf(buffer + offset, sizeof(buffer) - offset, "\nYellow Light: %s",
               (Canal.Yellowlight) ? ("true") : ("false"));
  offset += snprintf(buffer + offset, sizeof(buffer) - offset, "\nLeft:[");
  for (int i = 0; i < left_sea.capacity; i++) {
    boat boatprinter = left_sea.waiting[i];
    offset += snprintf(buffer + offset, sizeof(buffer) - offset, "%d ",
                       boatprinter.typeboat);
  }
  offset += snprintf(buffer + offset, sizeof(buffer) - offset, "]");
  offset += snprintf(buffer + offset, sizeof(buffer) - offset, "\nRight:[");
  for (int i = 0; i < right_sea.capacity; i++) {
    boat boatprinter = right_sea.waiting[i];
    offset += snprintf(buffer + offset, sizeof(buffer) - offset, "%d ",
                       boatprinter.typeboat);
  }
  offset += snprintf(buffer + offset, sizeof(buffer) - offset, "]\n");

  // send the message
  cemutex_lock(&socket_mutex);

  if (client_fd > 0) {
    // Set socket to non-blocking mode
    int flags = fcntl(client_fd, F_GETFL, 0);
    fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);

    message_id++;
    offset += snprintf(buffer + offset, sizeof(buffer) - offset,
                       "Message ID: %ld\nEND_OF_MESSAGE\n", message_id);
    n = send(client_fd, buffer, strlen(buffer), 0);

    if (n < 0) {
      if (errno == EPIPE || errno == ECONNRESET || errno == EAGAIN ||
          errno == EWOULDBLOCK) {
        perror("Error sending message to client_fd");
        // Close the socket and clean up
        close(client_fd);
        client_fd = -1;
        message_id--;
      } else {
        perror("Unexpected error in send() to client_fd");
      }
    }
  }

  if (client_fd2 > 0) {
    // Set socket to non-blocking mode
    int flags = fcntl(client_fd2, F_GETFL, 0);
    fcntl(client_fd2, F_SETFL, flags | O_NONBLOCK);

    message_id++;
    n = send(client_fd2, buffer, strlen(buffer), 0);

    if (n < 0) {
      if (errno == EPIPE || errno == ECONNRESET || errno == EAGAIN ||
          errno == EWOULDBLOCK) {
        perror("Error sending message to client_fd2");
        // Close the socket and clean up
        close(client_fd2);
        client_fd2 = -1;
        message_id--;
      } else {
        perror("Unexpected error in send() to client_fd2");
      }
    }
  }

  cemutex_unlock(&socket_mutex);
  return 0;
}

void Canal_init(const char *nombre_archivo) {
  Canal.managed_boats = 0;
  Canal.running = true;
  Canal.direction = false;
  Canal.TiempoReal = true;
  Canal.RRiter = 0;
  Canal.RRID = -2;

  FILE *file;
  char line[MAX_LINE_LENGTH];
  char clave[128];
  char valor[128];

  // Abre el archivo en modo lectura
  file = fopen(nombre_archivo, "r");
  if (file == NULL) {
    perror("Error al abrir el archivo");
    return;
  }

  // Lee el archivo línea por línea
  while (fgets(line, sizeof(line), file)) {
    // Elimina el salto de línea al final
    line[strcspn(line, "\n")] = 0;
    // Separa la clave y el valor
    if (sscanf(line, "%127[^=]=%127s", clave, valor) == 2) {
      if (strcmp(clave, "length") == 0) {
        Canal.size = atoi(valor);
        create_canal();
      } else if (strcmp(clave, "c_schedule") == 0) {
        Canal.canal_scheduling = atoi(valor);
      } else if (strcmp(clave, "W") == 0) {
        Canal.W = atoi(valor);
      } else if (strcmp(clave, "time") == 0) {
        Canal.time = atoi(valor);
      } else if (strcmp(clave, "speed") == 0) {
        Canal.boatspeeds[0] = atoi(&valor[0]);
        Canal.boatspeeds[1] = atoi(&valor[2]);
        Canal.boatspeeds[2] = atoi(&valor[4]);
      } else if (strcmp(clave, "left") == 0) {
        left_sea.capacity = 0;
        waitline_init(false, valor);
        Canal.TiempoReal = calendar(Canal.thread_scheduling, left_sea.waiting,
                                    left_sea.capacity, emptyboat);
      } else if (strcmp(clave, "right") == 0) {
        right_sea.capacity = 0;
        waitline_init(true, valor);
        Canal.TiempoReal = calendar(Canal.thread_scheduling, right_sea.waiting,
                                    right_sea.capacity, emptyboat);
      } else if (strcmp(clave, "queuelength") == 0) {
        right_sea.maxcapacity = atoi(valor);
        left_sea.maxcapacity = atoi(valor);
        waitline_create();
      } else if (strcmp(clave, "t_schedule") == 0) {
        Canal.thread_scheduling = atoi(valor);
      }
    }
  }
  // Cierra el archivo
  fclose(file);
}

void create_canal() {
  Canal.canal = malloc(Canal.size * sizeof(boat));
  Canal.boats_in = 0;
  if (Canal.canal == NULL) {
    perror("Error al asignar memoria");
    return;
  }

  // Inicializar el arreglo
  for (int i = 0; i < Canal.size; i++) {
    Canal.canal[i] = emptyboat;
  }
}

void destroy_canal() {
  free(right_sea.waiting);
  free(left_sea.waiting);
  free(Canal.canal);
}

void waitline_init(bool right, char *list) {
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < atoi(&list[i * 2]); j++) {
      addboatdummy(right, i + 1);
    }
  }
}

void waitline_create() {
  right_sea.waiting = malloc(right_sea.maxcapacity * sizeof(boat));
  left_sea.waiting = malloc(left_sea.maxcapacity * sizeof(boat));
}

void addboatdummy(bool right, int type) {
  cethread_t newthread;
  boat newBoat = {newthread,
                  Canal.managed_boats++,
                  -1,
                  Canal.boatspeeds[type - 1],
                  (1.0 / Canal.boatspeeds[type - 1]) * Canal.size,
                  (1.0 / Canal.boatspeeds[type - 1]) * Canal.size,
                  type,
                  (Canal.thread_scheduling == 4) ? (false) : (true)};

  if (right) {
    if (right_sea.capacity == right_sea.maxcapacity) {
      printf("No se puede agregar en right\n");
    } else {
      right_sea.waiting[right_sea.capacity++] = newBoat;
    }
  } else {
    if (left_sea.capacity == left_sea.maxcapacity) {
      printf("No se puede agregar en left\n");
    } else {
      left_sea.waiting[left_sea.capacity++] = newBoat;
    }
  }
  canalcontent();
}

void *boatmover(void *arg) {
  int adder = (Canal.direction) ? (1) : (-1);
  int position = (Canal.direction) ? (0) : (Canal.size - 1);
  boat boat2move = Canal.canal[position];
  float delay = (1.0 / boat2move.speed) * 1e6;

  while (Canal.running) {
    // La velocidad se refleja en el delay
    usleep(delay);
    if ((position == Canal.size - 1) &&
        (Canal.direction)) {  // El barco debe salir por la derecha
      cemutex_lock(&canal_mutex);
      Canal.canal[Canal.size - 1] = emptyboat;
      Canal.boats_in--;
      cemutex_unlock(&canal_mutex);
      canalcontent();
      break;
    } else if ((position == 0) &&
               (!Canal.direction)) {  // El barco debe salir por la izquierda
      cemutex_lock(&canal_mutex);
      Canal.canal[0] = emptyboat;
      Canal.boats_in--;
      cemutex_unlock(&canal_mutex);
      canalcontent();
      break;
    } else if (Canal.canal[position + 1].ID != -1 &&
               Canal.direction) {  // Hay un barco ocupando el lugar
      if (Canal.thread_scheduling ==
          5) {  // Si se esta en tiempo real no cumple con el deadline
        Canal.TiempoReal = false;
      }
      continue;
    } else if (Canal.canal[position - 1].ID != -1 &&
               (!Canal.direction)) {  // Hay un barco ocupando el lugar
      if (Canal.thread_scheduling ==
          5) {  // Si se esta en tiempo real no cumple con el deadline
        Canal.TiempoReal = false;
      }
      continue;
    } else {  // el barco se mueve
      boat2move = Canal.canal[position];
      if (boat2move.Permission) {
        cemutex_lock(&canal_mutex);
        Canal.canal[position] = emptyboat;
        position += adder;
        boat2move.position = position;
        boat2move.tiempo_restante -= delay / 1e6;
        Canal.canal[position] = boat2move;
        canalcontent();
        cemutex_unlock(&canal_mutex);
      } else {
        continue;
      }
    }
  }

  return NULL;
}

void canalcontent() {
  FILE *archivo = fopen("Canal.txt", "w");

  // Verifica si el archivo se abrió correctamente
  if (archivo == NULL) {
    perror("Error al abrir el archivo");
  }

  // Imprimir el contenido del arreglo

  for (int i = 0; i < Canal.size; i++) {
    boat boatprinter = Canal.canal[i];
    // fprintf(archivo,"[Id=%d,Speed=%d,Type=%d]
    // ",boatprinter.ID,boatprinter.speed,boatprinter.typeboat);
    fprintf(archivo, "%d ", boatprinter.typeboat);
  }
  fprintf(archivo, "\nDirection: %s", (Canal.direction) ? ("Right") : ("Left"));
  fprintf(archivo, "\nTiempoReal: %s",
          (Canal.TiempoReal) ? ("true") : ("false"));
  fprintf(archivo, "\nYellow Light: %s",
          (Canal.Yellowlight) ? ("true") : ("false"));
  fprintf(archivo, "\nLeft:[");
  for (int i = 0; i < left_sea.capacity; i++) {
    boat boatprinter = left_sea.waiting[i];
    // fprintf(archivo,"[Id=%d,Speed=%d,Type=%d]
    // ",boatprinter.ID,boatprinter.speed,boatprinter.typeboat);
    fprintf(archivo, "%d ", boatprinter.typeboat);
  }
  fprintf(archivo, "]\nRight:[");
  for (int i = 0; i < right_sea.capacity; i++) {
    boat boatprinter = right_sea.waiting[i];
    // fprintf(archivo,"[Id=%d,Speed=%d,Type=%d]
    // ",boatprinter.ID,boatprinter.speed,boatprinter.typeboat);
    fprintf(archivo, "%d ", boatprinter.typeboat);
  }
  fprintf(archivo, "]");

  // Cierra el archivo
  fclose(archivo);
  send_data();
}

void BoatGUI() {
  char respuesta[100];  // Buffer para almacenar el nombre
  int boattype = 1;
  const char *boatstrings[] = {"Normal", "Pesquero", "Patrulla"};
  char miString[3];

  // GUI Mensaje inicial
  printf(
      "Bienvenio a la GUI de creacion de barcos presione cualquier tecla para "
      "comenzar");
  printf(
      "\nComandos:\n    exit:salir del programa\n    r:Agregar barco a la "
      "derecha\n    l:Agregar barco a la izquierda\n    n: Barcos normales\n   "
      " f: Barcos pesqueros \n    p: Barcos patrulla\n");
  scanf("%99s", respuesta);  // Evitar desbordamiento

  // Hilo para el manejo del canal
  cethread_t Canal_thread;  // Identificador del hilo
  if (cethread_create(&Canal_thread, (void *)&Canal_Schedule, (void *)NULL) !=
      0) {
    fprintf(stderr, "Error al crear el hilo\n");
    return;
  }

  while (Canal.running) {
    printf("Escriba su comando:\n");
    printf("Barco por agregar %s\n", boatstrings[boattype - 1]);

    // Imprimir un mensaje en la terminal y leer el nombre
    scanf("%99s", respuesta);  // Evitar desbordamiento

    if (strcmp(respuesta, "exit") == 0) {
      Canal.running = false;
    } else if (strcmp(respuesta, "n") == 0) {
      boattype = 1;  // Cambiar a barco normal
    } else if (strcmp(respuesta, "f") == 0) {
      boattype = 2;  // Cambiar a barco pesquero
    } else if (strcmp(respuesta, "p") == 0) {
      boattype = 3;  // Cambiar a barco patrulla
    } else if (strcmp(respuesta, "r") == 0) {
      cemutex_lock(&canal_mutex);
      addboatdummy(true, boattype);  // Agregar barco a la derecha
      Canal.TiempoReal = calendar(Canal.thread_scheduling, right_sea.waiting,
                                  right_sea.capacity, GetSlowestBoat());
      cemutex_unlock(&canal_mutex);
    } else if (strcmp(respuesta, "l") == 0) {
      cemutex_lock(&canal_mutex);
      addboatdummy(false, boattype);  // Agregar barco a la izquierda
      Canal.TiempoReal = calendar(Canal.thread_scheduling, left_sea.waiting,
                                  left_sea.capacity, GetSlowestBoat());
      cemutex_unlock(&canal_mutex);
    }
  }

  // Esperar finalizacion del manejo del canal
  if (cethread_join(Canal_thread, NULL) != 0) {
    fprintf(stderr, "Error al esperar el hilo del Canal\n");
    return;
  }

  printf("Saliendo del programa. ¡Adiós!\n");
}

void *Canal_Schedule(void *arg) {
  // Variables de control actuales
  int w = 0;
  int timer = 0;

  // Variables de tiempo
  time_t start_time, current_time;
  start_time = time(NULL);

  while (Canal.running) {
    if (Canal.thread_scheduling == 4) {
      Canal.RRiter++;
      if (QUANTUM_mSEC < Canal.RRiter) {
        Canal.RRiter = 0;
        Canal_RR();
        calendar(Canal.thread_scheduling, right_sea.waiting, right_sea.capacity,
                 emptyboat);
        calendar(Canal.thread_scheduling, left_sea.waiting, left_sea.capacity,
                 emptyboat);
      }
    }
    if (Canal.canal_scheduling == 1) {  // W
      if (w == Canal.W) {
        YellowCanal();  // Esperar a que los barcos crucen
        w = 0;
        Canal.direction = !Canal.direction;
      } else {  // No se han cumplido los tiempos
        CheckRealTime();
        w += EnterCanal(0, !Canal.direction);
      }
    } else if (Canal.canal_scheduling == 2) {  // Time
      current_time = time(NULL);
      if (difftime(current_time, start_time) >= (float)Canal.time) {
        YellowCanal();  // Esperar a que el canal se vacie
        Canal.direction = !Canal.direction;
        start_time = time(NULL);
      } else {
        CheckRealTime();
        EnterCanal(0, !Canal.direction);
      }
    } else if (Canal.canal_scheduling == 3) {  // Modo tico
      if (Canal.direction) {
        if (left_sea.capacity == 0) {
          YellowCanal();
          Canal.direction = !Canal.direction;
        } else {
          CheckRealTime();
          EnterCanal(0, !Canal.direction);
        }
      } else {
        if (right_sea.capacity == 0) {
          YellowCanal();
          Canal.direction = !Canal.direction;
        } else {
          CheckRealTime();
          EnterCanal(0, !Canal.direction);
        }
      }
    } else {
      printf("Error en la seleccion de scheduler\n");
      Canal.running = false;
    }
    usleep(1000);  // Revision cada mili segundo
  }
}

void YellowCanal() {
  Canal.Yellowlight = true;
  while (Canal.Yellowlight) {
    if (Canal.boats_in == 0) {
      Canal.Yellowlight = false;
    }
    if (Canal.thread_scheduling == 4) {
      Canal.RRiter++;
      if (QUANTUM_mSEC < Canal.RRiter) {
        Canal.RRiter = 0;
        Canal_RR();
        calendar(Canal.thread_scheduling, right_sea.waiting, right_sea.capacity,
                 emptyboat);
        calendar(Canal.thread_scheduling, left_sea.waiting, left_sea.capacity,
                 emptyboat);
      }
    }
    CheckRealTime();

    usleep(1000);
  }
}

int EnterCanal(int Waitpos, bool queue) {
  int newposition = (Canal.direction) ? (0) : (Canal.size - 1);
  if (Canal.canal[newposition].ID != -1) {  // La posicion incial esta ocupada
    return 0;
  } else if (!Canal.direction && right_sea.capacity == 0) {
    return 0;
  } else if (Canal.direction && left_sea.capacity == 0) {
    return 0;
  } else {
    cemutex_lock(&canal_mutex);
    boat newboat = GetEnterBoat(Waitpos, queue);
    newboat.position = newposition;
    Canal.canal[newboat.position] = newboat;
    Canal.boats_in++;
    canalcontent();
    cemutex_unlock(&canal_mutex);
    if (cethread_create(&Canal.canal[newboat.position].thread,
                        (void *)&boatmover, (void *)NULL) != 0) {
      fprintf(stderr, "Error al crear el hilo\n");
      return 0;
    }
    return 1;
  }
}

boat GetEnterBoat(int index, bool queue) {
  if (queue) {
    boat newBoat = right_sea.waiting[index];
    for (int i = index + 1; i < right_sea.capacity; i++) {
      right_sea.waiting[i - 1] =
          right_sea.waiting[i];  // Corrimiento de los barcos en espera
    }
    right_sea.capacity--;
    right_sea.waiting[right_sea.capacity] =
        emptyboat;  // Borrado del ultimo barco
    return newBoat;
  } else {
    boat newBoat = left_sea.waiting[index];
    for (int i = index + 1; i < left_sea.capacity; i++) {
      left_sea.waiting[i - 1] =
          left_sea.waiting[i];  // Corrimiento de los barcos en espera
    }
    left_sea.capacity--;
    left_sea.waiting[left_sea.capacity] = emptyboat;  // Borrado del ultimo
                                                      // barco
    return newBoat;
  }
}

boat GetSlowestBoat() {
  boat Boatiter, SlowestBoat;
  SlowestBoat = emptyboat;

  for (int i = 0; i < Canal.size; i++) {
    Boatiter = Canal.canal[i];
    if (Boatiter.ID == -1) {  // No tomo en cuanta los espacios vacios
      continue;
    }
    if (SlowestBoat.tiempo_restante < Boatiter.tiempo_restante) {
      SlowestBoat = Boatiter;
    }
  }
  // printf("Slowboat: %f\n", SlowestBoat.tiempo_restante);

  return SlowestBoat;
}

void Canal_RR() {
  cemutex_lock(&canal_mutex);
  int firstboat = -1;
  int Quantumended = Canal.size;  // Me aseguro de minimo recorrer todo el canal
  for (int i = 0; i < Canal.size; i++) {
    if (Canal.canal[i].ID == -1) {  // No se toman en cuenta barcos nulos
      continue;
    }
    if (firstboat == -1) {  // Se toma nota del primer barco por si acaso se dio
                            // vuelta al canal de permisos
      firstboat = i;
    }
    if (Canal.canal[i].ID ==
        Canal.RRID) {  // Es el barco que se le acabo el quantum
      // printf("Quantum ended for %d in position %d\n", Canal.RRID, i);
      Canal.canal[i].Permission = false;
      Quantumended = i;
    }
    if (Quantumended < i) {  // Es el proximo en tener el quantum
      Canal.canal[i].Permission = true;
      Canal.RRID = Canal.canal[i].ID;
      // printf("Quantum granted to %d in position %d\n", Canal.RRID, i);
      cemutex_unlock(&canal_mutex);
      return;
    }
  }
  if (firstboat == -1) {  // No boats in the canal
    Canal.RRID = -2;
    cemutex_unlock(&canal_mutex);
    return;
  }
  Canal.canal[firstboat].Permission = true;
  Canal.RRID = Canal.canal[firstboat].ID;
  // printf("Quantum granted to %d in position %d\n", Canal.RRID, firstboat);
  cemutex_unlock(&canal_mutex);
}

void CheckRealTime() {
  if (Canal.thread_scheduling ==
      5) {  // Para tiempo real hay que verificar que no tenga barcos esperando
            // por direccion
    if (Canal.Yellowlight) {
      if (right_sea.capacity != 0 || left_sea.capacity != 0) {
        Canal.TiempoReal = false;
      }
    }
    if (Canal.direction) {  //->
      if (right_sea.capacity !=
          0) {  // No cumple, ay barcos esperando por cambio de direccion
        Canal.TiempoReal = false;
      }
    } else {  //<-
      if (left_sea.capacity !=
          0) {  // No cumple, ay barcos esperando por cambio de direccion
        Canal.TiempoReal = false;
      }
    }
  }
}