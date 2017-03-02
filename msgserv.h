#include "msgserv_utils.h"

#define BUFFER 1024 // Length of buffer for requests
#define STDIN 0

/* Functions */
void check_fd(FdStruct* fd_struct, fd_set* read_set);

void handle_alarm(int sig);
void handle_si_refresh(FdStruct* fd_struct);

void handle_rmb_request(int fd_rmb_udp);
void handle_terminal(FdStruct* fd_struct);

void cleanup();