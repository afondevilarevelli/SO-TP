#include <stdio.h>
#include <parsi/parser.h>//parser
#include <stdlib.h> // Para malloc
#include <unistd.h> // Para close
#include <readline/readline.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/string.h>

#include "../shared/mySocket.h"

// IP_C="127.0.0.1";2130706434
// IP_P="127.0.0.2";2130706433
// Puerto_C="8000";
// Puerto_P="8001";
int IP_C;
int IP_P;
int Puerto_C;
int Puerto_P;

int obtenerIP(char* arch_confi,char* key);
int obtenerPuerto(char* arch_confi,char* key);

void connectServ(int ip, int port);

int enviarSentenciaParseada(int socket_servidor,t_esi_operacion lineaParseada);
int enviarESIResultadoDeEjecucion(int socket_servidor, int respuesta);
void recibirEjecurtarProximaSentenciaESI(int socket_coordinador, int socket_planificador);
void parsearSiguienteInstruccion(int socket_coordinador, int socket_planificador );
int main(){

	IP_C=obtenerIP("ESI.config","IP_Coordinador");// me da  IP de coordinador
	puerto_C=obtenerPuerto("ESI.config","Puerto_Coordinador");// me da Puerto de coordinador
	IP_P=obtenerIP ("archivo","IP_P,8");// me da  IP de planificador	
	puerto_P=obtenerPuerto("ESI.config","Puerto_Planificador");// me da Puerto de planificador
	
	connectServ(IP_P,puerto_P);
	connectServ(IP_C , puerto_C);
	//conexiones(); No se pueden poner todas las conexiones en otra funcion?
	//falla : no puedo conectarme al mismo tiempo con ambos
	recibirEjecurtarProximaSentenciaESI( puerto_C,puerto_P);
return 0;
}
//buscar IP
int obtenerIP(char* arch_confi,char* key){
	char *a;
	t_config *p;
	p= config_create(arch_confi);
			if (config_has_property(p,key))
			a=config_get_string_value(p, key);
	return inet_aton(a);//transformado de string a int por inet_aton
                                          }

//buscar puerto
int obtenerPuerto(char* arch_confi,char* key){
	int b;
	t_config *q;
	q= config_create(arch_confi);
	        if (config_has_property(q,key))
		b=config_get_int_value(q,key);
	return b;
                                             }

//funcion conexion e intercambio de mensajes
void connectServ(int ip,int puerto){
	int esi3 = connectTo(ip , puerto);

	  while(1){

	   char mensaje[1000];
        scanf("%s",mensaje);
        send(esi3,mensaje,strlen(mensaje),0);
                }
                                    }
int main(){
int enviarSentenciaParseada(int socket_servidor, t_esi_operacion lineaParseada)
{
	int result;
	char resultadoDeEjecucion;
	void * buffer[256];
	result = send(socket_servidor, lineaParseada, sizeof(lineaParseada), 0);

	if(result == -1){
		perror("error al enviar datos");
		exit(1);
	}

	resultadoDeEjecucion = recv(socket_servidor, buffer, sizeof(buffer), 0);
	close(socket_servidor);
    return resultadoDeEjecucion;
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
