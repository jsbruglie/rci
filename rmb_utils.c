#include "rmb_utils.h"

/* Parses the optional arguments for execution from the command line arguments */
void parse_args(int argc, char** argv, char** _siip, int* _sipt){

    char* siip = NULL;
    int sipt = -1;
    char opt;

    while( (opt = getopt(argc, argv, "i:p:")) != -1 ){
        switch(opt){

            case 'i':
                siip = (char*) malloc(sizeof(char) * (strlen(optarg) + 1));
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
    if (siip == NULL){
        siip = (char*) malloc(sizeof(char) * (strlen(siip)));
        strcpy(siip, *_siip);
    }
    
    if (siip != NULL) *_siip = siip;
    if (sipt != -1) *_sipt = sipt;
}

/* Shows the latest n messages in the message server with which the terminal has a connection */
void show_latest_messages(int n, char* msgserv_ip, int msgserv_upt){
    
    char msg[PROTOCOL_SIZE] = "GET_MESSAGES ";
    char number[MAX_NUMBER_MSG];
    char buffer[LARGE_BUFFER_SIZE];

    snprintf(number, MAX_NUMBER_MSG, "%d", n);
    strcat(msg, number);

    if (n < 0 || !strcmp(msgserv_ip,"") || msgserv_upt < 0){
        // Invalid Command
        return;
    }

    request_udp(msgserv_ip, msgserv_upt, msg, sizeof(msg), buffer, sizeof(buffer));

    printf("%s", buffer);
}

/* Fetches the identity of the active servers from the ID server */
void get_servers(char* siip, int sipt, char* server_list, int print){

    char msg[PROTOCOL_SIZE] = "GET_SERVERS";
    char buffer[LARGE_BUFFER_SIZE];

    request_udp(siip, sipt, msg, sizeof(msg), buffer, sizeof(buffer));

    memset(server_list, 0, sizeof(server_list));
    strcpy(server_list, buffer);

    if(print){
        printf("%s", server_list);
    }
}

/* Generic UDP request. Performs a request and awaits a response */
void request_udp(char* ip, int upt, char* send, int send_size, char* recv, int recv_size){

    int fd;                                 // Socket file descriptor
    struct hostent *hostptr;                // Host name structure pointer
    struct sockaddr_in server_address;      // Server address struct
    int address_length;                     // Address length

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    hostptr = gethostbyname(ip);
    memset((void*) &server_address, (int) '\0', sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = ((struct in_addr *)(hostptr->h_addr_list[0]))->s_addr;
    server_address.sin_port = htons((u_short)upt);

    address_length = sizeof(server_address);
    int n = sendto(fd, send, send_size, 0, (struct sockaddr*) &server_address, address_length);
    if(n == -1){
        fprintf(stderr, "Sendto failed. Exiting.\n");
        exit(EXIT_FAILURE);
    }
    
    address_length = sizeof(server_address);
    recvfrom(fd, recv, recv_size, 0, (struct sockaddr*) &server_address, &address_length);
    if(n == -1){
        fprintf(stderr, "Recvfrom failed. Exiting.\n"); 
        exit(EXIT_FAILURE);
    }
    
    close(fd);

}

/* Choose a message server from a non-empty list of servers */
int pick_server(char* buffer, char* msgserv_name, char* msgserv_ip, int* msgserv_upt, int* msgserv_tpt){
    // Picks the first server of the list
    char* p1 = buffer + strlen("SERVERS\n"); 
    char p2[BUFFER_SIZE] = "";
    
    sscanf(p1, "%[^\n]", p2);
    sscanf(p2, "%256[^;];%256[^;];%d;%d", msgserv_name, msgserv_ip, msgserv_upt, msgserv_tpt);

    if(*msgserv_upt != -1 && msgserv_ip != NULL){
        return 0;
    }
    return -1;
}

/* Publishes a message to a given message server */
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
    if(n == -1){
        fprintf(stderr, "Could not publish message. Send failed. Exiting.\n");
        exit(EXIT_FAILURE);
    }

    close(fd);
}