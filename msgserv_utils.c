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

int create_udp_client(){
    int fd = socket(AF_INET,SOCK_DGRAM,0);
    return fd;
}

int create_tcp_server(u_short port){
    
    int fd;
    struct hostent *hostptr;
    struct sockaddr_in server_address;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    memset((void*) &server_address, (int) '\0', sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons((u_short) port);
    bind(fd, (struct sockaddr*) &server_address, sizeof(server_address));

    listen(fd,5);

    return fd;
}

/* Server Requests */

int accept_tcp_connection(int fd){

    int new_fd;
    struct sockaddr_in client_address; 
    int client_length = sizeof(client_address);

    new_fd = accept(fd, (struct sockaddr*) &client_address, &client_length);
    return new_fd;
}

void refresh(int fd, char* name, char* ip, char* siip, int sipt, int upt, int tpt){
    struct hostent *hostptr;
    struct sockaddr_in server_address;
    int address_length;
    char registration[1024];

    memset((void*)&server_address, (int)'\0',sizeof(server_address));
    server_address.sin_family = AF_INET;
    hostptr = gethostbyname(siip);
    server_address.sin_addr.s_addr = ((struct in_addr *) (hostptr->h_addr_list[0]))->s_addr;
    server_address.sin_port = htons((u_short)sipt);
    
    sprintf(registration, "REG %s;%s;%d;%d",name,ip,upt,tpt);
    address_length = sizeof(server_address);
    sendto(fd, registration, strlen(registration) + 1, 0, (struct sockaddr*) &server_address, address_length);
    
    //debug_print("REFRESH: Just registered in the Identity Server.\n");
}

int send_messages(int fd, struct sockaddr_in* client_addr_ptr, MessageTable* msg_table, int n){

    int ret = -1;
    int address_length = sizeof(*client_addr_ptr);

    sort_msg_table(msg_table);
    int size = strlen("MESSAGES\n") + size_latest_messages(msg_table, n, 0, !INCLUDE_CLK);
    char* buffer = malloc(sizeof(char) * size);
    strcpy(buffer,"MESSAGES\n");
    get_latest_messages(msg_table, n, 0, !INCLUDE_CLK, buffer);
    //debug_print("SEND_MSG: %d/%d bytes \n%s\n", size, strlen(buffer) + 1, buffer);
    ret = sendto(fd, buffer, size, 0, (struct sockaddr*) client_addr_ptr, address_length);
    free(buffer);
    
    return ret;
}

int send_messages_tcp(int fd, MessageTable* msg_table, int n, int all){

    int ret = -1;
    
    int size = strlen("SMESSAGES\n") + size_latest_messages(msg_table, n, all, INCLUDE_CLK);
    char* buffer = malloc(sizeof(char) * size);
    strcpy(buffer,"SMESSAGES\n");
    get_latest_messages(msg_table, n, all, INCLUDE_CLK, buffer);
    //debug_print("SEND_MSG_TCP: %d/%d bytes \n%s\n", size, strlen(buffer) + 1, buffer);
    ret = write(fd, buffer, strlen(buffer) + 1);
    free(buffer);
    
    return ret;
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

    char * return_string = (char*)malloc(sizeof(server_list));
    strcpy(return_string,server_list);

    debug_print("%s", server_list);

    return return_string;
}

FdStruct* create_fd_struct(int upt, int tpt){
    
    int max = 0;
    FdStruct* fd_s = (FdStruct*) malloc(sizeof(FdStruct));
    if (fd_s == NULL){
        return NULL;
    }
    fd_s->std_in = STDIN;
    fd_s->rmb_udp = create_udp_server(upt); 
    fd_s->si_udp = create_udp_client();
    fd_s->msg_tcp = create_tcp_server(tpt);
    // Set max to the highest integer among monitored file descriptors
    max = (fd_s->rmb_udp > max)? fd_s->rmb_udp : max;  
    max = (fd_s->si_udp > max)? fd_s->si_udp : max;
    max = (fd_s->msg_tcp > max)? fd_s->msg_tcp : max;
    fd_s->max = max;
    
    debug_print("CREATE FD STRUCT: rmb %d si %d msg %d max %d\n",fd_s->rmb_udp, fd_s->si_udp, fd_s->msg_tcp, fd_s->max);
    return fd_s;
}   

void delete_fd_struct(FdStruct* fd){
    if (fd != NULL){
        free(fd);
    }
}

void init_fd_set(fd_set* set, FdStruct* fd){
    if (set != NULL && fd != NULL){
        FD_ZERO(set);
        FD_SET(fd->std_in, set);
        FD_SET(fd->si_udp, set);
        FD_SET(fd->rmb_udp, set);
        FD_SET(fd->msg_tcp, set);
    }    
}

int fd_max(FdStruct* fd_struct){
    if (fd_struct != NULL){
        return fd_struct->max;
    }
    return -1;
}



