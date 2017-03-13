/** @file msgserv_utils.h
 *  @brief Function prototypes for the functions used
 *  in msgserv as subroutines
 *
 *  This contains the functions used in msgserv that are at a
 *  lower level of abstraction than the ones in msgserv, namely 
 *  functions to parse argument, create tcp and udp clients/servers
 *  message sending and fd management. It also contains a structure
 *  that contains fd's used between rmb, sid and msgserv. Fds between
 *  msgserv are stored in a list of servers in each msgserv (see sv_list.c)
 *
 *  @author João Borrego
 *  @author Pedro Abreu
 *  @bug No known bugs.
 */

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

#include "msg_table.h"
#include "sv_list.h"
#include "debug.h"
#include "defs.h"

/**
 * @brief Structure for storing several file descriptors for better code readibility
 *
 *  Contains the file descriptors for handling
 *  - `stdin`
 *  - UDP connections with RMB clients (terminals)
 *  - UDP connections with the Identity Server
 *  - TCP connections requested by other messages server instances 
 */
typedef struct _FdStruct{
    int max;        /**< The highest number among file descriptors */
    int std_in;     /**< `stdin` (should always be 0) */
    int rmb_udp;    /**< UDP connections with RMB clients (terminals)*/
    int si_udp;     /**< UDP connections with the Identity Server*/
    int msg_tcp;    /**< TCP connections requested by other messages server instances */
}FdStruct;

/** @brief Parses command line arguments using getopt
 *
 *  @param argc main argument count
 *  @param argv main argument list
 *  @param _name Pass by reference of msgserver name
 *  @param _ip Pass by refereñce of msgserver ip
 *  @param _upt Pass by reference of msgserver upt
 *  @param _tpt Pass by refence of msgserver tpt
 *  @param _siip Pass by reference of identity server ip
 *  @param _sipt Pass by reference of identity server udp port
 *  @param _m Pass by reference of msgservmax number of messages
 *  @param _r Pass by reference of the timer(in seconds) for msgserv refresh
 *  @param read_set set that has the fds that have been set
 *  @return Void.
 */
void parse_args(int argc, char** argv, char** _name, char** _ip, int* _upt, int* _tpt,
                char** _siip, int* _sipt, int* _m, int* _r);

/** @brief Function to create a udp server.
 *  Consists of socket setup preambles and returns fd
 *
 *  @param port port for the UDP server to attend requests
 *  @return Int. 
 */
int create_udp_server(u_short port);

/** @brief Function to create a udp client. All it does is
 *  create an fd from socket() and returns it
 *         
 *  @return Int.
 */
int create_udp_client();

/** @brief Function to create a tcp server.
 *  Consists of socket setup preambles and returns fd
 *
 *  @param port port for the TCP server to attend requests
 *  @return Int.
 */
int create_tcp_server(u_short port);

/** @brief Creates a new fd from an accept of a TCP connect
 *  and returns it
 *
 *  @param fd file descriptor of the tcp server that 
 *  is doing the accepts
 *  @return Int.
 */
int accept_tcp_connection(int fd);

/** @brief Function for the refresh of the msgserv with
 *  an identity server
 *         
 *
 *  @param fd file descriptor of the msgserv as udp client
 *  @param name Name of the msgserv we are registering
 *  @param ip IP of the msgserv we are registering
 *  @param siip IP of the Identity Server we are registering with
 *  @param sipt Port of the Identity Server we are registering with
 *  @param upt UDP port of the msgserv we are registering
 *  @param tpt TCP port of the msgserv we are registering
 *  @return Void.
 */
void refresh(int fd, char* name, char* ip, char* siip, int sipt, int upt, int tpt);

/** @brief Function to send n messages of the message table over UDP
 *
 *  @param fd file descriptor to handle the UDP communcation
 *  @param client_addr_ptr this comes from the handler function (to simplify code)
 *  @param msg_table The message table of this msgserv
 *  @param n Number of messages the client requested
 *  @return Void.
 */
void send_messages_udp(int fd, struct sockaddr_in* client_addr_ptr, MessageTable* msg_table, int n);

/** @brief Function to send n messages or all of 
 *  the message table over TCP. Used in the communication
 *  between message servers to keep tables consistent
 *
 *  @param fd file descriptor of client to send the table to
 *  @param msg_table The message table of this msgserv
 *  @param n number of messages to send over tcp. Often this is all messages
 *  @param all flag that tells the function to send all messages
 *  @return Void.
 */
void send_messages_tcp(int fd, MessageTable* msg_table, int n, int all);

void safe_tcp_write(int fd, char* buffer, int size);

/** @brief Make a request to the Identity Server to get the servers
 *
 *  @param siip IP of the Identity Server
 *  @param sipt Port of the Idenity Server
 *  @param server_string string to get the server list by reference
 *  @return Void.
 */
void get_servers(char* siip, int sipt, char* server_string);

/** @brief Create the structure that has all but the tcp connection
 *  fds between servers. Alloc it and return
 *
 *  @param upt
 *  @param tpt
 *  @return FdStruct*.
 */
FdStruct* create_fd_struct(int upt, int tpt);

/** @brief Prints character ch at the current location
 *         of the cursor.
 *
 *  @param fd_struct structure that contains the fds to check
 *  @param read_set set that has the fds that have been set
 *  @return Void.
 */
void delete_fd_struct(FdStruct* fd);

/** @brief Prints character ch at the current location
 *         of the cursor.
 *
 *  @param fd_struct structure that contains the fds to check
 *  @param read_set set that has the fds that have been set
 *  @return Void.
 */
void init_fd_set(fd_set* set, FdStruct* fd, ServerID* sv);

/** @brief Prints character ch at the current location
 *         of the cursor.
 *
 *  @param fd_struct structure that contains the fds to check
 *  @param read_set set that has the fds that have been set
 *  @return Void.
 */
int fd_max(FdStruct* fd_struct, ServerID* sv_list);

#endif