#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h> //Prints, getChar
#include <stdlib.h>
#include <unistd.h> // Fork
#include <pthread.h> //threads
#include <semaphore.h>
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

#define N 3 // Número de nodos

#define REPLY -1
#define REQUEST 1

// Identificadores
int mi_id = 1;
int mi_prioridad = 0;

// Arrays de nodos a tener en cuenta
int id_nodos[N-1] = {2, 3};
int id_nodos_pend[N-1];
int num_pend = 0;

// Variables para serializar
int quiero = 0;
int sc=0;
int stop=0;

// Colas de mesajes
int id_cola = 0;
int id_cola_ack = 0;

// semáforos
sem_t sem_paso_hijo;
sem_t sem_paso_padre;

sem_t sem_prot_quiero, sem_prot_sc, sem_prot_stop;

// cantidades
int num_hijos=0;
int cont = 0; // Contador de hijos atendidos
int first = 1;

int nodo_prioritario = 0;


int main(int argc, char const *argv[]) {

  mi_id = atoi(argv[1]);
  mi_prioridad = atoi(argv[2]);
  num_hijos= atoi(argv[3]);

  key_t key = ftok("/tmp", 123);
  key_t key_2 = ftok("/tmp", 1234);
  id_cola = msgget(key, 0777 | IPC_CREAT);
  id_cola_ack = msgget(key_2, 0777 | IPC_CREAT);
  pthread_t hiloR;
  pthread_t hilosH[2000];


  sem_init(&sem_paso_hijo,0,0);
  sem_init(&sem_paso_padre,0,0);

  sem_init(&sem_prot_quiero, 0, 1);
  sem_init(&sem_prot_sc, 0, 1);
  sem_init(&sem_prot_stop, 0, 1);

  crearVector();
  printf("ID de la cola de peticiones: %i\n", id_cola);
  printf("ID de la cola de respuestas: %i\n", id_cola_ack);

  // Hacer un thread para el proceso receptor
  pthread_create(&hiloR,NULL,procesoReceptor,"");

      for(int i=0;i<num_hijos;i++)
        pthread_create(&hilosH[i],NULL,hijo,"");
  // Hacer un thread para el proceso que crea los hijos?
  while(1)
  {

    sem_wait(&sem_prot_quiero);
    if (!quiero && !first) {
      sem_post(&sem_prot_quiero);
      getchar();

      for(int i=0;i<num_hijos;i++)
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

    if(mi_prioridad == 4 || mi_prioridad == 5) for (int i = 0; i < num_pend; i++)
    {
      sendMsg(REPLY, id_nodos_pend[i]);
      num_pend = 0;
    }
    
    sem_wait(&sem_prot_sc);
    sc=1;
    sem_post(&sem_prot_sc);
      // ------ Inicio sección crítica ----------------
      printf("Gano la exclusión mutua\n");
    for(int i = 0;i < num_hijos;i++) {
      sem_post(&sem_paso_hijo);
      cont++;
      if(mi_prioridad!=4 && mi_prioridad!=5)sem_wait(&sem_paso_padre);

      sem_wait(&sem_prot_stop);
      if (stop == 1) {
        sem_post(&sem_prot_stop);
        num_hijos -= cont;
        printf("Ha llegado una petición más prioritaria y he dejado de dar paso a mis hijos.\n\n");
        sendMsg(REPLY, nodo_prioritario);
        break;
      }
      sem_post(&sem_prot_stop);
    }

    if (stop == 1) { first = 1; stop = 0; continue;};
      // --------- Fin sección crítica ----------------

    sem_wait(&sem_prot_sc);
    sc=0;
    sem_post(&sem_prot_sc);

    sem_wait(&sem_prot_quiero);
    quiero = 0;
    sem_post(&sem_prot_quiero);


    printf("Número de nodos pendientes: %i\n",num_pend );
    for (int i = 0; i < num_pend; i++) { sendMsg(REPLY, id_nodos_pend[i]); }

    num_pend = 0;

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

    if (quiero != 1 || (prioridad_origen < mi_prioridad && sc!=1) || (prioridad_origen == mi_prioridad && id_nodo_origen < mi_id && sc!=1) )
    {
      sendMsg(REPLY,id_nodo_origen);
    }
    else
    {
      printf("Añadido nodo a la lista de pendientes\n" );
      if(prioridad_origen < mi_prioridad && sc==1)
      {
        stop=1;
        printf("Recibí petición con más prioridad desde %i\n", id_nodo_origen);
        nodo_prioritario = id_nodo_origen;
      }

      id_nodos_pend[num_pend++] = id_nodo_origen;
    }
  }
  pthread_exit(NULL);
} // Cierre procesoReceptor()

int sendMsg(int tipo, int id_destino) {
  mensaje msg;
  msg.id_nodo = mi_id;
  msg.mtype = id_destino; // ¿Destinatario?

  if (tipo == REPLY)
  {
    msg.prioridad = REPLY;
    return msgsnd (id_cola_ack, (struct msgbuf *)&msg, sizeof(msg.prioridad)+sizeof(msg.id_nodo)+sizeof(msg.mtype), 0);
  }
  msg.prioridad = mi_prioridad;

  return msgsnd (id_cola, (struct msgbuf *)&msg, sizeof(msg.prioridad)+sizeof(msg.id_nodo)+sizeof(msg.mtype), 0);
} // Cierre sendMsg

// receive()
mensaje receiveMsg(int id_cola) {

  mensaje msg;
  int res = msgrcv (id_cola, (struct msgbuf *)&msg, sizeof(msg), mi_id, 0);

  printf ("Recibo desde: %i y la cola es %i\n", msg.id_nodo,id_cola);

  return msg;
} // Cierre receiveMsg()

void crearVector()
{

  for(int i=0,j=0; i<N;i++)
  {
    if(i+1!=mi_id)id_nodos[j++]=(i+1);

  }
}

void *hijo (void *arg) {

  // Cogemos los parámetros de entrada
  int tipo = mi_prioridad;


  // Mensaje de bienvenida
  switch (tipo) {

    case 1:  printf("Soy un proceso de pagos, y acabo de ser creado.\n");
             break;
    case 2:  printf("Soy un proceso de anulaciones, y acabo de ser creado.\n");
             break;
    case 3:  printf("Soy un proceso de pre-reservas, y acabo de ser creado.\n");
             break;
    case 4:  printf("Soy un proceso de gradas, y acabo de ser creado.\n");
             break;
    case 5:  printf("Soy un proceso de eventos, y acabo de ser creado.\n");
             break;
    default: printf("Soy un proceso desconocido ...\n");
             break;

  }

  // Esperamos a que el padre nos de permiso (Inicio S.C)
  sem_wait(&sem_paso_hijo);

  // Imprimimos mensaje de S.C.
  switch (tipo) {

    case 1:  printf("Soy un proceso de pagos, y estoy en mi sección crítica.\n");
             break;
    case 2:  printf("Soy un proceso de anulaciones, y estoy en mi sección crítica.\n");
             break;
    case 3:  printf("Soy un proceso de pre-reservas, y estoy en mi sección crítica.\n");
             break;
    case 4:  printf("Soy un proceso de gradas, y estoy en mi sección crítica.\n");
             break;
    case 5:  printf("Soy un proceso de eventos, y estoy en mi sección crítica.\n");
             break;
    default: printf("Soy un proceso desconocido ...\n");
             break;

  }

  // Devolvemos el control al padre (Fin S.C.)
  getchar();

  // Mensaje de despedida
  switch (tipo) {

    case 1:  printf("Soy un proceso de pagos, y salgo de mi sección crítica.\n");
             break;
    case 2:  printf("Soy un proceso de anulaciones, y salgo de mi sección crítica.\n");
             break;
    case 3:  printf("Soy un proceso de pre-reservas, y salgo de mi sección crítica.\n");
             break;
    case 4:  printf("Soy un proceso de gradas, y salgo de mi sección crítica.\n");
             break;
    case 5:  printf("Soy un proceso de eventos, y salgo de mi sección crítica.\n");
             break;
    default: printf("Soy un proceso desconocido ...\n");
             break;

  }

  sem_post(&sem_paso_padre);

  pthread_exit(NULL);
}
