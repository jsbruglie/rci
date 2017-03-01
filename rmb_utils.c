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

void show_latest_messages(int n, char* msgserv_ip, int msgserv_upt){
    
    int fd;                                 // Socket file descriptor
    struct hostent *hostptr;                // Host name structure pointer
    struct sockaddr_in server_address;      // Server address struct
    int address_length;                     // Address length

    char buffer[2048];
    char protocol_msg[200] = "GET_MESSAGES ";
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
    sendto(fd, protocol_msg, strlen(protocol_msg)+1, 0,(struct sockaddr*)&server_address, address_length);

    address_length = sizeof(server_address);
    recvfrom(fd,buffer,sizeof(buffer), 0,(struct sockaddr*)&server_address, &address_length);

    // Print server reply
    printf("%s",buffer);

    close(fd);
}
char* get_servers(char* siip, int sipt){

    int fd;
    struct hostent *hostptr;
    struct sockaddr_in server_address;
    int address_length;
    char server_list[2048];

    fd = socket(AF_INET,SOCK_DGRAM,0);
    hostptr = gethostbyname(siip);
    memset((void*)&server_address, (int)'\0',sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = ((struct in_addr *)(hostptr->h_addr_list[0]))->s_addr;
    server_address.sin_port = htons((u_short)sipt);

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

void pick_server(char* buffer, char* msgserv_name, char* msgserv_ip, int* msgserv_upt, int* msgserv_tpt){
    //Getting data - we just need to get the first msgserv in the list of servers we get and publish to that one, it will then propagate
    //Alter this to be a random server in buffer
    char* p1 = buffer + 8; 
    char p2[256];
    sscanf(p1,"%[^\n]",p2);
    debug_print("%s\n", p2);
    sscanf(p2,"%256[^;];%256[^;];%d;%d",msgserv_name,msgserv_ip,&(*msgserv_upt),&(*msgserv_tpt));
}

void publish_msg(char* message, char* msgserv_ip, int msgserv_upt){
    int fd;
    struct hostent *hostptr;
    struct sockaddr_in server_address;
    int address_length;
    char protocol_msg[200] = "PUBLISH ";
    strcat(protocol_msg,message);

    debug_print("Publishing message to msgserv, IP: %s PORT: %d\n", msgserv_ip, msgserv_upt);

    fd = socket(AF_INET,SOCK_DGRAM,0);
    hostptr = gethostbyname(msgserv_ip);
    memset((void*)&server_address, (int)'\0',sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = ((struct in_addr *)(hostptr->h_addr_list[0]))->s_addr;
    server_address.sin_port = htons((u_short)msgserv_upt); 

    address_length = sizeof(server_address);
    sendto(fd, protocol_msg, strlen(protocol_msg)+1,0,(struct sockaddr*)&server_address,address_length);

    close(fd);
}

void print_servers(char* siip, int sipt){
    int fd;
    struct hostent *hostptr;
    struct sockaddr_in server_address;
    int address_length;
    char server_list[2048];

    fd = socket(AF_INET,SOCK_DGRAM,0);
    hostptr = gethostbyname(siip);
    memset((void*)&server_address, (int)'\0',sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = ((struct in_addr *)(hostptr->h_addr_list[0]))->s_addr;
    server_address.sin_port = htons((u_short)sipt);

    address_length = sizeof(server_address);
    sendto(fd, "GET_SERVERS", strlen("GET_SERVERS") + 1, 0, (struct sockaddr*) &server_address, address_length);

    address_length = sizeof(server_address);
    recvfrom(fd, server_list, sizeof(server_list), 0, (struct sockaddr*) &server_address, &address_length);
    close(fd);

    printf("%s\n", server_list);
}