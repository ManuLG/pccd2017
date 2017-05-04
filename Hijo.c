#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <semaphore.h>

int main (int argc, char *argv[]) {

  // Cogemos los parámetros de entrada
  if (argc != 4) printf("Por favor, indique el tipo de proceso y los dos semáforos.\n");
  int tipo = atoi(argv[1]);
  sem_t semH = atoi(argv[2]);
  sem_t semP = atoi(argv[3]);

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
  sem_wait(semH);

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
  sem_post(semP);

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

}
