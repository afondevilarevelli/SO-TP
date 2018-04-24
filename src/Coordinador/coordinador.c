#include <stdio.h>
#include <pthread.h>
#include <commons/collections/list.h>

#include "../shared/testConnection.h"
#include "../shared/mySocket.h"
#include "../shared/protocolo.h"

#define IP_COORD INADDR_ANY
#define PORT_COORD 8000

typedef struct
		{
			int id;
			int socket;
			bool connected;
		} ESI_t;
t_list * coord_ESIs;

typedef struct
		{
			int id;
			int socket;
			bool connected;
		} inst_t;
t_list * coord_Insts;

int socket_planificador;

t_list * hilos;

void atenderConexionEntrante(int listener);

void atenderInstancia(int socket);
void registrarNuevaInstancia( int inst_socket, int id );
inst_t * new_Inst( int id, int socket );
void instanciaDesconectada(int inst_ID);
inst_t * get_instancia_by_ID( t_list * instancias, int id );
bool is_instancia_ID_equal( inst_t * pInst, int id );

void atenderPlanificador(int socket);
void registrarPlanificador(int socket);
void planificadorDesconectado(void);

void atenderESI(int socket);
void registrarNuevoESI( int ESI_socket, int id );
ESI_t * new_ESI( int id, int socket );
void ESIDesconectado( int ESI_ID );
ESI_t * get_ESI_by_ID( t_list * ESIs, int id );
bool is_ESI_ID_equal( ESI_t * pESI, int id );

void procesarSolicitudESI(void * solicitud, int size);
void procesarSolicitudInstancia(void * solicitud, int size);
void procesarSolicitudPlanificador(void * solicitud, int size);

void terminarHilo( pthread_t * pHilo );

int main(void)
{
	//iniciare todas las variables globales de listas
	coord_Insts = list_create();
	coord_ESIs = list_create();
	hilos = list_create();

	//dejare al coordinador escuchar nuevas conexiones a traves del IP y PUERTO indicados
	int listener = listenOn(IP_COORD, PORT_COORD), i;
	int max_fd = listener;
	fd_set master_set, read_set;
	FD_ZERO(&master_set);
	FD_SET(listener, &master_set);

	//lo dejare atender las conexiones entrantes que detecta el listener
	while(1)
	{
		read_set = master_set;
		select(listener  + 1, &read_set, NULL, NULL, NULL);

		atenderConexionEntrante(listener);
	}

	//cuando salga del ciclo de atender conexiones, esperara a que terminen todos los hilos antes de cerrar el programa
	list_iterate( hilos, (void *)&terminarHilo );
}

void atenderConexionEntrante( int listener)
{
	tipoDeProceso_t tipoDeProceso;
	void * identificacion;

	pthread_t * pNuevoHilo = malloc(sizeof(pthread_t));

	//acepto la conexion
	int nuevaConexion = acceptClient(listener);	

	//el proceso conectado me enviara su tipo
	recvWithBasicProtocol( nuevaConexion, &identificacion);
	tipoDeProceso = *( (tipoDeProceso_t *)identificacion );
	
	//iniciare un hilo para atender al proceso segun su tipo
	switch(tipoDeProceso)
	{
		case ESI:
			pthread_create( pNuevoHilo, NULL, (void *)&atenderESI, (void *)nuevaConexion );
			break;
		case INSTANCIA:
			pthread_create( pNuevoHilo, NULL, (void *)&atenderInstancia, (void *)nuevaConexion );
			break;
		case PLANIFICADOR:
			pthread_create( pNuevoHilo, NULL, (void *)&atenderPlanificador, (void *)nuevaConexion );
			break;
		default:
			puts("Tremendo error");
			exit(1);
	}

	//agregare el hilo a la lista global de hilos del coordinador
	list_add(hilos, (void*)pNuevoHilo);
}

/*----------INSTANCIA----------*/

void atenderInstancia( int socket )
{
	int * pID, id;

	recvWithBasicProtocol( socket, (void**)&pID);
	id = *pID;
	registrarNuevaInstancia( socket, id);

	fd_set read_fds, master_fds;
	FD_ZERO(&read_fds);
	FD_ZERO(&master_fds);

	FD_SET(socket, &master_fds);

	while(1)
	{
		int size;
		void * solicitud;

		read_fds = master_fds;
		select( socket + 1, &read_fds, NULL, NULL, NULL);

		size = recvWithBasicProtocol( socket, &solicitud);

		if( size ) // SI NO SE DESCONECTO
		{
			procesarSolicitudInstancia( solicitud, size);
			free(solicitud);
		}
		else
		{
			instanciaDesconectada( id );
			return;
		}
	}
	
}

void registrarNuevaInstancia( int Inst_socket, int id )
{
	inst_t * pInst = new_Inst( id, Inst_socket);
	list_add( coord_Insts, pInst);
}


inst_t * new_Inst( int id, int socket )
{
	inst_t * pInst = malloc(sizeof(inst_t));
	pInst->id = id;
	pInst->socket = socket;
	pInst->connected = true;

	return pInst;
}

void procesarSolicitudInstancia(void * solicitud, int size)
{
	puts("Solicitud de Instancia procesada");
}

void instanciaDesconectada( int inst_ID )
{
	//buscar al ESI, Instancia o Planificador al que pudo pertenecer ese socket deconectado
	inst_t * pInst = get_instancia_by_ID( coord_Insts, inst_ID);

	//cambiar el estado de connected a false
	pInst->connected = false;

	printf("La instancia de id %d, se ha desconectado\n", inst_ID);
}

inst_t * get_instancia_by_ID( t_list * instancias, int id )
{
	t_link_element * pAct = instancias->head;
	inst_t * pInst;

	while( pAct != NULL )
	{
		pInst = (inst_t *)(pAct->data);

		if( is_instancia_ID_equal( pInst, id) )
			return pInst;

		pAct = pAct->next;
	}

	return NULL;
}

bool is_instancia_ID_equal( inst_t * pInst, int id )
{
	return pInst->id == id;
}


/*------------PLANIFICADOR------------*/

void atenderPlanificador( int socket )
{
	registrarPlanificador( socket );

	fd_set read_fds, master_fds;
	FD_ZERO(&read_fds);
	FD_ZERO(&master_fds);

	FD_SET(socket, &master_fds);

	while(1)
	{
		int size;
		void * solicitud;

		read_fds = master_fds;
		select( socket + 1, &read_fds, NULL, NULL, NULL);

		size = recvWithBasicProtocol( socket, &solicitud);

		if( size ) // SI NO SE DESCONECTO
			procesarSolicitudPlanificador( solicitud, size);
		else
			planificadorDesconectado();

		free(solicitud);
	}
	
}

void registrarPlanificador( int socket )
{
	socket_planificador = socket;
}

void procesarSolicitudPlanificador(void * solicitud, int size)
{
	puts("Solicitud de Planificador procesada");
}

void planificadorDesconectado()
{
	socket_planificador = -1;
}

/*------------ESI-----------*/

void atenderESI( int socket )
{
	int * pID, id;

	recvWithBasicProtocol( socket, (void **)&pID);
	id = *pID;
	registrarNuevoESI( socket, id);

	fd_set read_fds, master_fds;
	FD_ZERO(&read_fds);
	FD_ZERO(&master_fds);

	FD_SET(socket, &master_fds);

	while(1)
	{
		int size;
		void * solicitud;

		read_fds = master_fds;
		select( socket + 1, &read_fds, NULL, NULL, NULL);

		size = recvWithBasicProtocol( socket, &solicitud);

		if( size ) // SI NO SE DESCONECTO
		{
			procesarSolicitudESI( solicitud, size);
			free(solicitud);
		}
		else
		{
			ESIDesconectado( id );
			return;
		}

	}
	
}

void registrarNuevoESI( int ESI_socket, int id )
{
	ESI_t * pESI = new_ESI( id, ESI_socket);
	list_add( coord_ESIs, pESI);
}

ESI_t * new_ESI( int id, int socket )
{
	ESI_t * pESI = malloc(sizeof(ESI_t));
	pESI->id = id;
	pESI->socket = socket;
	pESI->connected = true;

	return pESI;
}

void procesarSolicitudESI(void * solicitud, int size)
{
	puts("Solicitud ESI procesada");
}

void ESIDesconectado( int ESI_ID )
{
	//buscar al ESI, Instancia o Planificador al que pudo pertenecer ese socket deconectado
	ESI_t * pESI = get_ESI_by_ID( coord_ESIs, ESI_ID);

	//cambiar el estado de connected a false
	pESI->connected = false;

	printf("El ESI de id %d, se ha desconectado\n", ESI_ID);
}

ESI_t * get_ESI_by_ID( t_list * ESIs, int id )
{
	t_link_element * pAct = ESIs->head;
	ESI_t * pESI;

	while( pAct != NULL )
	{
		pESI = (ESI_t *)(pAct->data);

		if( is_ESI_ID_equal( pESI, id) )
			return pESI;

		pAct = pAct->next;
	}

	return NULL;
}

bool is_ESI_ID_equal( ESI_t * pESI, int id )
{
	return pESI->id == id;
}

/*-------------GENERAL--------------*/

void terminarHilo( pthread_t * pHilo )
{
	pthread_join(*pHilo, NULL);
}