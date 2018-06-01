#include <commons/config.h>
#include <commons/collections/list.h>

#include "../shared/mySocket.h"
#include "../shared/buffer.h"
#include "../shared/protocolo.h"

#define ENTRY_SIZE 50
#define MAX_ENTRADA 5

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

t_list * cargarTablaDeEntradas(void);
t_entrada * new_entrada(char * clave, int size);
void obtenerIPyPuertoDeCoordinador(t_config * pConf, int * ip, int * puerto);

int main(void)
{
	tablaDeEntradas = cargarTablaDeEntradas();
	int coord_socket = conectarseACoordinador();

	while(1)
	{
		void * ordenDeAcceso;
		int size = recvWithBasicProtocol(coord_socket, &ordenDeAcceso);
		printf("size=%d\n", size);

		int aviso = SOLICITUD_ESI_ATENDIENDOSE;
		sendWithBasicProtocol(coord_socket, &aviso, sizeof(int));

		tBuffer * buffSent = makeBuffer(ordenDeAcceso, size);
		ESISentenciaParseada_t sent;
		sent.operacion = readIntFromBuffer(buffSent);
		sent.clave = readStringFromBuffer(buffSent);
		if(sent.operacion == SET)
			sent.valor = readStringFromBuffer(buffSent);
		else
			sent.valor = NULL;

		freeBuffer(buffSent);

		printf("op=%d, clave=%s, valor=%s\n", sent.operacion, sent.clave, sent.valor?sent.valor:"No corresponde");
		rtdoEjec_t rtdo = accederRecurso(sent.operacion, sent.clave, sent.valor);
		sendWithBasicProtocol(coord_socket, &rtdo, sizeof(rtdoEjec_t));
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
			puts("Error: Operacion invalida");
	}

	return rtdo;
}

rtdoEjec_t getRecurso(char * clave)
{
	if( getEntrada(clave) )
		return SUCCESS;
  else
	 	crearEntradaPorAlgCircular(clave);
}


void crearEntradaPorAlgCircular( char * clave)
{
 	static int pointer;
	agregarATabla(clave, pointer);
 	pointer ++;
 	pointer %= MAX_ENTRADA;
}

void agregarATabla(char * clave, int pointer)
{
	list_replace( tablaDeEntradas, pointer, new_entrada(clave, ENTRY_SIZE) );
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
	strcpy(pEntry->clave, clave);
	pEntry->valor = malloc(size);

	return pEntry;
}

t_list * cargarTablaDeEntradas(void)
{
	t_list * tabla = list_create();
	list_add(tabla, new_entrada("jugador", ENTRY_SIZE));

	return tabla;
}

int conectarseACoordinador(void)
{
	t_config * pConf = config_create("instancia.config");
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
