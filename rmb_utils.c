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