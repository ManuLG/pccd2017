#!/bin/bash

# Hay un proceso de eventos en la sección crítica.

./main 1 1 5 1 # ID_Nodo Numero_Nodos Prioridad Numero_Hijos
sleep 1

# Un proceso de pre-reservas (un escritor) quiere acceder a su sección crítica. Pero dado que el lector ya está en la sección crítica, éste deberá esperar para poder entrar.

./main 1 1 3 1
