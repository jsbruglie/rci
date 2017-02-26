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

#include "debug.h"

/* Message struct type */
typedef struct _Message{
	char text[140];
	int clock;
}Message;

/* Functions */

/* Message Server */
void parse_args(int argc, char** argv, char** _name, char** _ip, int* _upt, int* _tpt,
				char** _siip, int* _sipt, int* _m, int* _r);

int create_udp_server(u_short port);

/* Message Table */
Message** create_msg_table(int size);
void delete_msg_table(Message** msg_table, int size);

/* Quick Sort */
void quick_sort(Message** msg_table, int l, int r);
int partition(Message** msg_table, int l, int r, int pivot);
void sort_msg_table(Message** msg_table, int size, int items);
int get_msg_clock(Message** msg_table, int i);
void swap_msg(Message** msg_table, int a, int b);

/* DEBUG */
void print_msg_table(Message** msg_table, int size);

#endif