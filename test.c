#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/uio.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/queue.h>
#include <unistd.h>

#include "list.h"
#include "influxdb.h"
#include "config.h"

int measure_net();
int work(influx_client_t* c);

extern char param_addr[];
extern char param_interface[];
extern char param_db_name[];
#define BUFFER_LEN	1000
char buffer[BUFFER_LEN];


extern int param_port;

config_influx_client_t cnf;
/*
#define SERVER_ADDRESS_LEN      100;

struct _config_influx_client_t{
        char server_addr[SERVER_ADDRESS_LEN];
        uint16_t        port;
}config_influx_client_t;
*/

//int ctrl_opts (int argc, char **argv)

void post_netif_http(influx_client_t* c)
{
    tailq_entry_t* item;

    measure_net();
    while(!is_empty_queue()){
        item = pop();
//        printf("\n===========\n%s\n===========\n", item->payload->string);
//printf("1 %s %p\n", __func__, item);
        post_http_direct(c, item->payload->string);
        free(item->payload->string);
//printf("2 %s\n", __func__);
        free(item->payload);
//printf("3 %s\n", __func__);
       free(item);
//printf("4 %s\n", __func__);
    }
}


int main(int argc, char **argv)
{
    unsigned long long current_time;
    influx_client_t c;
    tailq_entry_t* item;

    if(0 != configure(&cnf)){
        return -1;
    }

    ctrl_opts (argc, argv);

    init_queue();

    current_time = time(NULL);
    current_time *= 1000000000;

//printf("---- %u000000000\n", (unsigned)time(NULL));
//printf("---- %llu\n", current_time);

//    c.host = strdup("192.168.1.230");
    if(!GET_INPUT_PARAM(AFLAG)){
        c.host = strdup(cnf.server_addr);
    } else {
        c.host = strdup(param_addr);
    }
    if(!GET_INPUT_PARAM(PFLAG)){
        c.port = cnf.port;
    } else {
        c.port = param_port;
    }

    if(!GET_INPUT_PARAM(DFLAG)){
        c.db = strdup(param_db_name);
        fprintf(stderr, "No database name specified. use -d option\n");
	return -1;
    } else {
        c.db = strdup(param_db_name);
    }

    c.usr = strdup("usr");
    c.pwd = strdup("pwd");



    printf("\nserver: %s:%d\n", cnf.server_addr, cnf.port);


    if(GET_INPUT_PARAM(CFLAG)){
        create_database(&c, param_db_name);
        return 0;
    }

    if(GET_INPUT_PARAM(SFLAG)){
	int len;
	char* cr;
	fgets(buffer, BUFFER_LEN, stdin);
	len =  strlen(buffer);
	cr = strchr(buffer, '\n');
	len = cr - buffer;
	buffer[len] = '\0';
        post_http_send_line(&c, buffer, strlen(buffer));
        return 0;
    }


    measure_net();
    while(!is_empty_queue()){
        item = pop();
//        printf("\n===========\n%s\n===========\n", item->payload->string);
        post_http_direct(&c, item->payload->string);
        free(item->payload->string);
        free(item->payload);
        free(item);
    }
    usleep(1000000);
//    measure_net();


#if 1
//    post_netif_http(&c);
    work(&c);

/*
    while(!is_empty_queue()){
        item = pop();
        printf("\n===========\n%s\n===========\n", item->payload->string);
        post_http_direct(&c, item->payload->string);
        free(item->payload->string);
        free(item->payload);
        free(item);
    }
*/
#else
    post_http(&c,
        INFLUX_MEAS("foo"),
        INFLUX_TAG("k", "v"),
        INFLUX_TAG("x", "y"),
        INFLUX_F_INT("x", 10),
        INFLUX_F_FLT("y", 10.3, 2),
        INFLUX_F_FLT("z", 10.3456, 2),
        INFLUX_F_BOL("b", 10),
//        INFLUX_TS(1512722735522840540),
        INFLUX_TS(current_time),
        INFLUX_END);
#endif

    return 0;
}
/*
  influx_client_t c = {strdup("127.0.0.1"), 8091, NULL, NULL, NULL};
  char *line = NULL;
  int len = 0;
  int used = 0;

  for (int i = 0; i < 10; ++i) {
      used = format_line(line, &len, used,
          INFLUX_MEAS("foo"),
          INFLUX_TAG("k", "v"),
          INFLUX_F_INT("x", i),
          INFLUX_END);
  }

  post_http_send_line(&c, line, used);
  ```

### TODO

- Add more test cases for send functions.
- Supports DSN initializatin for influx_client_t.
- Add query function.
- Do not need to connect every time.

### Misc

- Please feel free to use influxdb-c.
- Looking forward to your suggestions.
- If your project is using influxdb-c, you can show your project or company here by creating a issue or let me know.
*/
