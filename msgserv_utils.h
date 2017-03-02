#ifndef MSGSERV_UTILS
#define MSGSERV_UTILS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <limits.h>
#include <netinet/in.h>
#include <netdb.h>

# include "msg_table.h"
#include "debug.h"

/* */

/* Protocol MACROS */
#define SEND

/* Functions */

void parse_args(int argc, char** argv, char** _name, char** _ip, int* _upt, int* _tpt,
                char** _siip, int* _sipt, int* _m, int* _r);

int create_udp_server(u_short port);

int send_messages(int fd, struct sockaddr_in* client_addr_ptr, MessageTable* msg_table, int n);
void register_in_server(char* name, char* ip, char* siip, int sipt, int upt, int tpt);
char* get_servers(char* siip, int sipt);

#endif