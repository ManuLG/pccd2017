#!/bin/bash

# Hay un proceso de pagos en su sección crítica.
# Mientras, en otros dos nodos diferentes hay procesos de lectores (eventos y gradas) suspendidos a la espera.

./main 1 3 1 1 # ID_Nodo Numero_Nodos Prioridad Numero_Hijos
./main 2 3 4 2
./main 2 3 5 3
./main 3 3 4 3
./main 3 3 5 2

# Primero entrará el proceso de pagos por ser el ms prioritario.
# Cuando este salga, entrarán los lectores, pues ya no quedaría ningún proceso escritor en ningún nodo.
