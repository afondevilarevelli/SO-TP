#include "bibliotecaInstancia.h"

void mostrarEntrada(t_entrada_tabla * pEntry)
{
	printf("-------------------\n");
	printf("-------------------\r");
	printf("Puntero: %d\n", pEntry->pointerEntrada);
	printf("-------------------\n");
	printf("-------------------\r");
	printf("Clave: %s\n", pEntry->clave);
	printf("-------------------\n");
	printf("-------------------\r");
	printf("TamanioValor: %d\n", pEntry->tamanioValor);
	printf("-------------------\n");
	printf("-------------------\n");
	printf("-------------------\n\n");
}

//Basicas
rtdoEjec_t accederRecurso(op_t operacion, char * clave, char * valor)
{

	rtdoEjec_t rtdo;
	switch(operacion)
	{
		case GET:
			log_trace(pLog, "QUIERE HACER GET");
			rtdo=SUCCESS;
			break;

		case SET:
			rtdo = setRecurso(clave, valor);
			break;

		case STORE:
			rtdo = storeRecurso(clave);
			break;

		case ORDEN_COMPACTAR:
			compactar();
			break;

		default:
			log_error(pLog, "Se intento realizar una operacion invalida");
			break;
	}
	if (operacion!=COMPACTAR)
		actualizarRegistro(clave);

	list_iterate(tablaDeEntradas, (void*)&mostrarEntrada);

	return rtdo;
}
rtdoEjec_t storeRecurso(char * clave)
{//NO ANDA DESDE EL PLANIFICADOR ASIQUE NO SE
	//REAHCER
	log_trace(pLog, "1");
	FILE * f = fopen(strcat("*/PuntoDeMontaje/",clave), "w+");
	log_trace(pLog, "1");
	char*valor= obtenerValor(clave);
	log_trace(pLog, "1");
	int size = strlen(valor)+1;
	int written = fwrite(valor, size, 1, f);
	log_trace(pLog, "1");
	fclose(f);

	return (f != NULL && written != -1)?SUCCESS:FAILURE;
}
rtdoEjec_t setRecurso(char * clave, char * valor)
{
	rtdoEjec_t rtdo;
	t_entrada_tabla * pointerAlma;
	//averiguar si esta la clave para cambiarle el valor
	if(anyEnTabla(clave)){//    -----  1=true/0=false  ------
		t_entrada_tabla * repetido=findEnTablaEntrada(clave);

		if((repetido->tamanioValor+(entrySize%repetido->tamanioValor))>=strlen(valor))
		{
			int i;
			for(i=0;i=!strlen(valor);i++)
			{
				almacenamiento[repetido->pointerEntrada+i]=valor[i];
			}

			repetido->tamanioValor=strlen(valor);

			return SUCCESS;
		}
		else
		{
			log_trace(pLog, "Longitud de valor superior al valor de la entrada reservada");
			return FAILURE;
		}

	}
	rtdo = agregarEntrada(clave,valor);

	return rtdo;
}

void compactar()
{
	char *almNuevo = calloc(entryCant,entrySize);
	int pointer=0;
	t_entrada_tabla *entrada=malloc(sizeof(t_entrada_tabla));
	int i;
	for(i=0;list_size(tablaDeEntradas)!=i;i++)
	{
		entrada=list_get(tablaDeEntradas,i);
		char* valor=obtenerValor(entrada->clave);
		entrada->pointerEntrada=pointer;

		int j;
		for(j=0; strlen(valor)!= j; j++)
		{
			almNuevo[pointer]=valor[j];
			pointer++;
		}

		if((strlen(valor)% entrySize)!=0)
		{
			pointer=pointer+(entrySize%strlen(valor));//agrega al puntero la cantidad extra q falta para llegar al Entrycant.
		}
	}

	almacenamiento=almNuevo;
	//free(almNuevo);
	log_trace(pLog, "Compactacion Terminada");
	//free(entrada);
}

void actualizarRegistro (char * claveN)
{
	void clave_equal_put_zero( entrada_LRU  * pEntry)
		{
			if (string_equals_ignore_case(&pEntry->entrada->clave, claveN))
				pEntry->cant=0;
			else
				pEntry->cant++;
		}

	list_iterate(registroLRU, (void*)clave_equal_put_zero);
}

rtdoEjec_t agregarEntrada(char * clave, char *valor)
{
	rtdoEjec_t rtdo;
	static int yaCompacto=1;
	static int pointer=0;
	int cantEntradaNecesarias=strlen(valor)/entrySize;

	if(entrySize%strlen(valor))
		cantEntradaNecesarias++;

	if(entryCant*entrySize<=(pointer+cantEntradaNecesarias))
	{//Se termino la cantidad de lugar. el tamaño del almacenaiento debe volver atras
		if(yaCompacto)
		{
			log_trace(pLog, "Almacenamiento insuficiente. Compactacion");
			compactar();
			 yaCompacto=0;
			//HACER___Enviara acoordinador a q las otras Instancias compacten
			rtdo=agregarEntrada(clave,valor);
		}
		else
		{
			log_trace(pLog, "Almacenamiento insuficiente. Reemplazar ");
			rtdo=reemplazarEntrada(clave, valor);
			yaCompacto=1;
		}
	}
	else
	{
		rtdo =agregarATabla(clave, &pointer, strlen(valor));
		pointer= nueva_entrada(valor, pointer);
	}

	return rtdo;
}

rtdoEjec_t agregarATabla(char * clave, int *pointerAEntrada, int sizeValor)
{

	t_entrada_tabla* entradaTE =new_entrada_tabla(clave, pointerAEntrada, sizeValor);
	list_add (tablaDeEntradas, entradaTE);
	entrada_LRU * entrLRU=malloc(sizeof(entrada_LRU));
	entrLRU->entrada=  entradaTE;
	entrLRU->cant=0;
	list_add (registroLRU, entrLRU);

	return SUCCESS;
}

//Reemplazar
rtdoEjec_t algoritmoLRU(char * claveNew, char *valorNew)
{
	//rtdoEjec_t rtdo;
	int minimo=0;
	t_entrada_tabla entrada;
	t_entrada_tabla* old;
	//log_info(pLog, "AlfgroritmoReemplazo2");
	void search_min( entrada_LRU  * pEntry)
			{
				if(minimo>pEntry->cant)
				{
					minimo=pEntry->cant;
					old=pEntry->entrada;
				}
			}

	list_iterate(registroLRU, (void*)search_min);
	reemplazarEnAlmacenamiento( old,valorNew);
	reemplazarEnTabla( old,claveNew, valorNew);

	return SUCCESS;
}

rtdoEjec_t algoritmoBSU(char * claveN, char *valorN)
{
	rtdoEjec_t rtdo;
	t_entrada_tabla* entradaElegida=malloc(sizeof(t_entrada_tabla));
	t_entrada_tabla *entrada=malloc(sizeof(t_entrada_tabla));
	entradaElegida->tamanioValor=0;
	int seEncontroValor=0;
	int i;

	for(i=0;list_size(tablaDeEntradas)!=i;i++)
	{
		entrada=list_get(tablaDeEntradas,i);
		if(entrada->tamanioValor<=entrySize)
		{
			if(entrada->tamanioValor>entradaElegida->tamanioValor)
			{
				entradaElegida=entrada;
				seEncontroValor=1;
			}
		}

	}

	log_trace(pLog, "asadsasda---%d",seEncontroValor);

	if(seEncontroValor==1)
	{
		log_trace(pLog, "Se reemplazara clave=%s",entradaElegida->clave);
		reemplazarEnTabla(entradaElegida,claveN, valorN);
		reemplazarEnAlmacenamiento(entrada,valorN);
		rtdo= SUCCESS;
	}
	else
	{
		log_trace(pLog, "****ERROR**** No se encontro entrada con valor atomico");
		rtdo= FAILURE;
	}

	//free(entradaElegida);
	//free(entrada);

	return rtdo;
	//Biggest Space Used
}

rtdoEjec_t algoritmoCIRC(char * claveN, char *valorN)
{
	t_entrada_tabla *entrada=malloc(sizeof(t_entrada_tabla));;
	static int puntero=0;
	int i;

	for(i=0;list_size(tablaDeEntradas)!=0;i++)
	{
		entrada=list_get(tablaDeEntradas,i);

		if(entrada->tamanioValor<=entrySize)
		{
			//Cambia en entrada
			reemplazarEnTabla(entrada,claveN, valorN);
			//cambia almacenamiento
			reemplazarEnAlmacenamiento(entrada,valorN);
			puntero=entrada->pointerEntrada+entrySize;

			return SUCCESS;
		}
	}
	free(entrada);

	return FAILURE;
}


rtdoEjec_t reemplazarEntrada(char * claveNuevo, char *valorNuevo)
{
	rtdoEjec_t rtdo;
	if(strlen(valorNuevo)<=entrySize)
	{
	//Elige entrada segun el algoritmo del config
		switch(algReemp)
		{
			case CIRC:
				log_trace(pLog, "Algoritmo de Reemplazo Circular");
				rtdo=algoritmoCIRC(claveNuevo, valorNuevo);
				break;
			case LRU:
				log_trace(pLog, "Algoritmo de Reemplazo Last Recently Used");
				rtdo=algoritmoLRU( claveNuevo, valorNuevo);
				break;
			case BSU:
				log_trace(pLog, "Algoritmo de Reemplazo Biggest Space Used");
				rtdo=algoritmoBSU( claveNuevo, valorNuevo);
				break;

			default:
				log_trace(pLog, "****ERROR**** El algoritmo de remplazo no existe");
				break;
		}

	}
	else
	{
		log_trace(pLog, "****ERROR**** entrada con valor no atomico");
		rtdo=FAILURE;
	}
	//Avisar al coordinador?
	return rtdo;
}
void reemplazarEnTabla(t_entrada_tabla *entrada,char* claveN, char *valorN)
{
	strcpy(entrada->clave, claveN);
	entrada->tamanioValor=strlen(valorN);
}
void reemplazarEnAlmacenamiento(t_entrada_tabla *entrada, char *valorN)
{ //TO DO
	int ponter=entrada->pointerEntrada;
	int j;
	for(j=0;strlen(valorN)!=j;j++)
	{
		almacenamiento[ponter+j]=valorN[j];
	}
}

//Busqueda
t_entrada_tabla * findEnTablaEntrada(char *claveN)
{
	int clave_equal( t_entrada_tabla  * pEntry)
	{
		return string_equals_ignore_case(&pEntry->clave, claveN);
	}

	return list_find(tablaDeEntradas, (void*)clave_equal);
}

void * findEnTablaLRU(char *claveN)
{
	int clave_equal( entrada_LRU  * pEntry)
	{
		return string_equals_ignore_case(&pEntry->entrada->clave, claveN);
	}

	return list_find(registroLRU, (void*)clave_equal);
}

int anyEnTabla(char *clave)
{
	int clave_equals( t_entrada_tabla * pEntry)
	{
		return string_equals_ignore_case(&pEntry->clave, clave);
	}

	return list_any_satisfy(tablaDeEntradas, (void*)clave_equals);
}

char * obtenerValor(char * clave)
{
	t_entrada_tabla * pEntry = findEnTablaEntrada(clave);
	char val[pEntry->tamanioValor];
	char * valor=val;
	int pointer=pEntry->pointerEntrada;
	int i;

	for(i =0;i!=pEntry->tamanioValor;i++)
	{
		val[i]=almacenamiento[pointer];
		pointer++;
	}

	return valor;
}

//Creacion?
t_entrada_tabla * new_entrada_tabla(char * clave, int * pointerAEntrada, int sizeValor)
{
	t_entrada_tabla * pEntry = malloc(sizeof(t_entrada_tabla));
	strcpy(pEntry->clave, clave);
	pEntry->tamanioValor=sizeValor;
	pEntry->pointerEntrada=*pointerAEntrada;

	return pEntry;
}

int nueva_entrada(char *valor, int  pointer){

	//que agregue a almacenamiento
	int i;
	for(i=0; strlen(valor)!=i;i++)
	{
		almacenamiento[pointer]=valor[i];
		pointer++;
	}

	if((strlen(valor)% entrySize)!=0)
	{
		pointer=pointer+(entrySize%strlen(valor));//agrega al puntero la cantidad extra q falta para llegar al Entrycant.
	}

	return pointer;
}

void cargarTablaDeEntradasYAlmacenamiento(t_config * pConf)
{
	almacenamiento = calloc(entryCant,entrySize);

	tablaDeEntradas = list_create();
	registroLRU = list_create();

	//HAY TABLA A CARGAR? CARGARLA O CREARLA
		//agregarATabla(CLAVEL, POINTER A LA PRIEMRA ENTRADA, TAMAÑO DEL VALOR )
		//agregarATabla("",NULL, 0 )

	return;
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
