#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

/* Functions */
void parse_args(int argc, char** argv, char** _name, char** _ip, int* _upt, int* _tpt);
