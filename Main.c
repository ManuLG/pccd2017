#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h> //Prints
#include <stdlib.h>
#include <unistd.h> // Fork
#include <signal.h> // Signals

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
    mensaje receiveMsgReply();
    void procesoReceptor();
    
int main(int argc, char const *argv[]) {

  mi_id = atoi(argv[1]);
  mi_prioridad = atoi(argv[2]);
  quiero = atoi(argv[3]);
  mi_ticket = mi_prioridad;

  key_t key = ftok("/tmp", 123);
  id_cola = msgget(key, 0777 | IPC_CREAT);

  id_cola_ack = msgget(ftok("/home/", 123), 0777 | IPC_CREAT);

  printf("ID de la cola: %i\n", id_cola);
  printf("ID de la cola ACK: %i\n", id_cola_ack);

  int pos = 0;
  for (int i = 1; i < N; i++)
  {
   if (i != mi_id) {id_nodos[pos++] = i;}
 }

 int pid = fork();
 if (pid == 0) { while(1) { procesoReceptor(); }}

 while (1) {

  for (int i = 0; i < N -1; i++) { sendMsg(REQUEST, id_nodos[i]); }
  for (int i = 0; i < N -1; i++) { receiveMsgReply(); }

    // ------ Inicio sección crítica ----------------
  printf("ENTRO EN LA SECCION CRITICA\n");
    // --------- Fin sección crítica ----------------
  quiero = 0;

  for (int i = 0; i < num_pend; i++) { sendMsg(REPLY, id_nodos_pend[i]); }

      num_pend = 0;

    return 0;
  } // Cierre while 

  kill(pid, SIGSTOP);
} // Cierre main

void procesoReceptor() {
  int id_nodo_origen, ticket_origen;

  mensaje msg = receiveMsg();

  id_nodo_origen = msg.id_nodo;
  ticket_origen = msg.prioridad;
  
  if ((quiero != 1) || (ticket_origen < mi_ticket) || ((ticket_origen == mi_ticket && (id_nodo_origen < mi_id)))) {
    sendMsg(REPLY, id_nodo_origen);
  } else {
    id_nodos_pend[num_pend++] = id_nodo_origen;
  }
} // Cierre procesoReceptor()

int sendMsg(int tipo, int id_destino) {
  mensaje msg;
  int cola = id_cola;

  if (tipo == REPLY) {
    msg.prioridad = REPLY;
    cola = id_cola_ack;
  } else {
    msg.prioridad = mi_prioridad;
  }
  
  msg.id_nodo = mi_id;
  msg.mtype = id_destino; // ¿Destinatario?

  return msgsnd (cola, (struct msgbuf *)&msg, sizeof(msg.prioridad)+sizeof(msg.id_nodo)+sizeof(msg.mtype), 0);
} // Cierre sendMsg

// receive()
mensaje receiveMsg() {

  mensaje msg;
  int res = msgrcv (id_cola, (struct msgbuf *)&msg, sizeof(msg), mi_id, 0);

  printf ("Recibo desde: %i\n", msg.id_nodo);

  return msg;
} // Cierre receiveMsg()

// receive()
mensaje receiveMsgReply() {

  mensaje msg;
  int res = msgrcv (id_cola_ack, (struct msgbuf *)&msg, sizeof(msg), mi_id, 0);

  printf ("Recibo (ACK) desde: %i\n", msg.id_nodo);

  return msg;
} // Cierre receiveMsg()

