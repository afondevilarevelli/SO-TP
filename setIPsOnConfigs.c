/*
 * Find local ip used as source ip in ip packets.
 * Read the /proc/net/route file
 */

#include<stdio.h> //printf
#include<string.h>    //memset
#include<errno.h> //errno
#include<sys/socket.h>
#include<netdb.h>
#include<ifaddrs.h>
#include<stdlib.h>
#include<unistd.h>
#include<commons/config.h>

int main ( int argc , char *argv[] )
{
  FILE *f;
char line[100] , *p , *c;

f = fopen("/proc/net/route" , "r");

while(fgets(line , 100 , f))
{
    p = strtok(line , " \t");
    c = strtok(NULL , " \t");

    if(p!=NULL && c!=NULL)
    {
        if(strcmp(c , "00000000") == 0)
        {
            //printf("Default interface is : %s \n" , p);
            break;
        }
    }
}

    //which family do we require , AF_INET or AF_INET6
    int fm = AF_INET;
    struct ifaddrs *ifaddr, *ifa;
    int family , s;
    char host[NI_MAXHOST];

    if (getifaddrs(&ifaddr) == -1)
    {
        perror("getifaddrs");
        exit(EXIT_FAILURE);
    }

    //Walk through linked list, maintaining head pointer so we can free list later
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
    {
        if (ifa->ifa_addr == NULL)
        {
            continue;
        }

        family = ifa->ifa_addr->sa_family;

        if(strcmp( ifa->ifa_name , p) == 0)
        {
            if (family == fm)
            {
                s = getnameinfo( ifa->ifa_addr, (family == AF_INET) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6) , host , NI_MAXHOST , NULL , 0 , NI_NUMERICHOST);

                if (s != 0)
                {
                    printf("getnameinfo() failed: %s\n", gai_strerror(s));
                    exit(EXIT_FAILURE);
                }

            }
        }
    }

    freeifaddrs(ifaddr);
    //printf("address: %s\n", host);
    t_config * pConf;
    char * configName;
    FILE * confFile;
    configName = malloc(strlen(argv[1]) + strlen(".config")+1);
    sprintf(configName, "%s.config", argv[1]);
    confFile = fopen(configName, "r+");
    fclose(confFile);
    pConf = config_create(configName);

    if(config_has_property(pConf, "COORD_IP"))
      if(argc >= 3)
        config_set_value(pConf, "COORD_IP", argv[2]);
      else
        config_set_value(pConf, "COORD_IP", host);

    if(config_has_property(pConf, "COORD_PUERTO"))
      if(argc >= 4)
        config_set_value(pConf, "COORD_PUERTO", argv[3]);

    if(config_has_property(pConf, "PLANIF_IP"))
      if(argc >= 5)
        config_set_value(pConf, "PLANIF_IP", argv[4]);
      else
        config_set_value(pConf, "PLANIF_IP", host);

    if(config_has_property(pConf, "PLANIF_PUERTO"))
      if(argc == 6)
        config_set_value(pConf, "PLANIF_PUERTO", argv[3]);

    config_save(pConf);
    config_destroy(pConf);
    free(configName);

    return 0;
}
