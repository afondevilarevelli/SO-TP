#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tablaEntradas.h"

int procesarSentencia(int sentencia){ //no es como hallara la clave de una sentencia aun
	return sentencia +2;
}
void obtenerValorEnElAlmacenamiento(int clave){}
void enviarRespuestaAlCoordinador(){}

int main(){
	//leer la sentencia que el coordinador me envio,creo que es sokets,se guarda en undato tipo sentencia
	int sentencia = 6;//de arriba
	int clave = procesarSentencia(sentencia);// al procesarla me devuelve su clave

	elementoDeTabla* p = informacionDeLaTabla(clave);//tenemos los datos en la tabla

	obtenerValorEnElAlmacenamiento(p-> numEntradaAsociada);

	enviarRespuestaAlCoordinador();

return 0;
}
