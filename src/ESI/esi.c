#include <stdio.h>
#include <parsi/parser.h>//parser
#include <stdlib.h> // Para malloc
#include <unistd.h> // Para close
#include <readline/readline.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/string.h>

#include "../shared/mySocket.h"

#define t_config "ESI.config";
//#define IP_C "127.0.0.1";
//#define IP_P "127.0.0.2";
//#define Puerto_C "8000";
//#define Puerto_P "8001";
int IP_C;
int IP_P;
int Puerto_C;
int Puerto_P;

int obtenerIP(char* arch_confi,char* key);
int obtenerPuerto(char* arch_confi,char* key);
void conexionA_Planificador(int ip, int port);
int enviarSentenciaParseada(int socket_servidor,t_esi_operacion lineaParseada);
int enviarESIResultadoDeEjecucion(int socket_servidor);
void recibirEjecurtarProximaSentenciaESI(int socket_coordinador, int socket_planificador);
void parsearSiguienteInstruccion(int socket_coordinador, int socket_planificador );
int main(){

	IP_C=obtenerIP("ESI.config","IP_Coordinador");// me da  IP de coordinador

	IP_P=obtenerIP ("archivo","IP_P,8");// me da  IP de planificador
	Puerto_C=obtenerPuerto("archivo","Puerto_C,4");// me da  Puerto de coordinador
	Puerto_P=obtenerPuerto("archivo","Puerto_P,4");// me da  Puerto de planificador*/
	conexionA_Coordinador(IP_C,Puerto_C);
	conexionA_Planificador(IP_P,Puerto_P);
	//conexiones(); No se pueden poner todas las conexiones en otra funcion?
	recibirEjecurtarProximaSentenciaESI( Puerto_C,Puerto_P);
return 0;
}
//buscar IP

int obtenerIP(char* arch_confi,char* key){
	char *a;
	t_config *p;
	p= config_create(arch_confi);
			if (config_has_property(p,key)){
			a=config_get_string_value(p, key);
			return inet_aton(a);//transformado de string a int por inet_aton
}


	return inet_aton(a);//transformado de string a int por inet_aton
}

int obtenerPuerto(char* arch_confi,char* key)
{
	//abrir archivo
	//leer archivo
	//recorrer el archivo hasta encontrar la palabra IP_C
	//tomar las 4 posiciones sgtes
	//resultado
}

//crear socket y conexion a P
void conexionA_Planificador(int ip, int port)
{
	int PlaniConect=connectTo( ip ,port);
}

//crear socket a c
//conect con C
//conect con P

int enviarSentenciaParseada(int socket_servidor,t_esi_operacion lineaParseada)
{
	int result;
	void * buffer[256];
	result = send(socket_servidor, lineaParseada, sizeof(lineaParseada), 0);

	if(result == -1){
		perror("error al enviar datos");
		exit(1);
	}

	result = recv(socket_servidor, buffer, sizeof(buffer), 0);
	close(socket_servidor);
    return result;
}


int enviarESIResultadoDeEjecucion(int socket_servidor, int respuesta)
{

	int result;
	void * buffer[256];
	result = send(socket_servidor, respuesta, sizeof(respuesta), 0);
	if(result == -1){
		perror("error al enviar datos");
		exit(1);
	}
	result = recv(socket_servidor, buffer, sizeof(buffer), 0);

	close(socket_servidor);

	return result;
}

void recibirEjecurtarProximaSentenciaESI(int socket_coordinador, int socket_planificador)
{
	int result;
	void * buffer[256];
	while( recv(socket_planificador, buffer, sizeof(buffer), 0)){
		parsearSiguienteInstruccion( socket_coordinador,  socket_planificador);
	}

	return;

}
void parsearSiguienteInstruccion(int socket_coordinador, int socket_planificador ){

	    FILE * fp;//de donde obtengo el file???
	    char * line = NULL;
	    int respuestaCoor;
	    size_t len = 0;
	    ssize_t read;

	    fp = fopen("escript.esi", "r");
	    if (fp == NULL){
	        perror("Error al abrir el archivo: ");
	        exit(EXIT_FAILURE);
	    }

	    while ((read = getline(&line, &len, fp)) != -1) {
	        t_esi_operacion lineaParseada = parse(line);

	        if(lineaParseada.valido){
	           //queremos probar que funciona----------------------------
	        	switch(lineaParseada.keyword){
	                case GET:
	                    printf("GET\tclave: <%s>\n", lineaParseada.argumentos.GET.clave);
	                    break;
	                case SET:
	                    printf("SET\tclave: <%s>\tvalor: <%s>\n", lineaParseada.argumentos.SET.clave, lineaParseada.argumentos.SET.valor);
	                    break;
	                case STORE:
	                    printf("STORE\tclave: <%s>\n", lineaParseada.argumentos.STORE.clave);
	                    break;
	                default:
	                    fprintf(stderr, "No pude interpretar <%s>\n", line);
	                    exit(EXIT_FAILURE);
	            }//-----------------------------------------------
	            //enviar al coordinador
	        	respuestaCoor= enviarSentenciaParseada(socket_coordinador, lineaParseada);
	            destruir_operacion(lineaParseada);
	            //respuesta del coordinador se manda al planificador
	        	enviarESIResultadoDeEjecucion(socket_planificador,respuestaCoor);
	        } else {
	            fprintf(stderr, "La linea <%s> no es valida\n", line);
	            exit(EXIT_FAILURE);
	        }
	    }

	    fclose(fp);
	    if (line)
	        free(line);

	    return;
	}
