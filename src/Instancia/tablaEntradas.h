#ifndef TABLAENTRADAS_H_
#define TABLAENTRADAS_H_

typedef struct {
	int clave;
	int numEntradaAsociada;
	int tamanio;
}elementoDeTabla ;

elementoDeTabla* informacionDeLaTabla(int _clave);
int procesarSentencia(char* sentencia);

int sacoNumEntradadeLaTabla(int clave);

char* obtenerValorEnElAlmacenamiento(int numEntradaAsociada);

#endif /* TABLAENTRADAS_H_ */
