#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"

/*
#define SERVER_ADDRESS_LEN	100;

struct _config_influx_client_t{
	char server_addr[SERVER_ADDRESS_LEN];
	uint16_t	port;
}config_influx_client_t;
*/

int configure(config_influx_client_t* cnf)
{
        char* s = getenv("INFDB_IP");
	if(s == NULL){
		printf("Address INFDB_IP is not defined\n");
		return -1;
	}

	if(strlen(s) > SERVER_ADDRESS_LEN){
		printf("Address is longer than: %d\n", SERVER_ADDRESS_LEN);
		return -1;
	}
	strcpy(cnf->server_addr, s);
        printf("IP Address: %s\n", cnf->server_addr);


        s = getenv("INFDB_PORT");
        if(!s){
                printf("Port INFDB_PORT is not defined\n");
                return -1;
        }
	cnf->port = atoi(s);

        printf("port: %d\n", cnf->port);
        return 0;
}
