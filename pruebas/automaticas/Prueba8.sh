#!/bin/sh

# En esta prueba llegan varios procesos de varios tipos.
# Deberán entrar por orden de prioridad, y cuando los escritores acaben (ejecutarán su sección crítica en exclusión mútua), entrarán los lectores en su sección crítica concurrentemente.

./main 1 15 1 5 # ID_Nodo Numero_Nodos Prioridad Numero_Hijos
./main 2 15 2 3
./main 3 15 3 2
./main 4 15 1 1
./main 5 15 2 2
./main 6 15 3 3
./main 7 15 4 4
./main 8 15 5 5
./main 9 15 2 1
./main 10 15 3 4
./main 11 15 1 3
./main 12 15 2 2
./main 13 15 3 1
./main 14 15 4 7
./main 15 15 5 7
