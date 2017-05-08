#!/bin/bash

# Un proceso de pagos y varios lectores intentan acceder a su sección crítica.

./main 1 3 1 1 & # ID_Nodo Numero_Nodos Prioridad Numero_Hijos
./main 2 3 4 5 &
./main 3 3 5 5 &
sleep 2

# Un proceso de anulaciones quiere acceder a su sección crítica mientras algunos lectores todavía siguen en ella.
# Deberá esperar a que los lectores que se están ejecutando salgan de su sección crítica.

./main 1 3 2 1 &
