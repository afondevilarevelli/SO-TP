#ifndef TABLAENTRADAS_H_
#define TABLAENTRADAS_H_

typedef struct {
	int clave;
	int numEntradaAsociada;
	int tamanio;
}elementoDeTabla ;


elementoDeTabla* informacionDeLaTabla(int _clave);


#endif /* TABLAENTRADAS_H_ */
