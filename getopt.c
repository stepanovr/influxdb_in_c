#include <stdint.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "config.h"



uint32_t  input_param_status = 0;

char param_addr[IPADDRLEN];
char param_interface[IPADDRLEN];
char param_db_name[IPADDRLEN];

int param_port = 0;

int ctrl_opts (int argc, char **argv)
{
	int index;
	int c;

	opterr = 0;


	while ((c = getopt (argc, argv, "a:bcd:p:i:")) != -1)
		switch (c)
		{
			case 'a':
				strncpy(param_addr, optarg, IPADDRLEN-1);
				SET_INPUT_PARAM(AFLAG);
				break;
			case 'p':
				param_port = atoi(optarg);
				SET_INPUT_PARAM(PFLAG);
				break;
			case 'i':
				strncpy(param_interface, optarg, IPADDRLEN-1);
				SET_INPUT_PARAM(IFLAG);
				break;
			case 'b':
				SET_INPUT_PARAM(BFLAG);
				break;
                        case 'c':
                                SET_INPUT_PARAM(CFLAG);
                                break;
			case 'd':
				SET_INPUT_PARAM(DFLAG);
				strncpy(param_db_name, optarg, IPADDRLEN-1);
				break;
			case '?':
				switch(optopt){
					case 'a':
					case 'p':
					case 'd':
                                        case 'i':
 						fprintf (stderr, "Option -%c requires an argument.\n", optopt);
						break;

					default:
						fprintf (stderr,
								"Unknown option character `\\x%x'.\n", optopt);
				}

				return 1;
			default:
				abort ();
		}


	printf ("addr = %s:%d  %s %08X %s\n", param_addr, param_port, param_interface, input_param_status, param_db_name);

	for (index = optind; index < argc; index++)
		printf ("Non-option argument %s\n", argv[index]);

	return 0;
}

