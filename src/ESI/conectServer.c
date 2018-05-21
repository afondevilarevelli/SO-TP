#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h> // Para close
#include <sys/socket.h>
#include <commons/string.h>
#include <commons/config.h>
#include <arpa/inet.h>
#include "shared/mySocket.h"

int IP_P=2130706434; //127.0.0.2
int puerto_P=8001;

int IP_C=2130706433;// 127.0.0.1
int puerto_C=8000;

void connectServ(int ip,int puerto){
	int esi3 = connectTo(ip , puerto);

	  while(1){

	   char mensaje[1000];
        scanf("%s",mensaje);
        send(esi3,mensaje,strlen(mensaje),0);
                }
                                    }
int main(){

 	connectServ(IP_P,puerto_P);

	connectServ(IP_C , puerto_C);
	//no se conectan al mismo tiempo
return 0;
           }
