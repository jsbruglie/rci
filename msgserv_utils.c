#include "msgserv_utils.h"

/* Message Server */
void parse_args(int argc, char** argv, char** _name, char** _ip, int* _upt, int* _tpt,
				char** _siip, int* _sipt, int* _m, int* _r){
	
	/* Mandatory Parameters */
	char* name = NULL;
	char* ip = NULL;
	int upt = -1;
	int tpt = -1;
	
	/* Optional Parameters */
	char* siip = NULL;
	int sipt = -1;
	int m = -1;
	int r = -1;	
	
	char opt;	

	while( (opt = getopt(argc, argv, "n:j:u:t:i:p:m:r:")) != -1 ){
		switch(opt){
			/* Mandatory arg processing */
			case 'n':
				name = (char*)malloc(sizeof(char) * (strlen(optarg) + 1));
				strcpy(name, optarg);
				break;
			case 'j':
				ip = (char*)malloc(sizeof(char) * (strlen(optarg) + 1));
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
				siip = (char*)malloc(sizeof(char) * (strlen(optarg) + 1));
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

	// Check if mandatory arguments were provided
	if(name == NULL || ip == NULL || upt == -1 || tpt == -1){ 
		fprintf(stderr, "Usage: %s -n name -j ip -u upt -t tpt [-i siip] [-p sipt] [-m m] [-r r] \n", argv[0]);
		exit(EXIT_FAILURE);
	}
	if(upt == tpt){
		fprintf(stderr,"Error: UDP and TCP port are the same.\n"); 	
		exit(EXIT_FAILURE);
	}
	
	*_name = name; *_ip = ip; *_upt = upt; *_tpt = tpt;
	
	if(siip != NULL) *_siip = siip;
	if(sipt != -1) *_sipt = sipt;
	if(m != -1) *_m = m;
	if(r != -1) *_r = r; 
}

/* Socket handling */
int create_udp_server(u_short port){

	int fd;
	struct hostent *hostptr;
	struct sockaddr_in server_address;

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	memset((void*) &server_address, (int) '\0', sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons((u_short) port);
	bind(fd, (struct sockaddr*) &server_address, sizeof(server_address));

	return fd;
}


/* Message Table */

int get_msg_clock(Message** msg_table, int i){
	if (msg_table != NULL){
		if(msg_table[i] != NULL){
			return msg_table[i]->clock;
		}
	}
	return -1;
}

void swap_msg(Message** msg_table, int a, int b){
	if (msg_table != NULL || a == b){
		if(msg_table[a] != NULL && msg_table[b] != NULL){
			Message* temp = msg_table[a];
			msg_table[a] = msg_table[b];
			msg_table[b] = temp;
		}
	}
}

Message** create_msg_table(int size){

	Message** message_table = (Message**) malloc(sizeof(Message*) * size);
	int i;
	for(i = 0; i < size; i++){
		message_table[i] = NULL;
		//message_table[i] = (Message*) malloc(sizeof(Message));
		//message_table[i]->clock = size - i;
	}
	return message_table;
}

void delete_msg_table(Message** msg_table, int size){

	int i;
	for(i = 0; i < size; i++){
		if(msg_table[i] != NULL)
			free(msg_table[i]);
	}
	free(msg_table);
}

void sort_msg_table(Message** msg_table, int size, int items){
	quick_sort(msg_table, 0, items - 1);
}

/* Quick Sort */
void quick_sort(Message** msg_table, int l, int r){
	int partition_point, pivot;
	if(r > l){
		pivot = get_msg_clock(msg_table, r);
		partition_point = partition(msg_table, l, r, pivot);
		printf("QUICK: l - %d r - %d p - %d.\n", l, r, pivot);
		quick_sort(msg_table, l, partition_point - 1);
		quick_sort(msg_table, partition_point + 1, r);
	}
}

int partition(Message** msg_table, int l, int r, int pivot){
	int i = l - 1;
	int j = r;

	while(1){
   		while(get_msg_clock(msg_table, ++i) < pivot){}
		while(j > 0 && get_msg_clock(msg_table, --j) > pivot){}
      	if(i >= j)
			break;
      	swap_msg(msg_table, i, j);
	}
	swap_msg(msg_table, i, r);
	return i;
}

/* DEBUG */
void print_msg_table(Message** msg_table, int size){

	int i;
	for (i = 0; i < size; i++){
		if(msg_table[i] != NULL)
			printf("MSG: %d\n", msg_table[i]->clock);
	}
}