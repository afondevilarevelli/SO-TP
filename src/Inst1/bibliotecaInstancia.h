#ifndef BIBLIO_INSTANCIA
#define BIBLIO_INSTANCIA

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include <commons/config.h>
#include <commons/log.h>
#include <commons/bitarray.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <commons/txt.h>

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

	//Variables globales
	//Registros y Tablas
	t_list * registroLRU;
	t_list * tablaDeEntradas;
	char * almacenamiento;
	t_bitarray *bitarray;
	t_log * pLog;
	//Variables
	int coord_socket ;
	//typedef enum{EXITO, REEMPLAZAR} validacionReemplazo;
	algReemplazo algReemp;
	int entrySize, entryCant, instanciaActiva;
	char * pathMontaje;


	//Funciones de Coonexion e Iniciaciones
	int conectarseACoordinador(t_config * pConf);
	void obtenerIPyPuertoDeCoordinador(t_config * pConf, int * ip, int * puerto);
	int punteroLugarDisponible(char *valor);
	rtdoEjec_t agregarEntrada(char * clave, char *valor);
	void cargarTablaDeEntradasYAlmacenamiento(t_config * pConf);
	rtdoEjec_t accederRecurso(op_t operacion, char * clave, char * valor);

	//Set
	rtdoEjec_t setRecurso(char * clave, char * valor);
	//Reemplazo de Entradas
	void actualizarRegistro (char * claveN);//del Algoritmo LRU
	rtdoEjec_t algoritmoLRU(char * claveNew, char *valorNew);
	rtdoEjec_t algoritmoBSU(char * claveN, char *valorN);
	rtdoEjec_t algoritmoCIRC(char * claveN, char *valorN);
	rtdoEjec_t reemplazarEntrada(char * claveNuevo, char *valor);
	/////Aux
	void reemplazarEnTabla(t_entrada_tabla *entrada,char* claveN, char *valorN);
	void reemplazarEnAlmacenamiento(t_entrada_tabla *entrada, char *valorN);
	void  nueva_entrada( char *valor, int  pointer);

	//Store
	rtdoEjec_t storeRecurso(char * clave);


	//Dump & Compactar
	void compactar();
	int deboCompactar(char *valor);
	void dump(t_config * pConf);
	//OTRAS
	//Busqueda
	t_entrada_tabla * findEnTablaEntrada(char *claveN);
	void * findEnTablaLRU(char *claveN);
	int anyEnTabla(char *clave);
	//Manejo de Entradas y Tabla
	int cantidadDeEntradasNecesarias(char * valor);
	char * obtenerValor(char * clave);
	t_entrada_tabla * new_entrada_tabla(char * clave, int  pointerAEntrada, int sizeValor);
	rtdoEjec_t agregarATabla(char * clave, int pointerAEntrada, int sizeValor);
	//Aux
	void avisarCoordTamanioOcupado();



#endif
