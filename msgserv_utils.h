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
#include <signal.h>
#include <stdbool.h>

# include "msg_table.h"
#include "debug.h"

/* */
typedef struct _FdStruct{
	int max;
	int std_in;
	int rmb_udp;
	int si_udp;
	int msg_tcp;
}FdStruct;

#define STDIN 0

/* Functions */

void parse_args(int argc, char** argv, char** _name, char** _ip, int* _upt, int* _tpt,
                char** _siip, int* _sipt, int* _m, int* _r);

int create_udp_server(u_short port);
int create_udp_client();
int create_tcp_server(u_short port);

void refresh(int fd, char* name, char* ip, char* siip, int sipt, int upt, int tpt);
int send_messages(int fd, struct sockaddr_in* client_addr_ptr, MessageTable* msg_table, int n);
void register_in_server(char* name, char* ip, char* siip, int sipt, int upt, int tpt);
char* get_servers(char* siip, int sipt);

FdStruct* create_fd_struct(int upt, int tpt);
void delete_fd_struct(FdStruct* fd);
void init_fd_set(fd_set* set, FdStruct* fd);
int fd_max(FdStruct* fd_struct);

#endif