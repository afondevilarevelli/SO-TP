#include <stdio.h>
#include <string.h>
#include <stdlib.h> // Para malloc
#include <unistd.h> // Para close
#include <readline/readline.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <sys/socket.h>
#include "shared/mySocket.h"

//#define t_config "ESI.config";

int IP_C;

int main(){

	IP_C=obtenerIP("ESI.config","IP_Coordinador");// me da  IP de coordinador
	printf("%d",IP_C);
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
