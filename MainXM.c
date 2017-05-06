#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h> //Prints
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
int mi_ticket = 0;
int mi_prioridad = 0;
int max_ticket = 0;

// Arrays de nodos a tener en cuenta
int id_nodos[N-1] = {2, 3};
int id_nodos_pend[N-1];
int num_pend = 0;
// Variables para serializar
int quiero = 1;
int sc=0;
int stop=0;
// Colas de mesajes
int id_cola = 0;
int id_cola_ack = 0;
// semáforos
sem_t semH;
sem_t semP;
// cantidades
int num_hijos=0;
int iter=0;




int main(int argc, char const *argv[]) {

  mi_id = atoi(argv[1]);
  mi_prioridad = atoi(argv[2]);
  num_hijos= atoi(argv[3]);
  iter=num_hijos;

  key_t key = ftok("/tmp", 123);
  key_t key_2 = ftok("/tmp", 1234);
  id_cola = msgget(key, 0777 | IPC_CREAT);
  id_cola_ack = msgget(key_2, 0777 | IPC_CREAT);
  pthread_t hiloR;
  pthread_t hilosH[255];

// Adicion mía
  sem_init(&semH,0,1);
  sem_init(&semP,0,1);

// Adicion mía
  if(num_hijos>0)
    quiero=1;

  sem_wait(&semP); // Bloqueamos a los hijos
  for(int i=0;i<num_hijos;i++)
    pthread_create(&hilosH[i],NULL,hijo,"");

  crearVector();
  printf("ID de la cola: %i\n", id_cola);

  // Hacer un thread para el proceso receptor
  pthread_create(&hiloR,NULL,procesoReceptor,"");

  // Hacer un thread para el proceso que crea los hijos?
  while(1)
  {
    while (quiero)
    {

      for (int i = 0; i < N -1; i++)  sendMsg(REQUEST, id_nodos[i]);
      for (int i = 0; i < N -1; i++)  receiveMsg(id_cola_ack);
      if(mi_prioridad == 4 || mi_prioridad == 5) for (int i = 0; i < num_pend; i++)sendMsg(REPLY, id_nodos_pend[i]);

      sc=1;

      // ------ Inicio sección crítica ----------------
      printf("ENTRO EN LA SECCION CRITICA\n");
      for(int i=0;i<num_hijos;i++)sem_post(&semP); //Subimos el contador interno del semáforo para que pasen todos
      for(int i=0;i<num_hijos;i++)pthread_join(hilosH[i],NULL); //Esperamos hasta nuestro último hijo
      // --------- Fin sección crítica ----------------

      sc=0;
      quiero = 0;
      printf("num_pend: %i\n",num_pend );
      for (int i = 0; i < num_pend; i++) { sendMsg(REPLY, id_nodos_pend[i]); }

      num_pend = 0;

    } // Cierre while de quiero

  } //Cierre while de proceso padre permanente

} // Cierre main

void *procesoReceptor()
{
  while(1)
  {
    int id_nodo_origen, ticket_origen;

    mensaje msg = receiveMsg(id_cola);

    id_nodo_origen = msg.id_nodo;
    ticket_origen = msg.prioridad;
    mi_ticket = mi_prioridad;

    if (quiero != 1 || (ticket_origen < mi_ticket && sc!=1) || (ticket_origen == mi_ticket && id_nodo_origen < mi_id && sc!=1) )
    {
      printf("procesoReceptor\n" );
      sendMsg(REPLY,id_nodo_origen);
    }
    else
    {
      printf("Añadido pendiente\n" );
      if(ticket_origen < mi_ticket && sc==1)
      {
        stop=1;
        printf("Recibí petición con más prioridad\n");
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
    printf("Sending reply\n");
    msg.prioridad = REPLY;
    return msgsnd (id_cola_ack, (struct msgbuf *)&msg, sizeof(msg.prioridad)+sizeof(msg.id_nodo)+sizeof(msg.mtype), 0);
  }

  printf("Sending request\n");
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
    printf("mi id: %i    valor i: %i   valo j: %i\nM",mi_id,i,j);
    if(i+1!=mi_id)id_nodos[j++]=(i+1);

  }
    printf("valor array: %i  %i \n",id_nodos[0],id_nodos[1]);

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
  sem_wait(&semP);
  if(mi_prioridad!=4 && mi_prioridad!=5)
    sem_wait(&semH);

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
  //getchar();

  // Mensaje de despedida
  switch (tipo) {

    case 1:  printf("Soy un proceso de pagos, y parece que ya acabé de hacer cosas.\n");
             break;
    case 2:  printf("Soy un proceso de anulaciones, y parece que ya acabé de hacer cosas.\n");
             break;
    case 3:  printf("Soy un proceso de pre-reservas, y parece que ya acabé de hacer cosas.\n");
             break;
    case 4:  printf("Soy un proceso de gradas, y parece que ya acabé de hacer cosas.\n");
             break;
    case 5:  printf("Soy un proceso de eventos, y parece que ya acabé de hacer cosas.\n");
             break;
    default: printf("Soy un proceso desconocido ...\n");
             break;

  }
  if(mi_prioridad!=4 && mi_prioridad!=5)
    sem_post(&semH);
  pthread_exit(NULL);
}
