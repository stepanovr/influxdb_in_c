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

#include "influxdb.h"

int create_database(influx_client_t *c, char *name_db);

int post_http_send_line(influx_client_t *c, char *buf, int len)
{
    int sock, ret_code = 0, content_length = 0;
    struct sockaddr_in addr;
    struct iovec iv[2];
    char ch;

    iv[1].iov_base = buf;
    iv[1].iov_len = len;

    if(!(iv[0].iov_base = (char*)malloc(len = 0x100))) {
        return -2;
    }
    for(;;) {
        iv[0].iov_len = snprintf((char*)iv[0].iov_base, len, "POST /write?db=%s&u=%s&p=%s HTTP/1.1\r\nHost: %s\r\nContent-Length: %zd\r\n\r\n",
            c->db, c->usr ? c->usr : "", c->pwd ? c->pwd : "", c->host, iv[1].iov_len);
        if((int)iv[0].iov_len >= len && !(iv[0].iov_base = (char*)realloc(iv[0].iov_base, len *= 2))) {
            free(iv[0].iov_base);
            return -3;
        }
        else
            break;
    }

	fprintf(stderr, "influxdb-c::post_http: iv[0] = '%s'\n", (char *)iv[0].iov_base);
	fprintf(stderr, "influxdb-c::post_http: iv[1] = '%s'\n", (char *)iv[1].iov_base);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(c->port);
    if((addr.sin_addr.s_addr = inet_addr(c->host)) == INADDR_NONE) {
        free(iv[0].iov_base);
        return -4;
    }

    if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        free(iv[0].iov_base);
        return -5;
    }

    if(connect(sock, (struct sockaddr*)(&addr), sizeof(addr)) < 0) {
        ret_code = -6;
        goto END;
    }

    if(writev(sock, iv, 2) < (int)(iv[0].iov_len + iv[1].iov_len)) {
        ret_code = -7;
        goto END;
    }

    iv[0].iov_len = recv(sock, iv[0].iov_base, len-1, 0);
    printf("%s", (char*)iv[0].iov_base);
END:

    close(sock);

    free(iv[0].iov_base);
    return ret_code / 100 == 2 ? 0 : ret_code;
}



int create_database(influx_client_t *c, char *name_db)
{
    int sock, ret_code = 0, content_length = 0;
    struct sockaddr_in addr;
    struct iovec iv[2];
    char ch;
    int len, len1;

//    iv[1].iov_base = NULL;
//    iv[1].iov_len = 0;

    if(!(iv[0].iov_base = (char*)malloc(len = 0x100))) {
        return -2;
    }
    if(!(iv[1].iov_base = (char*)malloc(len1 = 0x100))) {

        free(iv[0].iov_base);
        return -2;
    }
    iv[1].iov_len = snprintf((char*)iv[1].iov_base, len, "q=CREATE%%20DATABASE%%20%s\r\n", name_db);
    iv[1].iov_len -= 2;
//POST http://localhost:8086/query --data-urlencode "q=CREATE DATABASE rx_tx
//q=CREATE%20DATABASE%20rx_tx
    for(;;) {
        iv[0].iov_len = snprintf((char*)iv[0].iov_base, len, "POST /query HTTP/1.1\r\nHost: %s:%d\r\nAccept: */*\r\nContent-Length: %zd\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\n",
             c->host, c->port, iv[1].iov_len);
        if((int)iv[0].iov_len >= len && !(iv[0].iov_base = (char*)realloc(iv[0].iov_base, len *= 2))) {

            free(iv[1].iov_base);
            free(iv[0].iov_base);
            return -3;
        }
        else
            break;
    }

	fprintf(stderr, "2 influxdb-c::post_http: iv[0] = '%s'\n", (char *)iv[0].iov_base);
	fprintf(stderr, "2 influxdb-c::post_http: iv[1] = '%s'\n", (char *)iv[1].iov_base);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(c->port);
    if((addr.sin_addr.s_addr = inet_addr(c->host)) == INADDR_NONE) {

        free(iv[1].iov_base);
        free(iv[0].iov_base);
        return -4;
    }

    if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        free(iv[1].iov_base);
        free(iv[0].iov_base);
        return -5;
    }

    if(connect(sock, (struct sockaddr*)(&addr), sizeof(addr)) < 0) {
        ret_code = -6;
        goto END;
    }

    if(writev(sock, iv, 2) < (int)(iv[0].iov_len + iv[1].iov_len)) {
        ret_code = -7;
        goto END;
    }

    iv[0].iov_len = recv(sock, iv[0].iov_base, len-1, 0);
    printf("%s", (char*)iv[0].iov_base);

END:
    close(sock);

    free(iv[0].iov_base);
    free(iv[1].iov_base);
    return ret_code / 100 == 2 ? 0 : ret_code;
}


int post_http_direct(influx_client_t* c, char *line)
{
    int ret_code = 0, len = 0;

    len = strlen(line);

    if(len <= 0)
        return -1;

    ret_code = post_http_send_line(c, line, len);

    return ret_code;
}




//curl -i -XPOST http://localhost:8086/query --data-urlencode "q=CREATE DATABASE rx_tx"
//curl -i -XPOST 'http://192.168.169.13:8086/write?db=rx_tx' --data-binary 'eth_tx,port=0,dir=rx value=5 1561054238000000000'
//curl -i -XPOST 'http://localhost:8086/write?db=mydb' --data-binary 'cpu_load_short,host=server01,region=us-west value=0.64 1434055562000000000'

