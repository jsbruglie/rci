#include "rmb_utils.h"

void parse_args(int argc, char** argv, char** _siip, int* _sipt){

    char* siip = NULL;
    int sipt = -1;
    char opt;

    while( (opt = getopt(argc, argv, "i:p:")) != -1 ){
        switch(opt){

            case 'i':
                siip = (char*) malloc(sizeof(char) * (strlen(optarg) +1));
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
    if (siip != NULL){
        siip = (char*) malloc(sizeof(char) * (strlen(siip)));
        strcpy(siip, *_siip);
    }
    *_siip = siip;
    
    if (sipt != -1)
        *_sipt = sipt;
}

void show_latest_messages(int n, char* msgserv_ip, int msgserv_upt){
    
    int fd;                                 // Socket file descriptor
    struct hostent *hostptr;                // Host name structure pointer
    struct sockaddr_in server_address;      // Server address struct
    int address_length;                     // Address length

    char buffer[BUFFER_SIZE];
    char protocol_msg[PROTOCOL_SIZE] = "GET_MESSAGES ";
    char number_msg[10];
    snprintf(number_msg, 10, "%d", n);
    strcat(protocol_msg, number_msg);

    if (n < 0 || !strcmp(msgserv_ip,"") || msgserv_upt < 0)
        return;

    fd = socket(AF_INET,SOCK_DGRAM,0);
    hostptr = gethostbyname(msgserv_ip);
    memset((void*)&server_address, (int)'\0', sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = ((struct in_addr *)(hostptr->h_addr_list[0]))->s_addr;
    server_address.sin_port = htons((u_short)msgserv_upt);

    address_length = sizeof(server_address);
    int r = sendto(fd, protocol_msg, strlen(protocol_msg)+1, 0,(struct sockaddr*)&server_address, address_length);
    if(r==-1)exit(EXIT_FAILURE);//error

    address_length = sizeof(server_address);
    r = recvfrom(fd,buffer,sizeof(buffer), 0,(struct sockaddr*)&server_address, &address_length);
    if(r==-1)exit(EXIT_FAILURE);//error

    // Print server reply, this is NOT debug
    printf("%s",buffer);

    close(fd);
}
char* get_servers(char* siip, int sipt){

    int fd;
    struct hostent *hostptr;
    struct sockaddr_in server_address;
    int address_length;
    char server_list[BUFFER_SIZE];

    fd = socket(AF_INET,SOCK_DGRAM,0);
    hostptr = gethostbyname(siip);
    memset((void*)&server_address, (int)'\0',sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = ((struct in_addr *)(hostptr->h_addr_list[0]))->s_addr;
    server_address.sin_port = htons((u_short)sipt);

    address_length = sizeof(server_address);
    int n = sendto(fd, "GET_SERVERS", strlen("GET_SERVERS") + 1, 0, (struct sockaddr*) &server_address, address_length);
    if(n==-1)exit(EXIT_FAILURE);//error
    
    address_length = sizeof(server_address);
    recvfrom(fd, server_list, sizeof(server_list), 0, (struct sockaddr*) &server_address, &address_length);
    if(n==-1)exit(EXIT_FAILURE);//error
    close(fd);

    char * return_string = (char*)malloc(sizeof(server_list));
    if(return_string == NULL){
        printf("Out of memory\n");exit(EXIT_FAILURE); 
    }
    strcpy(return_string,server_list);

    //debug_print("%s", server_list);

    return return_string;
}

void pick_server(char* buffer, char* msgserv_name, char* msgserv_ip, int* msgserv_upt, int* msgserv_tpt){
    // Picks the first server of the list
    char* p1 = buffer + 8; 
    char p2[BUFFER_SIZE];
    sscanf(p1, "%[^\n]", p2);
    debug_print("%s\n", p2);
    sscanf(p2, "%256[^;];%256[^;];%d;%d", msgserv_name, msgserv_ip, &(*msgserv_upt), &(*msgserv_tpt));
}

void publish_msg(char* message, char* msgserv_ip, int msgserv_upt){
    int fd;
    struct hostent *hostptr;
    struct sockaddr_in server_address;
    int address_length;
    char protocol_msg[PROTOCOL_SIZE] = "PUBLISH ";
    strcat(protocol_msg,message);

    debug_print("Publishing message to msgserv, IP: %s PORT: %d\n", msgserv_ip, msgserv_upt);

    fd = socket(AF_INET,SOCK_DGRAM,0);
    hostptr = gethostbyname(msgserv_ip);
    memset((void*)&server_address, (int)'\0',sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = ((struct in_addr *)(hostptr->h_addr_list[0]))->s_addr;
    server_address.sin_port = htons((u_short)msgserv_upt); 

    address_length = sizeof(server_address);
    int n = sendto(fd, protocol_msg, strlen(protocol_msg)+1,0,(struct sockaddr*)&server_address,address_length);
    if(n == -1)
        exit(EXIT_FAILURE);

    close(fd);
}

void print_servers(char* siip, int sipt){
    int fd;
    struct hostent *hostptr;
    struct sockaddr_in server_address;
    int address_length;
    char server_list[BUFFER_SIZE];

    fd = socket(AF_INET,SOCK_DGRAM,0);
    hostptr = gethostbyname(siip);
    memset((void*)&server_address, (int)'\0',sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = ((struct in_addr *)(hostptr->h_addr_list[0]))->s_addr;
    server_address.sin_port = htons((u_short)sipt);

    address_length = sizeof(server_address);
    int n = sendto(fd, "GET_SERVERS", strlen("GET_SERVERS") + 1, 0, (struct sockaddr*) &server_address, address_length);
    if(n == -1)
        exit(EXIT_FAILURE);

    address_length = sizeof(server_address);
    n = recvfrom(fd, server_list, sizeof(server_list), 0, (struct sockaddr*) &server_address, &address_length);
    if(n == -1)
        exit(EXIT_FAILURE);
    
    close(fd);

    printf("%s\n", server_list); //This is not debug
}