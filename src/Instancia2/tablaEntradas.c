#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tablaEntradas.h"

elementoDeTabla**tabla;//lista de elementos de tabla,malloc?

elementoDeTabla* informacionDeLaTabla(int _clave){
	int i=0;
	while(tabla[i]-> clave != _clave){
		i++;
	}
	return tabla[i];//retorna el puntero en el que esta alojado ltodos los datos de la clave
}
int procesarSentencia(char* sentencia){ //no es como hallara la clave de una sentencia aun
	//sacara la clave
	return 3;
}
int sacoNumEntradadeLaTabla(int clave){
	elementoDeTabla* p = informacionDeLaTabla(clave);
	return p-> numEntradaAsociada;
}
char* obtenerValorEnElAlmacenamiento(int numEntradaAsociada){
	//sacara el valor alojado en el almacenamiento le puse char poque no se su estructura
	// obtendra ese valor y generara una respuesta en base a ese valor
	return "valor del almacenamientor";
}
