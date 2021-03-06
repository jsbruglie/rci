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
        err_print("UDP and TCP port are the same.");  
        exit(EXIT_FAILURE);
    }
    if (siip == NULL){
        siip = (char*) malloc(sizeof(char) * (strlen(*_siip) + 1));
        strcpy(siip, *_siip);
    }

    *_name = name; *_ip = ip; *_upt = upt; *_tpt = tpt;
    
    if(siip != NULL) *_siip = siip;
    if(sipt != -1) *_sipt = sipt;
    if(m != -1) *_m = m;
    if(r != -1) *_r = r; 
}

/* Socket handling */
int create_udp_server(u_short port){

    struct hostent *hostptr;
    struct sockaddr_in server_address;

    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd == -1){
        err_print("Could not create a socket. Exiting...");
        exit(EXIT_FAILURE);
    }

    memset((void*) &server_address, (int) '\0', sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons((u_short) port);
    int ret = bind(fd, (struct sockaddr*) &server_address, sizeof(server_address));
    if(ret == -1){
        err_print("Bind failed. Exiting...");
        exit(EXIT_FAILURE);
    }

    return fd;
}

int create_udp_client(){
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd == -1){
        err_print("Could not create a socket. Exiting...");
        exit(EXIT_FAILURE);
    }
    return fd;
}

int create_tcp_server(u_short port){
    
    struct hostent *hostptr;
    struct sockaddr_in server_address;

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1){
        err_print("Could not create a socket. Exiting...");
        exit(EXIT_FAILURE);
    }

    memset((void*) &server_address, (int) '\0', sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons((u_short) port);
    int ret = bind(fd, (struct sockaddr*) &server_address, sizeof(server_address));
    if(ret == -1){
        err_print("Bind failed. Exiting...");
        exit(EXIT_FAILURE);
    }
    listen(fd,5);

    return fd;
}

/* Server Requests */

int accept_tcp_connection(int fd){

    int new_fd;
    struct sockaddr_in client_address; 
    int client_length = sizeof(client_address);

    new_fd = accept(fd, (struct sockaddr*) &client_address, &client_length);
    if(new_fd == -1){
        err_print("Accept failed. Exiting...");
        exit(EXIT_FAILURE);
    }
    return new_fd;
}

void refresh(int fd, char* name, char* ip, char* siip, int sipt, int upt, int tpt){
    
    struct hostent *hostptr;
    struct sockaddr_in server_address;
    int address_length;
    char registration[BUFFER_SIZE] = {0};

    memset((void*)&server_address, (int)'\0',sizeof(server_address));
    server_address.sin_family = AF_INET;
    hostptr = gethostbyname(siip);
    server_address.sin_addr.s_addr = ((struct in_addr *) (hostptr->h_addr_list[0]))->s_addr;
    server_address.sin_port = htons((u_short)sipt);
    
    sprintf(registration, "REG %s;%s;%d;%d",name,ip,upt,tpt);
    address_length = sizeof(server_address);
    int n = sendto(fd, registration, strlen(registration), 0, (struct sockaddr*) &server_address, address_length);
    if(n == -1){
        err_print("sendto failed. Check if identity server is online. Exiting...");
        exit(EXIT_FAILURE);
    }
    //debug_print("REFRESH: Just registered in the Identity Server.\n");
}

void send_messages_udp(int fd, struct sockaddr_in* client_addr_ptr, MessageTable* msg_table, int n){

    int address_length = sizeof(*client_addr_ptr);

    /* Sort message table by ascending logic clock */
    sort_msg_table(msg_table);

    int size = strlen("MESSAGES\n") + size_latest_messages(msg_table, n, 0, !INCLUDE_CLK);
    char* buffer = malloc(sizeof(char) * (size + 1));
    strcpy(buffer,"MESSAGES\n");
    get_latest_messages(msg_table, n, 0, !INCLUDE_CLK, buffer);
    int nbytes = sendto(fd, buffer, size, 0, (struct sockaddr*) client_addr_ptr, address_length);
    debug_print("SEND_MSG: %d/%d bytes \n%s\n", size, (int) strlen(buffer) , buffer);
    if(nbytes == -1){
        err_print("sendto failed. Exiting...");
        exit(EXIT_FAILURE);
    } 
    free(buffer);
}

void send_messages_tcp(int fd, MessageTable* msg_table, int n, int all){
    
    int size = strlen("SMESSAGES\n") + size_latest_messages(msg_table, n, all, INCLUDE_CLK);
    char* buffer = malloc(sizeof(char) * (size + 1));
    strcpy(buffer,"SMESSAGES\n");
    get_latest_messages(msg_table, n, all, INCLUDE_CLK, buffer);
    int nbytes = write(fd, buffer, strlen(buffer));
    debug_print("SEND_MSG_TCP: %d/%d bytes \n%s", size, (int) strlen(buffer), buffer);
    if(nbytes == -1){
        /* Peer closed socket. Removal from list is handled in msgserv::handle_msg_activity */
        err_print("TCP write failed. Exiting...");
        free(buffer);
        exit(EXIT_FAILURE);
    }
    free(buffer);   
}

void get_servers(char* siip, int sipt, char* server_string){

    struct hostent *hostptr;
    struct sockaddr_in server_address;
    int address_length;
    int nbytes;
    char buffer[BUFFER_SIZE] = {0};

    /* Create UDP socket and setup */
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd==-1){
        err_print("Could not create a socket. Exiting...");
        exit(EXIT_FAILURE);
    }

    memset((void*)&server_address, (int)'\0',sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons((u_short)sipt);
    hostptr = gethostbyname(siip);
    server_address.sin_addr.s_addr = ((struct in_addr *)(hostptr->h_addr_list[0]))->s_addr;
    address_length = sizeof(server_address);

    /* Send a Get Servers request */
    nbytes = sendto(fd, "GET_SERVERS", strlen("GET_SERVERS"), 0, (struct sockaddr*) &server_address, address_length);
    if(nbytes == -1){
        err_print("sendto failed. Check if identity server is online.Exiting...");
        exit(EXIT_FAILURE);
    }

    /* Receive the Identity Server response */
    nbytes = recvfrom(fd, buffer, sizeof(buffer), 0, (struct sockaddr*) &server_address, &address_length);
    if(nbytes == -1){
        err_print("recvfrom failed. Check if identity server is online. Exiting...");
        exit(EXIT_FAILURE);
    }

    /* Force the insertion of a trailing null character in the response */
    /* Ternary operator prevents buffer overflow */
    buffer[(nbytes == sizeof(buffer))? nbytes - 1 : nbytes] = '\0';

    close(fd);

    strcpy(server_string, buffer);
    debug_print("%s", server_string);
}

/* FdStruct Functions */

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
    
    debug_print("Created: rmb %d si %d msg %d max %d",fd_s->rmb_udp, fd_s->si_udp, fd_s->msg_tcp, fd_s->max);
    return fd_s;
}   

void delete_fd_struct(FdStruct* fd){
    if (fd != NULL){
        close(fd->si_udp);
        close(fd->rmb_udp);
        close(fd->msg_tcp);
        free(fd);
    }
}

void init_fd_set(fd_set* set, FdStruct* fd, ServerID* sv){
    ServerID* id;
    if (set != NULL && fd != NULL){
        FD_ZERO(set);
        FD_SET(fd->std_in, set);
        FD_SET(fd->si_udp, set);
        FD_SET(fd->rmb_udp, set);
        FD_SET(fd->msg_tcp, set);
        for (id = sv; id != NULL; id = id->next){
            FD_SET(id->fd, set);
        }
    }    
}

int fd_max(FdStruct* fd_struct, ServerID* sv_list){
    int max = -1;
    ServerID* id;
    if (fd_struct != NULL){
        max = fd_struct->max;
        for (id = sv_list; id != NULL; id = id->next){
            max = (id->fd > max)? id->fd : max;
        }
        return max;
    }
    return -1;
}



