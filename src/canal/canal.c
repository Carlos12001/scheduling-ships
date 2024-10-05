#include "canal.h"

canal Canal;
waitline left_sea;
waitline right_sea;

pthread_mutex_t canal_mutex = PTHREAD_MUTEX_INITIALIZER;

boat emptyboat = {-1,-1,-1,-1};

void canal_tryout(){
    Canal_init("canal/canal.config");
    BoatGUI();
    destroy_canal();

}

void Canal_init(const char *nombre_archivo) {
    Canal.managed_boats=0;
    Canal.running=true;


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
        line[strcspn(line,"\n")] = 0;
        // Separa la clave y el valor
        if (sscanf(line, "%127[^=]=%127s", clave, valor) == 2) {
            if(strcmp(clave, "length")==0){
                Canal.size=atoi(valor);
                create_canal();
            }else if (strcmp(clave,"metodo")==0){
                Canal.scheduling=atoi(valor);
            }else if (strcmp(clave,"W")==0){
                Canal.W=atoi(valor);
            }else if (strcmp(clave,"tiempo")==0){
                Canal.time=atoi(valor);
            }else if (strcmp(clave,"velocidad")==0){
                Canal.boatspeeds[0]=atoi(&valor[0]);
                Canal.boatspeeds[1]=atoi(&valor[2]);
                Canal.boatspeeds[2]=atoi(&valor[4]);
            }else if (strcmp(clave,"left")==0){
                left_sea.capacity=0;
                left_sea.maxcapacity=5;
                waitline_init(false,valor);
            }else if (strcmp(clave,"right")==0){
                right_sea.capacity=0;
                right_sea.maxcapacity=5;
                waitline_init(true,valor);
            }
        }
    }
    // Cierra el archivo
    fclose(file);

}

void create_canal(){
    Canal.canal = malloc(Canal.size * sizeof(boat));
    Canal.boats_in=0;
    if (Canal.canal == NULL) {
        perror("Error al asignar memoria");
        return;
    }

    // Inicializar el arreglo
    for (int i = 0; i < Canal.size; i++) {
        Canal.canal[i]=emptyboat;
    }

    canalcontent();
}

void destroy_canal(){
    free(Canal.canal);
}

void waitline_init(bool right,char *list){
    
    for(int i = 0; i<3;i++){
        for (int j = 0; j < atoi(&list[i*2]); j++){
            addboatdummy(right,i+1);
            continue;
            if(right){
                //Aqui creo el hilo y uso vector_add(&mar_derecho,newthread);
                ;
            }else{
                //Aqui creo el hilo y uso vector_add(&mar_izquierdo,newthread);
                ;                
            }
        }
    }
    
}

void addboatdummy(bool right,int type){
    pthread_t newthread;
    boat newBoat={newthread,Canal.managed_boats++,-1,Canal.boatspeeds[type-1],type};
    if (right){
        if(right_sea.capacity==right_sea.maxcapacity){
            printf("No se puede agregar en right\n");
        }else{
            right_sea.waiting[right_sea.capacity++]=newBoat;
        }
    }else{
        if(left_sea.capacity==left_sea.maxcapacity){
            printf("No se puede agregar en left\n");
        }else{
            left_sea.waiting[left_sea.capacity++]=newBoat;
        }
    }
    canalcontent();
}

void* boatmover(void *arg){
    boat boat2move = Canal.canal[0];
    float delay=(1.0/boat2move.speed)*1e6;
    while (1){
        //La velocidad se refleja en el delay
        usleep(delay);

        if(boat2move.position==Canal.size-1){//El barco debe salir
            pthread_mutex_lock(&canal_mutex);
            Canal.canal[Canal.size-1]=emptyboat;
            canalcontent();
            Canal.boats_in--;
            pthread_mutex_unlock(&canal_mutex);
            break;
        }else if(Canal.canal[boat2move.position+1].ID!=-1){//Hay un barco ocupando el lugar
            continue;
        }else{//el barco se mueve
            pthread_mutex_lock(&canal_mutex);
            Canal.canal[boat2move.position]=emptyboat;
            Canal.canal[++boat2move.position]=boat2move;
            canalcontent();
            pthread_mutex_unlock(&canal_mutex);            
        }
    }

            
    return NULL;
}

void canalcontent(){
    
    FILE *archivo = fopen("Canal.txt", "w");

    // Verifica si el archivo se abrió correctamente
    if (archivo == NULL) {
        perror("Error al abrir el archivo");
    }

    // Imprimir el contenido del arreglo    
    for (int i = 0; i < Canal.size; i++) {
        fprintf(archivo,"%d ", Canal.canal[i].ID);
    }
    fprintf(archivo,"\nLeft:[");
    for (int i = 0; i < left_sea.capacity; i++) {
        fprintf(archivo,"%d ", left_sea.waiting[i].ID);
    }
    fprintf(archivo,"]\nRight:[");
    for (int i = 0; i < right_sea.capacity; i++) {
        fprintf(archivo,"%d ", right_sea.waiting[i].ID);
    }
    fprintf(archivo,"]");

    
    // Cierra el archivo
    fclose(archivo);

}

void BoatGUI(){
    char respuesta[100];   // Buffer para almacenar el nombre
    int boattype=1;
    const char *boatstrings[] = {
        "Normal",
        "Pesquero",
        "Patrulla"
    };
    char miString[3];    


    printf("Bienvenio a la GUI de creacion de barcos presione cualquier tecla para comenzar");
    scanf("%99s", respuesta); // Evitar desbordamiento
    printf("Comandos:\n    exit:salir del programa\n    r:Agregar barco a la derecha\n    l:Agregar barco a la izquierda\n    n: Barcos normales\n    f: Barcos pesqueros \n    p: Barcos patrulla\n");
    
    
    pthread_t Canal_thread; // Identificador del hilo

    if (pthread_create(&Canal_thread, NULL,(void*) &Canal_Schedule, (void *) NULL) != 0) {
        fprintf(stderr, "Error al crear el hilo\n");
        return;
    }

    while (Canal.running) {
        printf("Escriba su comando:\n");
        printf("Barco por agregar %s\n", boatstrings[boattype-1]);
        fflush(stdout); 

        // Imprimir un mensaje en la terminal y leer el nombre
        scanf("%99s", respuesta); // Evitar desbordamiento

        if (strcmp(respuesta, "exit") == 0) {
            Canal.running=false;
        }else if (strcmp(respuesta, "n") == 0) {
            boattype=1; //Cambiar a barco normal
        }else if (strcmp(respuesta, "f") == 0) {
            boattype=2; //Cambiar a barco pesquero
        }else if (strcmp(respuesta, "p") == 0) {
            boattype=3; //Cambiar a barco patrulla
        }else if (strcmp(respuesta, "r") == 0) {
            addboatdummy(true,boattype);//Agregar barco a la derecha
        }else if (strcmp(respuesta, "l") == 0) {
            addboatdummy(false,boattype);//Agregar barco a la izquierda
        }

    }

    if (pthread_join(Canal_thread, NULL) != 0) {
        fprintf(stderr, "Error al esperar el hilo del Canal\n");
        return;
    }

    printf("Saliendo del programa. ¡Adiós!\n");
}

void * Canal_Schedule(void *arg){
    time_t start_time, current_time;
    start_time = time(NULL);
    int w =0;
    while (Canal.running){
        if(Canal.scheduling==1){//W
            if(w==Canal.W){
                YellowCanal();//Esperar a que los barcos crucen
                w=0;
                Canal.direction=!Canal.direction;
            }else{//No se han cumplido los tiempos
                w+=EnterCanal();
            }
        }else if(Canal.scheduling==2){//Time
            current_time = time(NULL);

            // Comprobar si han pasado 3 segundos
            if (difftime(current_time, start_time) >= (float) Canal.time) {
                YellowCanal();//Esperar a que el canal se vacie
                Canal.direction=!Canal.direction;
                start_time = current_time;                
            }else{
                EnterCanal();
            }
        }else if(Canal.scheduling==3){//Modo tico
            if(Canal.direction){
                if(right_sea.capacity==0){
                    YellowCanal();
                    Canal.direction=!Canal.direction;
                }else{
                    EnterCanal();
                }
            }else{
                if(left_sea.capacity==0){
                    YellowCanal();
                    Canal.direction=!Canal.direction;
                }else{
                    EnterCanal();
                }
            }
        }else{
            printf("Error en la seleccion de scheduler");
            Canal.running=false;
        }
        usleep(1000);

    }
    

}

void YellowCanal(){
    Canal.Yellowlight=true;
    while (Canal.Yellowlight){
        if(Canal.boats_in==0){
            Canal.Yellowlight=false;
        }
    }
}

int EnterCanal(){
    if(Canal.canal[0].ID!=-1){//La posicion incial esta ocupada
        return 0;
    }else if(Canal.direction && right_sea.capacity==0){
        return 0;
    }else if(!Canal.direction && left_sea.capacity==0){
        return 0;
    }else{
        boat newboat=GetEnterBoat();
        newboat.position= 0;
        Canal.canal[0]=newboat;
        Canal.boats_in++;
        canalcontent();
        if (pthread_create(&Canal.canal[newboat.position].thread, NULL,(void*) &boatmover, (void*)NULL) != 0) {
        fprintf(stderr, "Error al crear el hilo\n");
        return 0;
        }
        return 1;

    }    
}

boat GetEnterBoat(){
    if(Canal.direction){
        boat newBoat=right_sea.waiting[0];
        for(int i=1;i<right_sea.capacity;i++){
            right_sea.waiting[i-1]=right_sea.waiting[i];//Corrimiento de los barcos en espera
        }
        right_sea.capacity--;
        right_sea.waiting[right_sea.capacity]=emptyboat;//Borrado del ultimo barco
        return newBoat;
    }else{
        boat newBoat=left_sea.waiting[0];
        for(int i=1;i<left_sea.capacity;i++){
            left_sea.waiting[i-1]=left_sea.waiting[i];//Corrimiento de los barcos en espera
        }
        left_sea.capacity--;
        left_sea.waiting[left_sea.capacity]=emptyboat;//Borrado del ultimo barco        
        return newBoat;
    }
}
