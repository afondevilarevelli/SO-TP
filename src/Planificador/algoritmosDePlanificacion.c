#include "algoritmosDePlanificacion.h"
#include <stdlib.h>

t_queue colaESIs = queue_create(); //cola de ID de ESI's

int ordenarColaSegunFifo(int id_ESI){
    return queue_pop(colaESI);
}

void ordenarColaSegunSJF(int id_ESI);

void ordenarColaSegunSRT(int id_ESI);

void ordenarColaSegunHRRN(int id_ESI);