#include "canal.h"

canal Canal;
waitline left_sea;
waitline right_sea;

pthread_mutex_t canal_mutex = PTHREAD_MUTEX_INITIALIZER;

void canal_tryout(){
    Canal_init("canal/canal.config");
    canalcontent();

    pthread_t hilo; // Identificador del hilo
    pthread_t hilo2; // Identificador del hilo

    BoatWrapper w1;
    BoatWrapper w2;

    w1.b=right_sea.waiting[0];
    w2.b=right_sea.waiting[4];
    

    // Crear el hilo
    if (pthread_create(&hilo, NULL,(void*) &boatmover, &w1) != 0) {
        fprintf(stderr, "Error al crear el hilo\n");
        return;
    }
    
    if (pthread_create(&hilo2, NULL,(void*) &boatmover, &w2) != 0) {
        fprintf(stderr, "Error al crear el hilo\n");
        return;
    }

    // Esperar a que los hilos terminen
    if (pthread_join(hilo, NULL) != 0) {
        fprintf(stderr, "Error al esperar el hilo\n");
        return;
    }
    
    if (pthread_join(hilo2, NULL) != 0) {
        fprintf(stderr, "Error al esperar el hilo\n");
        return;
    }
  
    printf("\n");
    fflush(stdout);
    
    destroy_canal();

}

void Canal_init(const char *nombre_archivo) {
    Canal.managed_boats=0;


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

    create_canal();
}

void create_canal(){
    Canal.canal = malloc(Canal.size * sizeof(int));
    if (Canal.canal == NULL) {
        perror("Error al asignar memoria");
        return;
    }

    // Inicializar el arreglo
    for (int i = 0; i < Canal.size; i++) {
        Canal.canal[i]=-1;
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
    boat newBoat={Canal.managed_boats++,-1,Canal.boatspeeds[type-1],type};
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
    
}

void* boatmover(void *arg){
    BoatWrapper* boatWrapper = (BoatWrapper*)arg; // Cast a tipo adecuado
    boat boat2move = boatWrapper->b;
    float delay=(1.0/boat2move.speed)*1e6;
    while (1){
        if (boat2move.position==-1){//Es la primera iteracion del bote
            if(Canal.canal[0]!=-1){//Cuando haga la funcion que comienza los hilos esto ya no es necesario
                ;
            }else{
                pthread_mutex_lock(&canal_mutex);
                Canal.canal[++boat2move.position]=boat2move.ID;
                canalcontent();
                pthread_mutex_unlock(&canal_mutex);
            }
        }else if(boat2move.position==Canal.size-1){//El barco debe salir
            pthread_mutex_lock(&canal_mutex);
            Canal.canal[Canal.size-1]=-1;
            canalcontent();
            pthread_mutex_unlock(&canal_mutex);
            break;

        }else if(Canal.canal[boat2move.position+1]!=-1){//Hay un barco ocupando el lugar
            continue;
        }else{//el barco se mueve
            pthread_mutex_lock(&canal_mutex);
            Canal.canal[boat2move.position]=-1;
            Canal.canal[++boat2move.position]=boat2move.ID;
            canalcontent();
            pthread_mutex_unlock(&canal_mutex);            
        }
        //La velocidad se refleja en el delay
        usleep(delay);

    }

            
    return NULL;
}

void canalcontent(){
    // Imprimir el contenido del arreglo    
    printf("\033[2K\r");
    for (int i = 0; i < Canal.size; i++) {
        printf("%d ", Canal.canal[i]);
    }
    fflush(stdout); 
}

