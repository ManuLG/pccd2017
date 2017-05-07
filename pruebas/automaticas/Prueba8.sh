#!/bin/sh

# En esta prueba llegan varios procesos de varios tipos.
# Deberán entrar por orden de prioridad, y cuando los escritores acaben (ejecutarán su sección crítica en exclusión mútua), entrarán los lectores en su sección crítica concurrentemente.

./main 1 5 1 5 # ID_Nodo Numero_Nodos Prioridad Numero_Hijos
./main 1 5 2 3
./main 1 5 3 2
./main 2 5 1 1
./main 2 5 2 2
./main 2 5 3 3
./main 2 5 4 4
./main 2 5 5 5
./main 3 5 2 1
./main 3 5 3 4
./main 4 5 1 3
./main 4 5 2 2
./main 4 5 3 1
./main 5 5 4 7
./main 5 5 5 7
