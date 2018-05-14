#ifndef PROTOCOLO_H
#define PROTOCOLO_H

typedef enum{

	ESI,

	INSTANCIA,

	PLANIFICADOR

	} tipoDeProceso_t;


typedef enum{GET, SET, STORE} op_t;

typedef struct{

	op_t operacion;

	char * clave;

	char * valor;

	} ESISentenciaParseada_t;

#endif
