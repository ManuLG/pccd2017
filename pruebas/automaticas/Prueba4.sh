#!/bin/bash

# Hay un proceso de anulaciones en la sección crítica.

./main 1 1 2 1 # ID_Nodo Numero_Nodos Prioridad Numero_Hijos
sleep 1

# Un proceso de pre-reservas (un escritor) quiere acceder a su sección crítica en el mismo nodo.
# A la vez, un proceso de pagos en otro nodo también quiere acceder a su sección crítica.

./main 1 2 3 1
./main 2 2 1 1

# Cuando el de anulaciones salga de su sección crítica, entrará el de pagos, y cuando éste salga, entrará el de pre-reservas.

### REVISAR! ###
