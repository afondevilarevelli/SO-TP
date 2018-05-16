#include <stdio.h>
#include <string.h>
#include <commons/string.h>
#include <commons/config.h>

//#define t_config "ESI.config";

char *IP_C;
char *IP_P;

//buscar IP
char *obtenerIP(char* arch_confi,char* key){
	char *a;t_config *p;
	p= config_create(arch_confi);
			if (config_has_property(p,key))
			a=config_get_string_value(p, key);
			return a;//transformado de string con puntos a int por inet_addr

}
int puerto_C;
int puerto_P;
//buscar puerto
int obtenerPuerto(char* arch_confi,char* key){
	int b;t_config *q;
	q= config_create(arch_confi);
	if (config_has_property(q,key))
		b=config_get_int_value(q,key);
	return b;}

int main(){

	IP_C=obtenerIP("ESI.config","IP_Coordinador");// me da  IP de coordinador
	printf("El IP del coordinador es: %s\n\n",IP_C);

	puerto_C=obtenerPuerto("ESI.config","Puerto_Coordinador");// me da Puerto de coordinador
	printf("El Puerto del coordinador es: %d\n\n",puerto_C);

	IP_P=obtenerIP("ESI.config","IP_Planificador");// me da  IP de coordinador
		printf("El IP del coordinador es: %s\n\n",IP_P);

    puerto_P=obtenerPuerto("ESI.config","Puerto_Planificador");// me da Puerto de coordinador
	printf("El Puerto del coordinador es: %d\n\n",puerto_P);

return 0;
}


