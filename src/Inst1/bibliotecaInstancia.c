#include "bibliotecaInstancia.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>


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
			avisarCoordTamanioOcupado();
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
	//Ver valores en el almacenamietno
//	int i;
//	for(i=0;entrySize*entryCant!=i;i++){
//		printf("-------------------------------\r");
//	printf("... %c ...\n", almacenamiento[i]);
//	}

	return rtdo;
}

rtdoEjec_t setRecurso(char * clave, char * valor)
{
	rtdoEjec_t rtdo;
	int cantEntradasOriginal,cantEntradasNuevo;
	//averiguar si esta la clave para cambiarle el valor
	if(anyEnTabla(clave)){//    -----  1=true/0=false  ------

		t_entrada_tabla * original=findEnTablaEntrada(clave);
		cantEntradasOriginal=cantidadDeEntradasNecesarias(obtenerValor(original->clave));

		if(cantEntradasOriginal*entrySize>=strlen(valor))
		{
			cantEntradasNuevo=cantidadDeEntradasNecesarias(valor);

			int poiterDelBit=original->pointerEntrada/entrySize;
			int j=cantEntradasOriginal;
			while(cantEntradasNuevo!=j)
			{
				j--;
				//log_trace(pLog, "elimino uno=%d",poiterDelBit+j);
				bitarray_clean_bit(bitarray,poiterDelBit+j);
				}

			int i;
			for(i=0;strlen(valor)!=i;i++)
			{
				almacenamiento[original->pointerEntrada+i]=valor[i];
			}

			original->tamanioValor=strlen(valor);

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


int cantidadDeEntradasNecesarias(char * valor)
{
	int cantEntradaNecesarias;

	cantEntradaNecesarias=strlen(valor)/entrySize;
			if(strlen(valor)%entrySize!=0)
			{
				cantEntradaNecesarias++;
			}
	return cantEntradaNecesarias;
}


void compactar()
{
	char *almNuevo = calloc((entryCant*entrySize),sizeof(char));
	int pointer=0,cantNecesarias;
	t_entrada_tabla *entrada;
	char *valor;
	int i,j,k;
	//Bitmap en 0
	for(i=0;entryCant!=i;i++)
	{
		bitarray_clean_bit(bitarray,i);
	}

	//pasa por toda la tabla actual
	for(i=0;list_size(tablaDeEntradas)!=i;i++)
	{
		//obtiene de a uno y su valor
		entrada=list_get(tablaDeEntradas,i);
		valor=obtenerValor(entrada->clave);

			//setea el bit Array
		cantNecesarias=cantidadDeEntradasNecesarias(valor);

		for(k=0;cantNecesarias!=k;k++)
		{
			bitarray_set_bit(bitarray,((pointer/entrySize)+k));
		}

					//Agrega los valores
		entrada->pointerEntrada=pointer;

		int aux=pointer;
		for(j=0; strlen(valor)!= j; j++)
		{
			almNuevo[aux]=valor[j];
			aux++;
		}

		pointer=pointer+(cantNecesarias*entrySize);

	}

	//Ver valores en el almacenamietno despues de compactar
//	int l;
//		for(l=0;entrySize*entryCant!=l;l++){
//			printf("-------------------------------\r");
//		printf("... %c ...\n", almNuevo[l]);
//		}
//	list_iterate(tablaDeEntradas, (void*)&mostrarEntrada);


	free(almacenamiento);
	almacenamiento=almNuevo;
	log_debug(pLog, "Compactacion Terminada");

}

void actualizarRegistro (char * claveN)
{
	void clave_equal_put_zero( entrada_LRU  * pEntry)
		{
			if (string_equals_ignore_case(pEntry->entrada->clave, claveN))
				pEntry->cant=0;
			else
				pEntry->cant++;
		}

	list_iterate(registroLRU, (void*)clave_equal_put_zero);
}


int punteroLugarDisponible(char *valor){

	int cantEntradaNecesarias=cantidadDeEntradasNecesarias(valor);
	int i,cantAux=cantEntradaNecesarias;

	//busca en todo el almacenamiento
	for(i=0;entryCant!=i;i++)
	{
		//busca q este seguido
		if(bitarray_test_bit(bitarray,i)==0)
		{
			cantAux--;
		}
		else
		{
			cantAux=cantEntradaNecesarias;
		}

		if(cantAux==0)
		{
			if(i-cantEntradaNecesarias==-1)
			return 0;

			else if(cantEntradaNecesarias==1)
				return i;

			else
				return i-cantEntradaNecesarias+1;
		}
	}
	return -1;
}

int deboCompactar(char *valor){
	int cantNecesarias=cantidadDeEntradasNecesarias(valor);
	int i,cantAux=0;
	//busca en todo el almacenamiento
		for(i=0;entryCant!=i;i++)
		{
			if(bitarray_test_bit(bitarray,i)==0)
			{
				cantAux++;
			}
		}
		if(cantAux>=cantNecesarias)
			return 0;

		log_trace(pLog, "Cantidad insuficiente de espacios libres. no se compactara.");
		return 1;
}
rtdoEjec_t agregarEntrada(char * clave, char *valor)
{
	rtdoEjec_t rtdo;
	static int yaCompacto=0;
	int puntero, compacto;

	puntero= punteroLugarDisponible(valor);

	//No se encntro lugar
	if(puntero==-1)
	{
		log_trace(pLog, "Entro");
		if(yaCompacto==0)
		{
			log_trace(pLog, "Almacenamiento insuficiente. Compactacion");
			compacto=deboCompactar(valor);
			if(compacto==0)
			{
			//Se dice al Coordinador qe todas deben compactar
//			rtdoEjec_t orden=ORDEN_COMPACTAR;
//			sendWithBasicProtocol(coord_socket, &orden, sizeof(rtdoEjec_t));
//			log_trace(pLog, "Resultado enviado al Coordinador. COOMPACTAR");

			compactar();
			yaCompacto=1;
			rtdo=agregarEntrada( clave,valor);
			}
		}
		 if(yaCompacto==0||compacto==1)
		{
			log_trace(pLog, "Almacenamiento insuficiente. Reemplazar ");
			rtdo=reemplazarEntrada(clave, valor);
			yaCompacto=0;
		}
	}
	else
	{
		rtdo =agregarATabla(clave, puntero*entrySize, strlen(valor));
		nueva_entrada(valor, puntero*entrySize);
	}
	return rtdo;
}

rtdoEjec_t agregarATabla(char * clave, int pointerAEntrada, int sizeValor)
{

	t_entrada_tabla* entradaTE =new_entrada_tabla(clave, pointerAEntrada, sizeValor);
	list_add (tablaDeEntradas, entradaTE);
	entrada_LRU * entrLRU=malloc(sizeof(entrada_LRU));
	entrLRU->entrada=entradaTE;
	entrLRU->cant=0;
	list_add (registroLRU, entrLRU);

	return SUCCESS;
}


void dump(t_config * pConf)
{
//	int msec = config_get_int_value(pConf, "DUMP_INTERVAL");
//	int usec = msec*1000;
//	t_entrada_tabla *entrada=malloc(sizeof(t_entrada_tabla));
//	while(1)
//	{
//		usleep(usec);
//		int i;
//		for(i = 0; list_size(tablaDeEntradas)!=i; i++)
//		{
//			entrada=list_get(tablaDeEntradas,i);
//			storeRecurso(entrada->clave);
//		}
//	}
//	free(entrada);
}

rtdoEjec_t storeRecurso(char * clave)
 {

	if(anyEnTabla(clave)){
		FILE *fp;
		//Obtencion del valor
		char* val = obtenerValor(clave);

		//Extencion completa del archivo
		char * dir = malloc(strlen(pathMontaje)+1+strlen(clave)+1);
		sprintf(dir, "%s/%s", pathMontaje, clave);

		struct stat st = {0};

		if (stat(pathMontaje, &st) == -1)
				mkdir(pathMontaje, 0700);

		fp=fopen(dir,"w");

		fwrite(val,sizeof(char),strlen(val)+1,fp);
		log_debug(pLog, "Se escribio %s en el archivo %s", val, clave);

		fclose(fp);
		//Archivos guardados
		return SUCCESS;
	}else
	{
		//log_trace(pLog, "Clave inexistente");
		return FAILURE;
	}

 }


//Reemplazar
rtdoEjec_t algoritmoLRU(char * claveNew, char *valorNew)
{
	int max=0;
	t_entrada_tabla* old;

	void search_max( entrada_LRU  * pEntry)
		{
				//que sea menor al maximo y q sea atomica
			if(max<pEntry->cant&&pEntry->entrada->tamanioValor<entrySize)
			{
				max=pEntry->cant;
				old=pEntry->entrada;
			}
		}
	list_iterate(registroLRU, (void*)search_max);

	log_trace(pLog, "Se reemplazara clave=%s",old->clave);
	reemplazarEnAlmacenamiento( old,valorNew);
	reemplazarEnTabla( old,claveNew, valorNew);

	return SUCCESS;
}

rtdoEjec_t algoritmoBSU(char * claveN, char *valorN)
{
	rtdoEjec_t rtdo;
	t_entrada_tabla* entradaElegida;
	t_entrada_tabla *entrada;
	entradaElegida->tamanioValor=0;
	int seEncontroValor=0;
	int i;

	for(i=0;list_size(tablaDeEntradas)!=i;i++)
	{
		entrada=list_get(tablaDeEntradas,i);
		if(entrada->tamanioValor<entrySize)
		{
			if(entrada->tamanioValor>entradaElegida->tamanioValor)
			{
				entradaElegida=entrada;
				seEncontroValor=1;
			}
		}
	}

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


	return rtdo;
	//Biggest Space Used
}

rtdoEjec_t algoritmoCIRC(char * claveN, char *valorN)
{
	rtdoEjec_t rtdo;
	t_entrada_tabla *entrada;
	static int puntero=0;
	int seEncontroValor=0;

	int i;
	for(i=0;list_size(tablaDeEntradas)!=0;i++)
	{
		entrada=list_get(tablaDeEntradas,puntero);
		if(entrada->tamanioValor<=entrySize)
		{
			seEncontroValor=1;
			puntero++;
			break;
		}
		else
		{
			if(list_size(tablaDeEntradas)==puntero)
				puntero=0;
			else
				puntero++;
		}
	}
	if(seEncontroValor==1)
	{
		log_trace(pLog, "Se reemplazara clave=%s",entrada->clave);
		reemplazarEnTabla(entrada,claveN, valorN);
		reemplazarEnAlmacenamiento(entrada,valorN);
		rtdo= SUCCESS;
	}
	else
	{
		log_trace(pLog, "****ERROR**** No se encontro entrada con valor atomico");
		rtdo= FAILURE;
	}

	return rtdo;
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
{
	int k;
	for(k=0;strlen(valorN)!=k;k++)
	{
		almacenamiento[entrada->pointerEntrada+k]=valorN[k];
	}
}

//Busqueda
t_entrada_tabla * findEnTablaEntrada(char *claveN)
{
	int clave_equal( t_entrada_tabla  * pEntry)
	{
		return string_equals_ignore_case(pEntry->clave, claveN);
	}

	return list_find(tablaDeEntradas, (void*)clave_equal);
}

void * findEnTablaLRU(char *claveN)
{
	int clave_equal( entrada_LRU  * pEntry)
	{
		return string_equals_ignore_case(pEntry->entrada->clave, claveN);
	}

	return list_find(registroLRU, (void*)clave_equal);
}

int anyEnTabla(char *clave)
{
	int clave_equals( t_entrada_tabla * pEntry)
	{
		return string_equals_ignore_case(pEntry->clave, clave);
	}

	return list_any_satisfy(tablaDeEntradas, (void*)clave_equals);
}

char * obtenerValor(char * clave)
{
	t_entrada_tabla * pEntry = findEnTablaEntrada(clave);
	char *val=calloc(pEntry->tamanioValor,sizeof(char));
	int pointer=pEntry->pointerEntrada;
	int i;
	for(i =0;pEntry->tamanioValor!=i;i++)
	{
		val[i]=almacenamiento[pointer];
		pointer++;
	}
	return val;
}

//Creacion?
t_entrada_tabla * new_entrada_tabla(char * clave, int  pointerAEntrada, int sizeValor)
{
	t_entrada_tabla * pEntry = malloc(sizeof(t_entrada_tabla));
	strcpy(pEntry->clave, clave);
	pEntry->tamanioValor=sizeValor;
	pEntry->pointerEntrada=pointerAEntrada;
	return pEntry;
}

void nueva_entrada(char *valor, int  pointer){

	//que agregue a almacenamiento
	int cantEntradaNecesarias=cantidadDeEntradasNecesarias(valor);

	int i;
	for(i=0;cantEntradaNecesarias!=i;i++)
	{
		bitarray_set_bit(bitarray,(pointer/entrySize)+i);
	}

	for(i=0; strlen(valor)!=i;i++)
	{
		almacenamiento[pointer]=valor[i];
		pointer++;
	}

	return;
}

void cargarTablaDeEntradasYAlmacenamiento(t_config * pConf)
{
	almacenamiento = calloc((entryCant*entrySize),sizeof(char));
	char * datoBitArray=calloc(entryCant,sizeof(char));
	bitarray = bitarray_create_with_mode(datoBitArray,entryCant,LSB_FIRST);
	tablaDeEntradas = list_create();
	registroLRU = list_create();
	//restaurar();

	return;
}
void restaurar()
{

	struct dirent* directorio;
	struct stat buf;
	char fichero [PATH_MAX];
	char valor[100];

	DIR* dirp = opendir (pathMontaje);
	if (dirp == NULL)
	{
		perror (pathMontaje);
		return;
	}

	while ((directorio = readdir (dirp)) != NULL )
	{
		printf("%s\n",directorio->d_name);
		//Obtengo la clave
		sprintf (fichero, "%s/%s", pathMontaje, directorio->d_name);
		if (lstat (fichero, & buf) == -1)
		{
			perror(fichero);
			exit(1);
		}
		//Obtengo el valor. Entro al archivo con esa clave
		FILE *arch;
		arch=fopen(fichero,"r");

		//Obtengo el valor
		fgets(valor,100,arch);
		log_debug(pLog, "La clave es %s  y el valor es %s", directorio->d_name, valor);
		fclose(arch);
		//Agregar entrada con los datos
		rtdoEjec_t rt = setRecurso(directorio->d_name, valor);
		if(rt==SUCCESS)
			log_debug(pLog, "Se agrego la entrada con exito.");
		else
			log_debug(pLog, "No se ha logrado cargar la entrada.");

	}
	//cierra el directorio
	closedir (dirp);
	return;
}

void avisarCoordTamanioOcupado(){
	int tamanio = 0;
	int i;
	for(i=0;entryCant!=i;i++)
	{
		if(bitarray_test_bit(bitarray,i)==1)
			tamanio++;
	}
	sendWithBasicProtocol(coord_socket, &tamanio, sizeof(int));
	log_trace(pLog, "Resultado de tamanio ocupado enviado");

}
int conectarseACoordinador(t_config * pConf)
{
	int ip, puerto;
	obtenerIPyPuertoDeCoordinador(pConf, &ip, &puerto);
	 coord_socket = connectTo(ip, puerto);
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
	*puerto= htons(config_get_int_value(pConf, "COORD_PUERTO"));
}
