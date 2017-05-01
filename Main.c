#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h> //Prints
#include <stdlib.h>

#define N 2 // Número de nodos
#define PRIORIDAD 1
#define ID_NODO 2

typedef struct mensaje {
  int prioridad;
  int id_nodo;
} mensaje;

int id = 0;

// Prototipos de las funciones
int sendMsg();
int receiveMsg();

int main(){
	while (1) {

		key_t key = ftok("/tmp", 123);
		id = msgget(key, 0777 | IPC_CREAT);

		printf("ID de la cola: %i", id);

		printf ("%i\n", sendMsg(id));
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

int sendMsg(int id_destino) {
	mensaje msg;

	msg.prioridad = PRIORIDAD;
	msg.id_nodo = ID_NODO;
	return msgsnd (id_destino, (struct msgbuf *)&msg, sizeof(msg), 0);
}

int receiveMsg() {

	mensaje msg;

	int res = msgsnd (id, (struct msgbuf *)&msg, sizeof(msg.prioridad)+sizeof(msg.id_nodo), 0);

	printf ("Recibo: %i", msg.prioridad);

	/*

		Hacer lo que toque al recibir un mensaje

	*/
	return 4;
}