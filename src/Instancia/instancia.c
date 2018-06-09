#include <commons/config.h>
#include <commons/log.h>
#include <commons/collections/list.h>

#include "../shared/mySocket.h"
#include "../shared/buffer.h"
#include "../shared/protocolo.h"

typedef struct
	{
		char clave[41];
		char * valor;
	} t_entrada;

t_list * tablaDeEntradas;

rtdoEjec_t accederRecurso(op_t operacion, char * clave, char * valor);
rtdoEjec_t getRecurso(char * clave);
rtdoEjec_t setRecurso(char * clave, char * valor);
rtdoEjec_t storeRecurso(char * clave);
t_entrada * getEntrada(char * clave);
bool is_entrada_clave_equal( t_entrada * pEntry, char * clave);

void crearEntradaPorAlgCircular( char * clave);
void agregarATabla(char * clave, int pointer);

t_list * cargarTablaDeEntradas(t_config * pConf);
t_entrada * new_entrada(char * clave, int size);
void obtenerIPyPuertoDeCoordinador(t_config * pConf, int * ip, int * puerto);
int conectarseACoordinador(t_config * pConf);

t_log * pLog;
int entrySize, entryCant;

int main(void)
{
	pLog = log_create("instancia.log", "INSTANCIA", true, LOG_LEVEL_TRACE);
	log_trace(pLog, "Iniciando...");


	t_config * pConf = config_create("instancia.config");
	tablaDeEntradas = cargarTablaDeEntradas(pConf);
	log_trace(pLog, "Tabla de entradas cargada");

	int coord_socket = conectarseACoordinador(pConf);
	log_trace(pLog, "Conectada a Coordinador");

	while(1)
	{
		void * ordenDeAcceso;

		log_trace(pLog, "A la espera de solicitudes");
		int size = recvWithBasicProtocol(coord_socket, &ordenDeAcceso);
		log_debug(pLog, "Solicitud recibida");

		int aviso = SOLICITUD_ESI_ATENDIENDOSE;
		sendWithBasicProtocol(coord_socket, &aviso, sizeof(int));
		log_trace(pLog, "Se informa al Coordinador que su solicitud esta siendo atendida");

		tBuffer * buffSent = makeBuffer(ordenDeAcceso, size);
		ESISentenciaParseada_t sent;
		sent.operacion = readIntFromBuffer(buffSent);
		sent.clave = readStringFromBuffer(buffSent);
		if(sent.operacion == SET)
			sent.valor = readStringFromBuffer(buffSent);
		else
			sent.valor = NULL;

		freeBuffer(buffSent);

		log_debug(pLog, "Sentencia recibida:\n op=%s, clave=%s, valor=%s\n", sent.operacion==SET?"SET":sent.operacion==GET?"GET":"STORE", sent.clave, sent.valor?sent.valor:"No corresponde");

		rtdoEjec_t rtdo = accederRecurso(sent.operacion, sent.clave, sent.valor);
		log_debug(pLog, "El resultado de la sentencia fue %s", rtdo==SUCCESS?"SUCCESS":rtdo==FAILURE?"FAILURE":rtdo==FIN_DE_EJECUCION?"FIN DE EJECUCION":rtdo==DISCONNECTED?"DESCONECTADO":rtdo==NO_HAY_INSTANCIAS_CONECTADAS?"NO HAY INSTANCIAS CONECTADAS":rtdo==ABORTED?"ABORTADO":"SENTENCIA" );

		sendWithBasicProtocol(coord_socket, &rtdo, sizeof(rtdoEjec_t));
		log_trace(pLog, "Resultado enviado al Coordinador");
	}

	return 0;
}

rtdoEjec_t accederRecurso(op_t operacion, char * clave, char * valor)
{
	rtdoEjec_t rtdo;
	switch(operacion)
	{
		case GET: rtdo = getRecurso(clave); break;

		case SET: rtdo = setRecurso(clave, valor); break;

		case STORE: rtdo = storeRecurso(clave); break;

		default:
			log_error(pLog, "Se intento realizar una operacion invalida");
	}

	return rtdo;
}

rtdoEjec_t getRecurso(char * clave)
{
	if( getEntrada(clave) )
	{
		//log_trace(pLog, "La clave se encontraba en ")
		return SUCCESS;
	}
  else
	 	crearEntradaPorAlgCircular(clave);
}


void crearEntradaPorAlgCircular( char * clave)
{
 	static int pointer;
	agregarATabla(clave, pointer);
 	pointer ++;
 	pointer %= entryCant;
}

void agregarATabla(char * clave, int pointer)
{
	list_replace( tablaDeEntradas, pointer, new_entrada(clave, entrySize) );
}

rtdoEjec_t setRecurso(char * clave, char * valor)
{
	t_entrada * pEntry = getEntrada(clave);
	strcpy(pEntry->valor, valor);

	return SUCCESS;
}

rtdoEjec_t storeRecurso(char * clave)
{
	FILE * f = fopen(clave, "w+");
	t_entrada * pEntry = getEntrada(clave);
	int size = strlen(pEntry->valor)+1;
	int written = fwrite(pEntry->valor, size, 1, f);
	fclose(f);

	return (f != NULL && written != -1)?SUCCESS:FAILURE;
}

t_entrada * getEntrada(char * clave )
{
	t_link_element * pAct = tablaDeEntradas->head;
	t_entrada * pEntry;

	while( pAct != NULL )
	{
		pEntry = (t_entrada *)(pAct->data);

		if( is_entrada_clave_equal( pEntry, clave) )
			return pEntry;

		pAct = pAct->next;
	}

	return NULL;
}

bool is_entrada_clave_equal( t_entrada * pEntry, char * clave )
{
	return !strcmp(pEntry->clave, clave);
}

t_entrada * new_entrada(char * clave, int size)
{
	t_entrada * pEntry = malloc(sizeof(t_entrada));
	if(clave)
		strcpy(pEntry->clave, clave);
	else
		memset(pEntry->clave, '\0', sizeof(pEntry->clave));

	pEntry->valor = malloc(size);

	return pEntry;
}

t_list * cargarTablaDeEntradas(t_config * pConf)
{
	entrySize = config_get_int_value(pConf, "ENTRY_SIZE");
	entryCant = config_get_int_value(pConf, "ENTRY_CANT");

	t_list * tabla = list_create();
	list_add(tabla, new_entrada(NULL, entrySize));

	return tabla;
}

int conectarseACoordinador(t_config * pConf)
{
	int ip, puerto;
	obtenerIPyPuertoDeCoordinador(pConf, &ip, &puerto);
	int coord_socket = connectTo(ip, puerto);

	tProceso tipoDeProceso = INSTANCIA;
	int instancia_ID = config_get_int_value(pConf, "INST_ID");

	sendWithBasicProtocol( coord_socket, &tipoDeProceso, sizeof(tProceso));
	sendWithBasicProtocol( coord_socket, &instancia_ID, sizeof(int));

	return coord_socket;
}

void obtenerIPyPuertoDeCoordinador(t_config * pConf, int * ip, int * puerto)
{
	char * strIP = config_get_string_value(pConf, "COORD_IP");
	*ip = inet_addr(strIP);

	//puts(strIP);

	*puerto= htons(config_get_int_value(pConf, "COORD_PUERTO"));
}
