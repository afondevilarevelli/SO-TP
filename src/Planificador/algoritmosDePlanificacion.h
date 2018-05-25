#ifndef ALGORITMOS_PLANIF_H
#define ALGORITMOS_PLANIF_H

#include <stdio.h>
#include <stdlib.h>
#include <commons/collections/queue.h>

#define alpha 40;

struct infoESI{
    int idESI;
    
}infoESI;

t_queue ordenarColaSegunSJF(int id_ESI);

t_queue ordenarColaSegunSRT(int id_ESI);

t_queue ordenarColaSegunHRRN(int id_ESI);

// ordena la cola y elige el siguiente ESI a ejecutar
#endif
