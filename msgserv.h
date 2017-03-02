#include <pthread.h>
#include "msgserv_utils.h"

#define BUFFER 1024 // Length of buffer for requests
#define STDIN 0

/* Functions */
void init_fd_set(fd_set* set, int fd_stdin, int fd_si_udp, int fd_rmb_udp);

void handle_rmb_request(int fd_rmb_udp);
void handle_terminal();

void* refresh_registration();