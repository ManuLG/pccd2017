#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h> //Prints, getChar
#include <stdlib.h>
#include <unistd.h> // Fork
#include <pthread.h> //threads
#include <semaphore.h>
#include <string.h> //strcopy
typedef struct mensaje {
      long      mtype;    // Necesario para filtrar el mensaje a recibir
      int prioridad;
      int id_nodo;
    } mensaje;


// Prototipos de las funciones
int sendMsg();
mensaje receiveMsg();
void *procesoReceptor();
void crearVector();
void *hijo (void *args);

int N = 3; // Número de nodos

#define REPLY -1
#define REQUEST 1

// Identificadores
int mi_id = 1;
int mi_prioridad = 0;

// Arrays de nodos a tener en cuenta
int *id_nodos;
int *id_nodos_pend;
int num_pend = 0;

// Variables para serializar
int quiero = 0;
int sc = 0;
int stop = 0;

// Colas de mesajes
int id_cola = 0;
int id_cola_ack = 0;

// semáforos
sem_t sem_paso_hijo;
sem_t sem_paso_padre;

sem_t sem_prot_quiero, sem_prot_sc, sem_prot_stop;

// cantidades
int num_hijos = 0;
int num_hijos_restantes = 0;
int cont = 0; // Contador de hijos atendidos
int first = 1;

int nodo_prioritario = 0;


int main(int argc, char const *argv[]) {

  mi_id = atoi(argv[1]);
  N = atoi(argv[2]);
  mi_prioridad = atoi(argv[3]);
  num_hijos= atoi(argv[4]);

  int id_nodos_main[N-1];
  id_nodos = id_nodos_main;
  int id_nodos_pend_main[N-1];
  id_nodos_pend = id_nodos_pend_main;

  pthread_t hiloR, hilosH[2000];

  id_cola = msgget(ftok("/tmp", 123), 0777 | IPC_CREAT);
  id_cola_ack = msgget(ftok("/tmp", 1234), 0777 | IPC_CREAT);

  printf("ID de la cola de peticiones: %i\n", id_cola);
  printf("ID de la cola de respuestas: %i\n", id_cola_ack);

  // Inicialización de los semáforos
  sem_init(&sem_paso_hijo,0,0);
  sem_init(&sem_paso_padre,0,0);

  sem_init(&sem_prot_quiero, 0, 1);
  sem_init(&sem_prot_sc, 0, 1);
  sem_init(&sem_prot_stop, 0, 1);

  // Creamos el vector que contiene los IDs de los nodos vecinos
  crearVector();


  // Hacer un thread para el proceso receptor
  pthread_create(&hiloR,NULL,procesoReceptor,"");

  // Creamos los hijos en un hilo cada uno
  for(int i = 0;i < num_hijos; i++) { pthread_create(&hilosH[i],NULL,hijo,""); };

  while(1) {

    // Si no es la primera vez espera aquí hasta que pulsemos una tecla. En ese momento generará nuevos hijos
    sem_wait(&sem_prot_quiero);
    if (!quiero && !first) {
      sem_post(&sem_prot_quiero);
      getchar();

      for(int i = 0;i < num_hijos;i++)
        pthread_create(&hilosH[i],NULL,hijo,"");

    } else {
      sem_post(&sem_prot_quiero);
      first = 0;
    }


    sem_wait(&sem_prot_quiero);
    quiero = 1;
    sem_post(&sem_prot_quiero);

    for (int i = 0; i < N -1; i++)  sendMsg(REQUEST, id_nodos[i]);
    for (int i = 0; i < N -1; i++)  receiveMsg(id_cola_ack);

     // Si soy un lector (prioridad igual a 4 o 5) todos los nodos que tenga como pendientes van a ser lectores también, por lo que les doy paso.
    if(mi_prioridad == 4 || mi_prioridad == 5) {
      for (int i = 0; i < num_pend; i++) {
          sendMsg(REPLY, id_nodos_pend[i]);
          num_pend = 0;
      }
    }

    sem_wait(&sem_prot_sc);
    sc = 1;
    sem_post(&sem_prot_sc);
    // ------ Inicio sección crítica ----------------

    printf("Gano la exclusión mutua\n");
    num_hijos_restantes = num_hijos - cont;
    for(int i = 0;i < num_hijos_restantes;i++) {
      sem_post(&sem_paso_hijo);
      cont++;
      if(mi_prioridad != 4 && mi_prioridad != 5) sem_wait(&sem_paso_padre);

      sem_wait(&sem_prot_stop);
      if (stop == 1) {
        sem_post(&sem_prot_stop);
        printf("Ha llegado una petición más prioritaria y he dejado de dar paso a mis hijos.\n\n");
        sendMsg(REPLY, nodo_prioritario);
        break; // Sale del for
      }
      sem_post(&sem_prot_stop);
    }

    // Si ha llegado alguien más prioritario vuelvo a empezar el bucle de ejecución
    if (stop == 1) { first = 1; stop = 0; continue;};
      // --------- Fin sección crítica ----------------

    sem_wait(&sem_prot_sc);
    sc = 0;
    sem_post(&sem_prot_sc);

    sem_wait(&sem_prot_quiero);
    quiero = 0;
    sem_post(&sem_prot_quiero);


    printf("Número de nodos pendientes: %i\n",num_pend );
    for (int i = 0; i < num_pend; i++) { sendMsg(REPLY, id_nodos_pend[i]); }

    num_pend = 0;
    cont = 0;

  } //Cierre while(1)
} // Cierre main

void *procesoReceptor()
{
  while(1)
  {
    int id_nodo_origen, prioridad_origen;

    mensaje msg = receiveMsg(id_cola);

    id_nodo_origen = msg.id_nodo;
    prioridad_origen = msg.prioridad;

    if (quiero != 1 || (prioridad_origen < mi_prioridad && sc != 1) || (prioridad_origen == mi_prioridad && id_nodo_origen < mi_id && sc != 1) ) {
      sendMsg(REPLY,id_nodo_origen);
    } else {
      printf("Añadido nodo a la lista de pendientes\n" );
      if(prioridad_origen < mi_prioridad && sc==1) {
        stop = 1;
        printf("Recibí petición con más prioridad desde %i\n", id_nodo_origen);
        nodo_prioritario = id_nodo_origen;
      } else {
        id_nodos_pend[num_pend++] = id_nodo_origen;
        printf("%i\n", num_pend);
      }
    }
  } // Cierre while(1)
  pthread_exit(NULL);
} // Cierre procesoReceptor()

int sendMsg(int tipo, int id_destino) {
  mensaje msg;
  msg.id_nodo = mi_id;
  msg.mtype = id_destino;

  if (tipo == REPLY) {
    msg.prioridad = REPLY;
    return msgsnd (id_cola_ack, (struct msgbuf *)&msg, sizeof(msg.prioridad)+sizeof(msg.id_nodo)+sizeof(msg.mtype), 0);
  } else {
    msg.prioridad = mi_prioridad;
    return msgsnd (id_cola, (struct msgbuf *)&msg, sizeof(msg.prioridad)+sizeof(msg.id_nodo)+sizeof(msg.mtype), 0);
  }
} // Cierre sendMsg


mensaje receiveMsg(int id_cola) {

  mensaje msg;
  int res = msgrcv (id_cola, (struct msgbuf *)&msg, sizeof(msg), mi_id, 0);

  printf ("Recibo desde: %i y la cola es %i\n", msg.id_nodo,id_cola);

  return msg;
} // Cierre receiveMsg()

void crearVector() {
  for(int i = 0,j = 0; i < N; i++) {
    if (i + 1 != mi_id) id_nodos[j++] = (i + 1);
  }
} // Cierre crearVector()

void *hijo (void *arg) {

  char nombre[30];

  // Mensaje de bienvenida
  switch (mi_prioridad) {
    case 1:  strcpy(nombre, "pagos"); break;
    case 2:  strcpy(nombre, "anulaciones"); break;
    case 3:  strcpy(nombre, "pre-reservas"); break;
    case 4:  strcpy(nombre, "gradas"); break;
    case 5:  strcpy(nombre, "eventos"); break;
    default: strcpy(nombre, "nombre desconocido"); break;
  }

  printf("Soy un proceso de %s.\n", nombre);

  // Esperamos a que el padre nos de permiso (Inicio S.C)
  sem_wait(&sem_paso_hijo);

  // Imprimimos mensaje de S.C.
  printf("Soy un proceso de %s, y estoy en mi sección crítica.\n", nombre);

  // Devolvemos el control al padre (Fin S.C.)
  getchar();

  // Mensaje de despedida
  printf("Soy un proceso de %s, y salgo mi sección crítica.\n", nombre);

  sem_post(&sem_paso_padre);

  pthread_exit(NULL);
}
