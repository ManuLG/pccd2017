#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h> //Prints
#include <stdlib.h>

#define N 3 // Número de nodos
#define PRIORIDAD 2
#define ID_NODO 2

#define REPLY -1
#define REQUEST 1

typedef struct mensaje {
      long      mtype;    // Necesario para filtrar el mensaje a recibir
      int prioridad;
      int id_nodo;
} mensaje;

int mi_id = 2;
int mi_ticket = 2;
int id_nodos[N-1] = {1, 3};
int id_nodos_pend[N-1];
int num_pend = 0;
int quiero = 0;
int max_ticket = 0;

int id_cola = 0;

// Prototipos de las funciones
int sendMsg();
struct mensaje receiveMsg();
void procesoReceptor();

int main(){

  key_t key = ftok("/tmp", 123);
  id_cola = msgget(key, 0777 | IPC_CREAT);

  printf("ID de la cola: %i\n", id_cola);

  while (1) { procesoReceptor(); }
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
    msg.prioridad = PRIORIDAD;
  }
  
  msg.id_nodo = ID_NODO;
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