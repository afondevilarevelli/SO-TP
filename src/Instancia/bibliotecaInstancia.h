#ifndef BIBLIO_INSTANCIA
#define BIBLIO_INSTANCIA

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#include <commons/config.h>
#include <commons/log.h>
#include <commons/collections/list.h>

#include "../shared/mySocket.h"
#include "../shared/buffer.h"
#include "../shared/protocolo.h"

pthread_mutex_t m_mantenimiento;

//Estructuras
typedef struct
	{
		char clave[41];
		int pointerEntrada;
		int tamanioValor;
	} t_entrada_tabla;

typedef struct
	{
		t_entrada_tabla * entrada;
		int cant;
	} entrada_LRU;
//BAncaaaaaaaa estoy haciendo algooooo


	//Variables globales
	//Registros y Tablas
	t_list * registroLRU;
	t_list * tablaDeEntradas;
	char * almacenamiento;
	//t_bitarray *bitarray;
	t_log * pLog;
	//Variables
	typedef enum{EXITO, REEMPLAZAR} validacionReemplazo;
	algReemplazo algReemp;
	int entrySize, entryCant, instanciaActiva;
	char * pathMontaje;



	//Funciones de Coonexion e Iniciaciones
	int conectarseACoordinador(t_config * pConf);
	void obtenerIPyPuertoDeCoordinador(t_config * pConf, int * ip, int * puerto);
	rtdoEjec_t agregarEntrada(char * clave, char *valor);

	void cargarTablaDeEntradasYAlmacenamiento(t_config * pConf);
	rtdoEjec_t accederRecurso(op_t operacion, char * clave, char * valor);

	//Set
	rtdoEjec_t setRecurso(char * clave, char * valor);
	//Reemplazo de Entradas
	rtdoEjec_t algoritmoLRU(char * claveNew, char *valorNew);
	rtdoEjec_t algoritmoBSU(char * claveN, char *valorN);
	rtdoEjec_t algoritmoCIRC(char * claveN, char *valorN);
	rtdoEjec_t reemplazarEntrada(char * claveNuevo, char *valor);

	/////Aux
	void reemplazarEnTabla(t_entrada_tabla *entrada,char* claveN, char *valorN);
	void reemplazarEnAlmacenamiento(t_entrada_tabla *entrada, char *valorN);
	int  nueva_entrada( char *valor, int  pointer);

	//Store
	rtdoEjec_t storeRecurso(char * clave);


	//Dump & Compactar
	void compactar();

	//OTRAS
	t_entrada_tabla * findEnTablaEntrada(char *claveN);
	void * findEnTablaLRU(char *claveN);
	int anyEnTabla(char *clave);
	void actualizarRegistro (char * claveN);
	char * obtenerValor(char * clave);
	t_entrada_tabla * new_entrada_tabla(char * clave, int * pointerAEntrada, int sizeValor);
	rtdoEjec_t agregarATabla(char * clave, int *pointerAEntrada, int sizeValor);

#endif
