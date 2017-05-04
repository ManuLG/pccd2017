#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h> //Prints
#include <stdlib.h>
#include <unistd.h> // Fork

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

// Prototipos de las funciones
    int sendMsg();
    mensaje receiveMsg();

int main(int argc, char const *argv[]) {

  mi_id = atoi(argv[1]);
  mi_prioridad = atoi(argv[2]);

  key_t key = ftok("/tmp", 123);
  id_cola = msgget(key, 0777 | IPC_CREAT);

  printf("ID de la cola: %i\n", id_cola);

  // Hacer un fork para el proceso receptor
  // Hacer un fork para el proceso que crea los hijos?

  while (quiero) {

    for (int i = 0; i < N -1; i++) { sendMsg(REQUEST, id_nodos[i]); }
    for (int i = 0; i < N -1; i++) { receiveMsg(); }

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

  mensaje msg = receiveMsg();

  id_nodo_origen = msg.id_nodo;
  ticket_origen = msg.prioridad;
  
  if ((quiero != 1) || (ticket_origen < mi_ticket) || ((ticket_origen == mi_ticket && (id_nodo_origen < mi_id)))) {
    sendMsg(id_nodo_origen);
  } else {
    id_nodos_pend[num_pend++] = id_nodo_origen;
  }
} // Cierre procesoReceptor()

int sendMsg(int tipo, int id_destino) {
  mensaje msg;

  if (tipo == REPLY) {
    msg.prioridad = REPLY;
  } else {
    msg.prioridad = mi_prioridad;
  }
  
  msg.id_nodo = mi_id;
  msg.mtype = id_destino; // ¿Destinatario?

  return msgsnd (id_cola, (struct msgbuf *)&msg, sizeof(msg.prioridad)+sizeof(msg.id_nodo)+sizeof(msg.mtype), 0);
} // Cierre sendMsg

// receive()
mensaje receiveMsg() {

  mensaje msg;
  int res = msgrcv (id_cola, (struct msgbuf *)&msg, sizeof(msg), mi_id, 0);

  printf ("Recibo desde: %i\n", msg.id_nodo);

  return msg;
} // Cierre receiveMsg()