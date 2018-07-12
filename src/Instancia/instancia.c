#include <string.h>
#include <pthread.h>
#include <unistd.h>

#include <commons/config.h>
#include <commons/log.h>
#include <commons/collections/list.h>

#include "../shared/mySocket.h"
#include "../shared/buffer.h"
#include "../shared/protocolo.h"

typedef struct
	{
		char clave[41];
		int size;
		int entryCant;
	} t_entrada;

t_entrada ** entries;

void * almacenamiento;

rtdoEjec_t accederRecurso(op_t operacion, char * clave, char * valor, int size);
rtdoEjec_t getRecurso(char * clave);
rtdoEjec_t setRecurso(char * clave, char * valor, int size);
rtdoEjec_t storeRecurso(char * clave);
rtdoEjec_t store(int pos);
int getEntryIndex(char * clave);
bool is_entrada_clave_equal( t_entrada * pEntry, char * clave);

typedef void (*fCrearEntradaPorAlg)(char*);
fCrearEntradaPorAlg crearEntradaPorAlg;

void crearEntradaPorAlgCircular( char * clave);
void crearEntradaPorAlgLRU( char * clave);
void crearEntradaPorAlgBSU( char * clave);
fCrearEntradaPorAlg obtenerAlgoritmoReemplazo(t_config * pConf);

void agregarATabla(char * clave, int pointer);
t_entrada ** cargarTablaDeEntradas(t_config * pConf);
void nuevaEntrada(void * valor, int size, char * clave, int pointer);
void obtenerIPyPuertoDeCoordinador(t_config * pConf, int * ip, int * puerto);
int conectarseACoordinador(t_config * pConf);

void dump(t_config * pConf);

t_log * pLog;
int entrySize, entryCant, instanciaActiva;
char * pathMontaje;

int main(void)
{
	pLog = log_create("instancia.log", "INSTANCIA", true, LOG_LEVEL_TRACE);
	log_trace(pLog, "Iniciando...");
	instanciaActiva = 1; //true


	t_config * pConf = config_create("instancia.config");

	log_trace(pLog, "Tabla de entradas cargada");

	int coord_socket = conectarseACoordinador(pConf);
	log_trace(pLog, "Conectada a Coordinador");

	void * infoEntries;
	int bytes = recvWithBasicProtocol(coord_socket, &infoEntries);
	tBuffer * pBuff = makeBuffer(infoEntries, bytes);
	entryCant = readIntFromBuffer(pBuff);
	entrySize = readIntFromBuffer(pBuff);
	freeBuffer(pBuff);

	entries = cargarTablaDeEntradas(pConf);

	crearEntradaPorAlg = obtenerAlgoritmoReemplazo(pConf);

	pathMontaje = config_get_string_value(pConf, "PTO_MONTAJE");

	pthread_t hiloDump;
	pthread_create(&hiloDump, NULL, (void*)&dump, (void*)pConf);

	while(1)
	{
		void * ordenDeAcceso;

		log_trace(pLog, "A la espera de solicitudes");
		int size = recvWithBasicProtocol(coord_socket, &ordenDeAcceso);

		if(size)
		{
			log_debug(pLog, "Solicitud recibida");

			int aviso = SOLICITUD_ESI_ATENDIENDOSE;
			sendWithBasicProtocol(coord_socket, &aviso, sizeof(int));
			log_trace(pLog, "Se informa al Coordinador que su solicitud esta siendo atendida");

			tBuffer * buffSent = makeBuffer(ordenDeAcceso, size);
			ESISentenciaParseada_t sent;
			sent.operacion = readIntFromBuffer(buffSent);
			sent.clave = readStringFromBuffer(buffSent);
			if(sent.operacion == SET)
			{
				sent.valor = readStringFromBuffer(buffSent);
				size = strlen(sent.valor)+1;
			}
			else
			{
				sent.valor = NULL;
				size = 0;
			}

			log_debug(pLog, "Sentencia recibida:\n op=%s, clave=%s, valor=%s\n", sent.operacion==SET?"SET":sent.operacion==GET?"GET":"STORE", sent.clave, sent.valor?sent.valor:"No corresponde");

			rtdoEjec_t rtdo = accederRecurso(sent.operacion, sent.clave, sent.valor, size);
			log_debug(pLog, "El resultado de la sentencia fue %s", rtdo==SUCCESS?"SUCCESS":rtdo==FAILURE?"FAILURE":rtdo==FIN_DE_EJECUCION?"FIN DE EJECUCION":rtdo==DISCONNECTED?"DESCONECTADO":rtdo==NO_HAY_INSTANCIAS_CONECTADAS?"NO HAY INSTANCIAS CONECTADAS":rtdo==ABORTED?"ABORTADO":"SENTENCIA" );

			freeBuffer(buffSent);

			sendWithBasicProtocol(coord_socket, &rtdo, sizeof(rtdoEjec_t));
			log_trace(pLog, "Resultado enviado al Coordinador");
		}
		else
			break;
	}

	log_error(pLog, "Se ha cortado la conexion con el Coordinador\nCerrando instancia...");
	instanciaActiva = 0; //false
	pthread_join(hiloDump, NULL);

	return 0;
}

void dump(t_config * pConf)
{
	int msec = config_get_int_value(pConf, "DUMP_INTERVAL");
	int usec = msec*1000;

	while(instanciaActiva)
	{
		usleep(usec);

		int i;
		for(i = 0; i < entryCant; i++)
			if(entries[i]->clave[0] != '\0')
				store(i);
	}
}

fCrearEntradaPorAlg obtenerAlgoritmoReemplazo(t_config * pConf)
{
	char * algoritmo = config_get_string_value(pConf, "ALG_REEMP");
	fCrearEntradaPorAlg fAlg;
	if( !strcmp(algoritmo, "CIRC") )
		fAlg = &crearEntradaPorAlgCircular;
	else if( !strcmp(algoritmo, "LRU") )
		fAlg = &crearEntradaPorAlgLRU;
	else if( !strcmp(algoritmo, "BSU") )
		fAlg = &crearEntradaPorAlgBSU;
	else
		log_error(pLog, "Algoritmo desconocido no implementado");

	log_trace(pLog, "Se cargo el algoritmo %s", algoritmo);

	free(algoritmo);

	return fAlg;
}

rtdoEjec_t accederRecurso(op_t operacion, char * clave, char * valor, int size)
{
	rtdoEjec_t rtdo;
	switch(operacion)
	{
		case GET: rtdo = getRecurso(clave); break;

		case SET: rtdo = setRecurso(clave, valor, size); break;

		case STORE: rtdo = storeRecurso(clave); break;

		default:
			log_error(pLog, "Se intento realizar una operacion invalida");
	}

	return rtdo;
}

rtdoEjec_t getRecurso(char * clave)
{
	if( !getEntryIndex(clave) )
	 	crearEntradaPorAlg(clave);

	return SUCCESS;
}

void crearEntradaPorAlgLRU( char * clave)
{
	static int pointer;
	nuevaEntrada(NULL, 0, clave, pointer);
	pointer ++;
	pointer %= entryCant;
}

void crearEntradaPorAlgBSU( char * clave)
{
	static int pointer;
	nuevaEntrada(NULL, 0, clave, pointer);
	pointer ++;
	pointer %= entryCant;
}

void crearEntradaPorAlgCircular( char * clave)
{
 	static int pointer;
	nuevaEntrada(NULL, 0, clave, pointer);
 	pointer ++;
 	pointer %= entryCant;
}

void nuevaEntrada(void * valor, int size, char * clave, int pointer)
{
	if(pointer < entryCant)
	{
		strcpy( entries[pointer]->clave, clave);
		entries[pointer]->size = size;
		entries[pointer]->entryCant = size/entrySize;
		memcpy(almacenamiento +	pointer*entrySize, valor, size);
	}
	else
	{
		log_error(pLog, "Intento de crear entrada %d mas alla del limite de %d entradas", pointer, entryCant);
	}
}

rtdoEjec_t setRecurso(char * clave, char * valor, int size)
{
	int pos = getEntryIndex(clave);
	if(size/entrySize <= entries[pos]->entryCant)
	{
		nuevaEntrada(valor, size, clave, pos);
		return SUCCESS;
	}
	else
	{
		log_error(pLog, "Se intento reemplazar un valor por otro que ocupaba mas entradas en la clave %s", clave);
		return FAILURE;
	}
}

rtdoEjec_t storeRecurso(char * clave)
{
	int pos = getEntryIndex(clave);
	return store(pos);
}

rtdoEjec_t store(pos)
{
	t_entrada * pEntry = entries[pos];
	char * path = malloc(strlen(pathMontaje) + sizeof((char)'/') + strlen(pEntry->clave) + 1);
	sprintf(path, "%s/%s",pathMontaje, pEntry->clave);
	FILE * f = fopen(path, "w+");
	int written = fwrite(almacenamiento + pos*entrySize, pEntry->size, 1, f);
	fclose(f);

	free(path);

	return written != -1?SUCCESS:FAILURE;
}

int getEntryIndex(char * clave)
{
	int i;
	for( i = 0; i < entryCant; i++ )
		if( is_entrada_clave_equal( entries[i], clave) )
			return i;

	return 0;
}

bool is_entrada_clave_equal( t_entrada * pEntry, char * clave )
{
	return !strcmp(pEntry->clave, clave);
}

t_entrada ** cargarTablaDeEntradas(t_config * pConf)
{
	t_entrada ** tabla;
	int i;

	almacenamiento = calloc(entryCant, entrySize);

	tabla = calloc(entryCant, sizeof(t_entrada *));
	for(i = 0; i < entryCant; i++)
	{
		tabla[i] = calloc(1, sizeof(t_entrada));
	}

	//tabla = malloc(entryCant*sizeof(entrada));
	//para c/archivo en carpeta "PuntoMontaje"
		//obtener nombre y size
		//crear entrada con:
			//static int i
		 	//valor=nombre
			//pos=i
			//i+= size/entrySize

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
