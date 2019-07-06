#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <sys/queue.h>
#include <unistd.h>

#include "list.h"


#define LIMIT_INTERFACES_AMOUNT      20

#define FIELDS_NUM      20
#define STR_LEN	200

#define INPUT_NAME "/proc/net/dev"

#define	FIELD0	"bytes"
#define FIELD1  "packets"
#define FIELD2  "errs"

char input[] = {"12 124 3256 57856 457   886578 90"};
char input1[] = {"12 124 3256 57856 457   886578 90"};
char buffer[STR_LEN];

static uint64_t result_int_64[FIELDS_NUM];
static uint64_t previous_int_64[LIMIT_INTERFACES_AMOUNT][FIELDS_NUM] = {0};
static char    *result_char[FIELDS_NUM];


int find_index(char *arr[], char* pattern, int start, int end)
{
	int i;
	for(i = start; i < end; i ++){
		if(arr[i] == NULL){
			return -1;
		}
		if(0 == strcmp(arr[i], pattern)){
			return i;
		}
	}
	return -1;
}

void release(char *arr[], int len)
{
	int i;
	for(i = 0; i < len; i ++){
		free(arr[i]);
	}
}

int init(char *arr[], int len)
{
	int i;
	for(i = 0; i < len; i++){
		if(!(arr[i] = malloc(FIELDS_NUM))){
			release(arr, i - 1);
			return -1;
		}
	}

}


int split_int(uint64_t* dst, char* str) {
	int init_size = strlen(str);
	char delim[] = " ";
	char *ptr = strtok(str, delim);
	int i = 0;

	while(ptr != NULL)
	{
//		printf("'%s'\n", ptr);
		dst[i++] = atoi(ptr);
		ptr = strtok(NULL, delim);
	}

	return i;
}

int split_int_named(uint64_t* dst, char* name, char* str) {
	int init_size = strlen(str);
	char delim[] = " ";
	char *ptr = strtok(str, delim);
	int i = 0;
	char* tmp_ptr;

	if(!ptr){
		return 0;
	}
	strcpy(name, ptr);
	ptr = strtok(NULL, delim);

	while(ptr != NULL)
	{
//		printf("'%s'\n", ptr);
		dst[i++] = strtoul(ptr, &tmp_ptr, 10);
		ptr = strtok(NULL, delim);
	}

	return i;
}

int split_char_skip(char* dst[], char* str) {
	int init_size = strlen(str);
	char delim[] = " ";
	char *ptr = strtok(str, delim);
	int i = 0;

	if(!ptr){
		return 0;
	}
	ptr = strtok(NULL, delim);

	while(ptr != NULL)
	{
//		printf("'%s'\n", ptr);
		strcpy(result_char[i++], ptr);
		ptr = strtok(NULL, delim);
	}

	return i;
}

int split_char(char* dst[], char* str) {
	int init_size = strlen(str);
	char delim[] = " ";
	char *ptr = strtok(str, delim);
	int i = 0;

	while(ptr != NULL)
	{
//		printf("'%s'\n", ptr);
		strcpy(result_char[i++], ptr);
		ptr = strtok(NULL, delim);
	}

	return i;
}


char field_name[50];


//'foo,k=v,x=y x=10i,y=10.30,z=10.35,b=t 1512722735522840540
//curl -i -XPOST 'http://192.168.1.230:8086/write?db=rx_tx' --data-binary 'eth_rx_tx,port=0,dir=rx value=394 1561605480000000000'

//eth0,port=0 rx_bytes=%llu,rx_packets=%llu,rx_errors=%llu,tx_bytes=%llu,tx_packets=%llu,tx_errors=%llu

//foo,k=v,x=y x=10i,y=10.30,z=10.35,b=t

int measure_net()
{
	int i;
	int len;
	char *replace;
        int f0_rx, f0_tx;
        int f1_rx, f1_tx;
        int f2_rx, f2_tx;
	int if_cnt = 0;
	tailq_entry_t* item;
	FILE* fi;
	int len_out;
        unsigned long long current_time;

	char *tmp = buffer;
	if(init(result_char, FIELDS_NUM) < 0){
		return -1;
	}



	fi  = fopen(INPUT_NAME, "r");

	fgets(tmp, STR_LEN, fi);
	if(!tmp){
		return -1;
	}

	fgets(tmp, STR_LEN, fi);
	if(!tmp){
		return -1;
	}

	replace = strstr(tmp, "|");
	*replace = ' ';
	replace = strstr(tmp, "|");
	*replace = ' ';

	replace = strstr(tmp, "\n");
	*replace = ' ';

	len = split_char_skip(result_char, tmp);
	//        printf("length = %d\n", len);

	while(tmp){
                uint64_t *prev = previous_int_64[if_cnt];
		if(LIMIT_INTERFACES_AMOUNT == if_cnt){
                    fprintf(stderr, "Net interfaces amount exceeds the linit of %d\n", if_cnt);
                }
		fgets(tmp, STR_LEN, fi);
//		printf("Str length: %d\n", strlen(tmp));
		if((!tmp) || (strlen(tmp) < 10)){
			break;
		}
		replace = strstr(tmp, "\n");
		*replace = ' ';


		len = split_int_named(result_int_64, field_name, tmp);
		//	printf("length = %d\n", len);

                f0_rx = find_index(result_char, FIELD0, 0, len);
                f0_tx = find_index(result_char, FIELD0, len/2, len);

                f1_rx = find_index(result_char, FIELD1, 0, len);
                f1_tx = find_index(result_char, FIELD1, len/2, len);

                f2_rx = find_index(result_char, FIELD2, 0, len);
                f2_tx = find_index(result_char, FIELD2, len/2, len);

		field_name[strlen(field_name) -1] = '\0';
/*
		printf("%s,dir=both rx_%s=%llu,rx_%s=%llu,rx_%s=%llu,tx_%s=%llu,tx_%s=%llu,tx_%s=%llu", field_name,
                       result_char[f0_rx], (long long)(result_int_64[f0_rx] - prev[f0_rx]),
                       result_char[f1_rx], (long long)(result_int_64[f1_rx] - prev[f1_rx]),
                       result_char[f2_rx], (long long)(result_int_64[f2_rx] - prev[f2_rx]),
                       result_char[f0_tx], (long long)(result_int_64[f0_tx] - prev[f0_tx]),
                       result_char[f1_tx], (long long)(result_int_64[f1_tx] - prev[f1_tx]),
                       result_char[f2_tx], (long long)(result_int_64[f2_tx] - prev[f2_tx]));
*/

		item = malloc(sizeof(*item));
                if(NULL == item){
                        perror("malloc failed");
                        abort();
                }

                item->payload = malloc(sizeof(*item->payload));
                if(NULL == item->payload){
                        perror("malloc failed");
                        abort();
                }

                item->payload->string = malloc(STR_LEN * 5);
                if(NULL == item->payload->string){
                        perror("malloc failed");
                        abort();
                }

    current_time = time(NULL);
    current_time *= 1000000000;

		len_out = snprintf(item->payload->string, STR_LEN * 5, "%s,dir=both rx_%s=%llui,rx_%s=%llui,rx_%s=%llui,tx_%s=%llui,tx_%s=%llui,tx_%s=%llui %llu",
		       field_name,
                       result_char[f0_rx], (long long)(result_int_64[f0_rx] - prev[f0_rx]),
                       result_char[f1_rx], (long long)(result_int_64[f1_rx] - prev[f1_rx]),
                       result_char[f2_rx], (long long)(result_int_64[f2_rx] - prev[f2_rx]),
                       result_char[f0_tx], (long long)(result_int_64[f0_tx] - prev[f0_tx]),
                       result_char[f1_tx], (long long)(result_int_64[f1_tx] - prev[f1_tx]),
                       result_char[f2_tx], (long long)(result_int_64[f2_tx] - prev[f2_tx]),
                       current_time);
		printf("%s\n", item->payload->string);
		push(item);

                memcpy(prev, result_int_64, len * sizeof(result_int_64[0]));
                if_cnt++;
//		printf("\n\ninterface: %s\n", field_name);
/*
		for(i = 0; i < len; i ++){
			printf("%s: %s:%llu\n", i<len/2 ? "RX":"TX", result_char[i], (long long)result_int_64[i]);

//foo,k=v,x=y x=10i,y=10.30,z=10.35,b=t
		}
*/
	}

	release(result_char, FIELDS_NUM);

	fclose(fi);


	return 0;
}

/*
int main()
{
	init_queue();
	measure_net();
	usleep(1000000);
	return measure_net();
}
*/
