#include <pthread.h>
#include "msgserv_utils.h"

#define BUFFER 1024 // Length of buffer for requests
#define STDIN 0

/* Functions */
void check_fd(FdStruct* fd_struct, fd_set* read_set);

void handle_rmb_request(int fd_rmb_udp);
void handle_terminal();

void* refresh_registration();