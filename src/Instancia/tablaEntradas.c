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

