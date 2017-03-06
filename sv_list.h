#ifndef SV_LIST
#define SV_LIST

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <limits.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>

#include "debug.h"

#define BUFFER_SIZE 2048  // Length of buffer for requests
#define PROTOCOL_SIZE 256 // Length of protocol macros
#define MESSAGE_SIZE 140  // Length of messages
#define COMMAND_SIZE 128  // Length of commands from the terminal
#define NAMEIP_SIZE 256   // Length for the name of servers and their ip

#define MAX_SIZE 256

/* Message server identity structure type */

#include "debug.h"

/* Message Server Identity structure type */

typedef struct _ServerID{
    char* name;
    char* ip;
    int tpt;
    int upt;
    int fd;
    struct _ServerID* next;
}ServerID;

/* Functions */

ServerID* server_list_push(ServerID * head, char* si_name, char* si_ip, int si_upt, int si_tpt, int fd);
void print_server_list(ServerID * head);
int tcp_connect(char* ip, int tpt);
ServerID* create_server_list(ServerID* server_list, char* server_string, char* name, int upt, int tpt);
void free_server_list(ServerID* server_list);
void delete_server_list(int del_fd, ServerID* server_list);	

#endif