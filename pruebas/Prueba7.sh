#!/bin/bash

# Sólo hay procesos lectores.

./main 1 2 4 2 # ID_Nodo Numero_Nodos Prioridad Numero_Hijos
./main 2 2 5 3
sleep 2

# Llega un proceso de pre-reservas en un nodo diferente que quiere entrar en su sección crítica.
# Mientras siguen llegando procesos lectores

./main 3 3 3 1
./main 1 3 4 2
./main 1 3 4 3

### REVISAR! ###
