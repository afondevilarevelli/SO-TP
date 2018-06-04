#ifndef PROTOCOLO_H
#define PROTOCOLO_H

typedef enum{

	ESI,

	INSTANCIA,

	PLANIFICADOR,

	COORDINADOR

} tProceso;

#ifndef PARSER_H_
typedef enum{GET, SET, STORE} op_t;
#endif

typedef struct{

	int operacion;

	char * clave;

	char * valor;

	} ESISentenciaParseada_t;

typedef enum{BLOQUEAR, EJECUTAR} orden_t;

typedef enum{FAILURE, SUCCESS} rtdoEjec_t;

typedef enum{FIFO,SJF, SRT, HHRR} algoritmoPlanificacion;

#define FIN_DE_EJECUCION 37
#define SOLICITUD_ESI_ATENDIENDOSE 13

#endif

