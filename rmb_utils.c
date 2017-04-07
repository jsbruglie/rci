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
        siip = (char*) malloc(sizeof(char) * (strlen(*_siip) + 1));
        strcpy(siip, *_siip);
    }
    
    if (siip != NULL) *_siip = siip;
    if (sipt != -1) *_sipt = sipt;
}

/* Shows the latest n messages in the message server with which the terminal has a connection */
void show_latest_messages(int n, char* msgserv_ip, int msgserv_upt){
    
    char msg[PROTOCOL_SIZE] = "GET_MESSAGES ";
    char number[MAX_NUMBER_MSG] = {0};
    char buffer[LARGE_BUFFER_SIZE] = {0};

    snprintf(number, MAX_NUMBER_MSG, "%d", n);
    strcat(msg, number);

    if (n <= 0 || msgserv_ip == NULL || msgserv_upt < 0){
        /* Invalid Command */
        return;
    }

    request_udp(msgserv_ip, msgserv_upt, msg, strlen(msg), buffer, sizeof(buffer));

    printf("%s", buffer);
}

/* Fetches the identity of the active servers from the ID server */
void get_servers(char* siip, int sipt, char* server_list, int print){

    char msg[PROTOCOL_SIZE] = "GET_SERVERS";
    char buffer[LARGE_BUFFER_SIZE] = {0};

    request_udp(siip, sipt, msg, strlen(msg), buffer, sizeof(buffer));
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
    struct timeval tv;                      // Time value structure for timeout
    tv.tv_sec = RMB_TIMEOUT;                // Timeout value in seconds
    tv.tv_usec = 0;
    int n;                                  // Number of bytes written/read

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(struct timeval)) < 0){
        err_print("Could not configure socket timeout. Exiting...");
        exit(EXIT_FAILURE);
    }

    hostptr = gethostbyname(ip);
    memset((void*) &server_address, (int) '\0', sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = ((struct in_addr *)(hostptr->h_addr_list[0]))->s_addr;
    server_address.sin_port = htons((u_short)upt);

    address_length = sizeof(server_address);
    n = sendto(fd, send, send_size, 0, (struct sockaddr*) &server_address, address_length);
    if(n == -1){
        err_print("sendto failed. Exiting...");
        exit(EXIT_FAILURE);
    }
    
    address_length = sizeof(server_address);
    n = recvfrom(fd, recv, recv_size, 0, (struct sockaddr*) &server_address, &address_length);
    if(n == -1){
        err_print("recvfrom failed. It is possible that the message server shut down. Try again in a few moments."); 
        //exit(EXIT_FAILURE);
        close(fd);
        return;
    }

    /* Force the insertion of a trailing null character in the response */
    /* Ternary operator prevents buffer overflow */
    recv[(n == recv_size)? n-1 : n] = '\0';
    
    close(fd);

}

/* Choose a message server from a non-empty list of servers */
int pick_server(char* buffer, char* msgserv_name, char* msgserv_ip, int* msgserv_upt, int* msgserv_tpt){
    
    char* p1 = buffer + strlen("SERVERS\n"); 
    char p2[BUFFER_SIZE] = {0};
    
    /* Pick the first server of the list */
    sscanf(p1, "%[^\n]", p2);
    sscanf(p2, SSCANF_SERVERS_PARSING, msgserv_name, msgserv_ip, msgserv_upt, msgserv_tpt);

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

    /* Generate message to be sent */
    char protocol_msg[PROTOCOL_SIZE] = "PUBLISH ";
    char buffer[BUFFER_SIZE] = {0};
    strcat(buffer, protocol_msg);
    strcat(buffer, message);

    debug_print("Publishing message %s to msgserv ip: %s port: %d", message, msgserv_ip, msgserv_upt);

    /* Create socket and setup */
    fd = socket(AF_INET,SOCK_DGRAM, 0);
    hostptr = gethostbyname(msgserv_ip);
    memset((void*)&server_address, (int)'\0',sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = ((struct in_addr *)(hostptr->h_addr_list[0]))->s_addr;
    server_address.sin_port = htons((u_short) msgserv_upt); 
    address_length = sizeof(server_address);
    
    /* Trailing null character (\0) is not sent */
    int n = sendto(fd, buffer, strlen(buffer), 0, (struct sockaddr*) &server_address, address_length);
    if(n == -1){
        err_print("Could not publish message. Send failed. Check connection or try again.");
    }
    close(fd);
}

/* Determines whether a message is valid or not*/
int valid_msg(char* message){
    int lenght = strlen(message);
    if (lenght == 0 || lenght > MESSAGE_SIZE-1){
        return 0;
    }
    
    /* One could insert extra verifications here    */
    /*  if (condition_fail(message)) return 0       */

    return 1;
}