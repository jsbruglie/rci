#include "rmb_utils.h"

void parse_args(int argc, char** argv, char** _siip, int* _sipt){

	char* siip = NULL;
	int sipt = -1;

	char opt;

	while( (opt = getopt(argc, argv, "i:p:")) != -1 ){
		switch(opt){

			case 'i':
				siip = (char*)malloc(sizeof(char) * (sizeof(optarg)/sizeof(optarg[0])));
				strcpy(siip, optarg);
				break;
			case 'p':
				sipt = atoi(optarg);
				break;

			default:
				fprintf(stderr, "Usage: %s [-i siip] [-p sipt] \n", argv[0]);
            	exit(EXIT_FAILURE);
		}
	}
	if (siip != NULL) *_siip = siip;
	if (sipt != -1) *_sipt = sipt;
}