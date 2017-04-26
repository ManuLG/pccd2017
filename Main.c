#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h> //Prints
#include <stdlib.h>

#define N 2 // Número de nodos

struct mensaje {
  int prioridad;
  int id_nodo;
}


int main(){
	while (1) {
		int quiero = 1;
		int mi_ticket = max_ticket + 1;
		for (i = 0; i < N-1; i++) {send(REQUEST, id_nodos[i], {mi_id, mi_ticket})};
		for (i = 0; i < N-1; i++) {receive(REPLY, &id_aux)};

		/*

			SECCIÓN CRÍTICA

		*/

		quiero = 0;
		for (i = 0; i < num_pend; i++) { send(REPLY, id_nodos_pend[i], mi_id);}
			
		num_pend = 0;
	} // Cierre while 
} // Cierre main