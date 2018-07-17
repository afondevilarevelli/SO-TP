#include <pthread.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/collections/queue.h>
#include "consolaPlanificador.h"
#include "../shared/protocolo.h"
#include "../shared/mySocket.h"
#include <string.h>
#include "algoritmosDePlanificacion.h"
#include "ESIHandling/ESIHandling.h"

int main(void){
    ESI_t esi;
    esi.state = 0;
    esi.socket = 153;
    esi.id = 60;
    esi.estimacionAnterior = 3;
    esi.duracionAnterior = 3;
    esi.tiempoEsperandoCPU = 5;

    float estimacion = algoritmoDeEstimacionProximaRafaga(&esi);

    printf("la estimacion es %f", estimacion);



    return 0;
}