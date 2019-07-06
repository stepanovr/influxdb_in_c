#ifndef __INFLUXDB_H__
#define __INFLUXDB_H__

typedef struct _influx_client_t
{
    char* host;
    int   port;
    char* db;  // http only
    char* usr; // http only [optional for auth]
    char* pwd; // http only [optional for auth]
} influx_client_t;

int post_http_direct(influx_client_t* c, char *line);
int send_udp(influx_client_t* c, ...);
int create_database(influx_client_t *c, char *name_db);


void post_netif_http(influx_client_t* c);


#endif

