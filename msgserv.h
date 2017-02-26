#include <pthread.h>
#include "msgserv_utils.h"

#define BUFFER 1024 // Length of buffer for requests

/* Thread functions - each is executed by a concurrent thread */
void* udp_server();
void* interface();