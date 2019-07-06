#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <stdint.h>



#define IPADDRLEN (4*4 + 4)     /* length of IP address:  xxx.xxx.xxx.xxx  */


#define SET_INPUT_PARAM(x)   (input_param_status |= x)
#define CLEAR_INPUT_PARAM(x) (input_param_status &= ~x)
#define GET_INPUT_PARAM(x)   ((input_param_status & x) != 0)


#define AFLAG   (1 << 0)
#define BFLAG   (1 << 1)
#define CFLAG   (1 << 2)
#define DFLAG   (1 << 3)
#define IFLAG   (1 << 4)
#define PFLAG   (1 << 5)


#define SERVER_ADDRESS_LEN      100

typedef struct _config_influx_client_t{
        char server_addr[SERVER_ADDRESS_LEN];
        uint16_t        port;
}config_influx_client_t;

extern char param_db_name[];
extern uint32_t  input_param_status;

int configure(config_influx_client_t* cnf);
int ctrl_opts (int argc, char **argv);

#endif

