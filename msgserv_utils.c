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

/* Server Requests */
int send_messages(int fd, struct sockaddr_in* client_addr_ptr, MessageTable* msg_table, int n){

    int ret, address_length = sizeof(*client_addr_ptr);

    sort_msg_table(msg_table);
    int size = strlen("MESSAGES\n") + size_latest_messages(msg_table, n, !INCLUDE_CLK);
    char* buffer = malloc(sizeof(char) * size);
    strcpy(buffer,"MESSAGES\n");
    get_latest_messages(msg_table, n, !INCLUDE_CLK, buffer);
    //debug_print("SEND_MSG: %d/%d bytes \n%s\n", size, strlen(buffer) + 1, buffer);
    
    ret = sendto(fd, buffer, size, 0, (struct sockaddr*) client_addr_ptr, address_length);
    free(buffer);
    return ret;
}

void register_in_server(char* name, char* ip, char* siip, int sipt, int upt, int tpt){

    int fd;
    struct hostent *hostptr;
    struct sockaddr_in server_address;
    int address_length;
    char registration[1024];

    fd = socket(AF_INET,SOCK_DGRAM,0);
    memset((void*)&server_address, (int)'\0',sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons((u_short)sipt);
    hostptr = gethostbyname(siip);
    server_address.sin_addr.s_addr = ((struct in_addr *) (hostptr->h_addr_list[0]))->s_addr;

    sprintf(registration, "REG %s;%s;%d;%d",name,ip,upt,tpt);
    address_length = sizeof(server_address);
    sendto(fd, registration, strlen(registration) + 1, 0, (struct sockaddr*) &server_address, address_length);
    close(fd);
    
    debug_print("REFRESH: Just registered in the Identity Server.\n");
}

char* get_servers(char* siip, int sipt){

    int fd;
    struct hostent *hostptr;
    struct sockaddr_in server_address;
    int address_length;
    char server_list[2048];

    fd = socket(AF_INET,SOCK_DGRAM,0);
    memset((void*)&server_address, (int)'\0',sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons((u_short)sipt);
    hostptr = gethostbyname(siip);
    server_address.sin_addr.s_addr = ((struct in_addr *)(hostptr->h_addr_list[0]))->s_addr;
    
    address_length = sizeof(server_address);
    sendto(fd, "GET_SERVERS", strlen("GET_SERVERS") + 1, 0, (struct sockaddr*) &server_address, address_length);
    address_length = sizeof(server_address);
    recvfrom(fd, server_list, sizeof(server_list), 0, (struct sockaddr*) &server_address, &address_length);
    close(fd);

    debug_print("%s", server_list);
}

FdStruct* create_fd_struct(int upt){
    FdStruct* fd_s = (FdStruct*) malloc(sizeof(FdStruct));
    fd_s->std_in = STDIN;
    fd_s->rmb_udp = create_udp_server(upt);
}

void init_fd_set(fd_set* set, int fd_stdin, int fd_si_udp, int fd_rmb_udp){
    FD_ZERO(set);
    FD_SET(fd_stdin, set);
    FD_SET(fd_si_udp, set);
    FD_SET(fd_rmb_udp, set);
}

int fd_max(FdStruct* fd_struct){
    if (fd_struct != NULL){
        return fd_struct->max;
    }
    return NULL;
}