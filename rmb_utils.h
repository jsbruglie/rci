#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "debug.h"

/* Functions */
void parse_args(int argc, char** argv, char** _siip, int* _sipt);

void show_latest_messages(int n, char* msgserv_ip, int msgserv_upt);
char* get_servers(char* siip, int sipt);
void pick_server(char* buffer, char* msgserv_name, char* msgserv_ip, int* msgserv_upt, int* msgserv_tpt);
void print_servers(char* siip, int sipt);