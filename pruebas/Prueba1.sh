#!/bin/bash

# Dos procesos quieren entrar en la sección crítica a la vez estando en el mismo nodo.
# Dado que son los únicos en nuestro sistema y que son procesos lectores, se ejecutarán concurrentemente.

./main 1 1 5 2 # ID_Nodo Numero_Nodos Prioridad Numero_Hijos
