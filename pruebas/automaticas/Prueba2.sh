#!/bin/bash

# Dos procesos de eventos quieren entrar en la sección crítica a la vez, estando en nodos diferentes.
# Dado que estamos ante un sistema distribuido donde los lectores se pueden ejecutar concurrentemente, ambos procesos se ejecutarán concurrentemente, pues son lectores y no hay ningún escritor esperando.

./main 1 2 5 1 & # ID_Nodo Numero_Nodos Prioridad Numero_Hijos
./main 2 2 5 1 &
