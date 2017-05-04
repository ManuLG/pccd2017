#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h> //Prints
#include <stdlib.h>
#include <unistd.h> // Fork

void procesoReceptor();
void crearVector();

#define N 3 // Número de nodos

#define REPLY -1
#define REQUEST 1

typedef struct mensaje {
      long      mtype;    // Necesario para filtrar el mensaje a recibir
      int prioridad;
      int id_nodo;
    } mensaje;

    int mi_id = 1;
    int mi_prioridad = 0;
    int mi_ticket = 0;
    int id_nodos[N-1] = {2, 3};
    int id_nodos_pend[N-1];
    int num_pend = 0;
    int quiero = 1;
    int max_ticket = 0;

    int id_cola = 0;
    int id_cola_ack = 0;

// Prototipos de las funciones
    int sendMsg();
    mensaje receiveMsg();

int main(int argc, char const *argv[]) {

  mi_id = atoi(argv[1]);
  mi_prioridad = atoi(argv[2]);

  key_t key = ftok("/tmp", 123);
  key_t key_2 = ftok("/tmp", 1234);
  id_cola = msgget(key, 0777 | IPC_CREAT);
  id_cola_ack = msgget(key_2, 0777 | IPC_CREAT);

  crearVector();
  printf("ID de la cola: %i\n", id_cola);

  // Hacer un fork para el proceso receptor
  int pid=fork();
  if(pid==0)
  {
    while(1)procesoReceptor();
  }

  // Hacer un fork para el proceso que crea los hijos?

  while (quiero) {

    for (int i = 0; i < N -1; i++) { sendMsg(REQUEST, id_nodos[i]); }
    for (int i = 0; i < N -1; i++) { receiveMsg(id_cola_ack); }
    if(mi_prioridad == 4 || mi_prioridad == 5) for (int i = 0; i < num_pend; i++) { sendMsg(REPLY, id_nodos_pend[i]); }
    // ------ Inicio sección crítica ----------------
    printf("ENTRO EN LA SECCION CRITICA\n");
    // --------- Fin sección crítica ----------------
    quiero = 0;

    for (int i = 0; i < num_pend; i++) { sendMsg(REPLY, id_nodos_pend[i]); }

    num_pend = 0;

    } // Cierre while
} // Cierre main

void procesoReceptor() {
  int id_nodo_origen, ticket_origen;

  mensaje msg = receiveMsg(id_cola);

  id_nodo_origen = msg.id_nodo;
  ticket_origen = msg.prioridad;
  mi_ticket = mi_prioridad;

  if ((quiero != 1) || (ticket_origen < mi_ticket) || ((ticket_origen == mi_ticket && (id_nodo_origen < mi_id)))) {
    sendMsg(id_nodo_origen);
  } else {
    id_nodos_pend[num_pend++] = id_nodo_origen;
  }
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

  printf ("Recibo desde: %i\n", msg.id_nodo);

  return msg;
} // Cierre receiveMsg()

void crearVector()
{

  for(int i=0,j=0; i<N;i++)
  {
    printf("mi id: %i    valor i: %i   valo j: %i\nM",mi_id,i,j);
    if(i+1!=mi_id)id_nodos[j++]=(i+1);
    //printf("valor array: %i\n",id_nodos[j-1]);
  }
    printf("valor array: %i  %i \n",id_nodos[0],id_nodos[1]);

}
