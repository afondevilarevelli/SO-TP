#include <stdio.h>
#include <stdlib.h> // para malloc
#include <string.h> // cadenas
#include <unistd.h> // para close
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "tablaEntradas.h"
//#include "shared/mySocket.h"// libreria share
#define PUERTO 3490  //Puerto al que se conectaran los usuarios
#define CONECMAX 10  //Cuantas conexiones se van a mantener en cola
#define MAXTAMDATO 100 //maximo numro de bytes que se pueden leer

int main() {// yo soy INSTANCIA y ELCOORDINADOR me envia una sentecia
	char buf[MAXTAMDATO] ; //se guarda la sentencia que cordinador me envio
	struct sockaddr_in direccionCoordinador;//informacion sobre la direccion del coordinador
	struct sockaddr_in direccionInstancia;// informacion sobre mi direccion
	direccionInstancia.sin_family = AF_INET;//familia de direcciones AF_xxx,por defecto
	direccionInstancia.sin_addr.s_addr = INADDR_ANY;//direccion de internet IIIPPP
	direccionInstancia.sin_port = htons(PUERTO); // Número de puerto le puedo poner un #define PUERTO 8080

	int instancia = socket(AF_INET,SOCK_STREAM,0);//my soket, servidor,Soy INSTANCIA
	memset(&(direccionInstancia),'\0',8);// poner a cero el resto de la estructura

	int activado = 1;//estas dos lineas para corregir errores de espera del bind
	setsockopt(instancia,SOL_SOCKET, SO_REUSEADDR,&activado,sizeof(activado));

	if(bind(instancia,(void*)& direccionInstancia,sizeof(direccionInstancia)) != 0 ){
		perror("fallo el bind");// bin en que puerto estoy
		return 1;
	}
	printf("Estoy escuchando");
	listen(instancia,CONECMAX);
//quiero obtener tu soket pendiente, te devuelve el socket del que quiere conectarse en coordinador
	int sin_size = sizeof(struct sockaddr);

	int cordinador = accept(instancia,(struct sockaddr *)&direccionCoordinador,&sin_size);

	/*int numeroBytes=*/recv(cordinador,buf,MAXTAMDATO - 1,0) ; //se leyo y almaceno en buf
	int clave = procesarSentencia(buf);
	int num_entra = sacoNumEntradadeLaTabla(clave);
	char* respuesta = obtenerValorEnElAlmacenamiento(num_entra);
	send(cordinador, respuesta, sizeof(respuesta),0);// enviar l arespuesta al coordinador
	return 0;
}
