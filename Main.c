#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h> //Prints
#include <stdlib.h>

#define N 2 // Número de nodos
#define PRIORIDAD 1
#define ID_NODO 2

typedef struct mensaje {
      long      mtype;    // Necesario para filtrar el mensaje a recibir
      int prioridad;
      int id_nodo;
  } mensaje;

  int mi_id = 7;
  int mi_ticket = 0;
  int id_nodos_pend[N-1];
  int num_pend = 0;
  int quiero = 0;
  int max_ticket = 0;

  int id_cola = 0;

// Prototipos de las funciones
  int sendMsg();
  int receiveMsg();

  int main(){

    key_t key = ftok("/tmp", 123);
    id_cola = msgget(key, 0777 | IPC_CREAT);

    printf("ID de la cola: %i\n", id_cola);

    while (1) {

      printf ("%i\n", sendMsg(mi_id));

      for (int i = 0; i < 10; i++) {
        printf ("Envío %i\n", i);
        sendMsg(i);
    }

      receiveMsg();

        // Pseudocódigo

        /*

        int quiero = 1;
        int mi_ticket = max_ticket + 1;
        for (i = 0; i < N-1; i++) {send(REQUEST, id_nodos[i], {mi_id, mi_ticket})};
        for (i = 0; i < N-1; i++) {receive(REPLY, &id_aux)};

        */

        /*
            SECCIÓN CRÍTICA
        */

        /*

        quiero = 0;
        for (i = 0; i < num_pend; i++) { send(REPLY, id_nodos_pend[i], mi_id);}

        num_pend = 0;

        */
    } // Cierre while 
} // Cierre main

// Pseudocodigo guia
void procesoReceptor() {
    int id_nodo_origen, ticket_origen;
    if ((quiero == 1) || (ticket_origen < mi_ticket) || ((ticket_origen == mi_ticket && (id_nodo_origen < mi_id)))) {

    }
}

int sendMsg(int id_destino) {
    mensaje msg;

    msg.prioridad = PRIORIDAD;
    msg.id_nodo = ID_NODO;
    msg.mtype = id_destino; // ¿Destinatario?

    return msgsnd (id_cola, (struct msgbuf *)&msg, sizeof(msg.prioridad)+sizeof(msg.id_nodo)+sizeof(msg.mtype), 0);
} // Cierre sendMsg



// receive()
int receiveMsg() {

    mensaje msg;

    int res = msgrcv (id_cola, (struct msgbuf *)&msg, sizeof(msg.prioridad)+sizeof(msg.id_nodo)+sizeof(msg.mtype), mi_id, 0);

    printf ("Recibo: %i\n", msg.mtype);

    return res;
} // Cierre receiveMsg()