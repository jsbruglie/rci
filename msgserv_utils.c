#include "msgserv_utils.h"

void parse_args(int argc, char** argv, char** _name, char** _ip, int* _upt, int* _tpt){
	
	/* Mandatory Parameters */
	char* name = NULL;
	char* ip = NULL;
	/* Optional Parameters */
	char* siip = "tejo.tecnico.ulisboa.pt";
	int sipt = 59000;
	int m = 200;
	int r = 10;	
	
	int upt = -1;
	int tpt = -1;
	char opt;	

	while( (opt = getopt(argc, argv, "n:j:u:t:i:p:m:r:")) != -1 ){
		switch(opt){
			/* Mandatory arg processing */
			case 'n':
				name = (char*)malloc(sizeof(char) * (sizeof(optarg)/sizeof(optarg[0])));
				strcpy(name, optarg);
				break;
			case 'j':
				ip = (char*)malloc(sizeof(char) * (sizeof(optarg)/sizeof(optarg[0])));
				strcpy(ip, optarg);
				break;
			case 'u':
				upt = atoi(optarg);
				break;
			case 't':
				tpt = atoi(optarg);
				break;

			/* Optional arg processing */
			case 'i':
				siip = NULL;
				siip = (char*)malloc(sizeof(char) * (sizeof(optarg)/sizeof(optarg[0])));
				strcpy(siip, optarg);
				break;
			case 'p':
				sipt = atoi(optarg);
				break;
			case 'm':
				m = atoi(optarg);
				break;
			case 'r':
				r = atoi(optarg);
				break;
			default:
				fprintf(stderr, "Usage: %s -n name -j ip -u upt -t tpt [-i siip] [-p sipt] [-m m] [-r r] \n", argv[0]);
            	exit(EXIT_FAILURE);
		}
	}

	if(name == NULL || ip == NULL || upt == -1 || tpt == -1){ //Check if the arguments were provided
		fprintf(stderr, "Usage: %s -n name -j ip -u upt -t tpt [-i siip] [-p sipt] [-m m] [-r r] \n", argv[0]);
		exit(EXIT_FAILURE);
	}
	if(upt == tpt)
		printf("Error: UDP and TCP port are the same.\n"); 	
	
	*_name = name; *_ip = ip; *_upt = upt; *_tpt = tpt;
}