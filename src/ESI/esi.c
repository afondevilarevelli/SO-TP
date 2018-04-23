#include <stdio.h>
#include <string.h>
#include <stdlib.h> // Para malloc
#include <unistd.h> // Para close
#include <readline/readline.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/string.h>

#include "shared/mySocket.h"

#define t_config "ESI.config";
//#define IP_C "127.0.0.1";
//#define IP_P "127.0.0.2";
//#define Puerto_C "8000";
//#define Puerto_P "8001";
int IP_C;
int IP_P;
int Puerto_C;
int Puerto_P;
int main(){
	IP_C=obtenerIP("ESI.config","IP_Coordinador");// me da  IP de coordinador

	IP_P=obtenerIP (archivo,IP_P,8);// me da  IP de planificador
	Puerto_C=obtenerPuerto(archivo,Puerto_C,4)// me da  Puerto de coordinador
	Puerto_P=obtenerPuerto(archivo,Puerto_P,4)// me da  Puerto de planificador*/
	conexionA_Coordinador(IP_C,Puerto_C);
	conexionA_Planificador();
return 0;
}
//buscar IP
int obtenerIP(char* arch_confi,char* key){
	char *a;t_config *p;
	p= config_create(arch_confi);
			if (config_has_property(p,key))
			a=config_get_string_value(p, key);
			return inet_aton(a);//transformado de string a int por inet_aton
}


	int obtenerPuerto(archivo,buscar palabaclave){
		//abrir archivo
	    //leer archivo
	    //recorrer el archivo hasta encontrar la palabra IP_C
		//tomar las 4 posiciones sgtes
		//resul
	}
	//crear socket y conexion a P
	void conexionA_Planificador(int ip, int port){
	struct sockaddr_in direccionPlanificador;
	setAddrStruct (direccionPlanificador,ip,port);

	int PlaniConect=connectTo( ip ,port);
	if(PlaniConect == -1){
			perror("error al conectar con coordinador");
			exit(1);
		}
	close(direccionPlanificador);

	}
	//crear socket a c
	//conect con C
	//conect con P
