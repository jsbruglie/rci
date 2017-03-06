/** @file rmb_utils.h
 *  @brief Function prototypes for rmb_utils.c
 *
 *  This contains the utility functions for a 
 *  terminal user, the RMB - Remote Message Board App
 *
 *  @author João Borrego
 *  @author Pedro Abreu
 *  @bug No known bugs.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "debug.h"

#define BUFFER_SIZE 2048 // Length of buffer for requests
#define PROTOCOL_SIZE 256 // Length of protocol macros
#define MESSAGE_SIZE 140 // Length of messages
#define COMMAND_SIZE 128 // Length of commands from the terminal
#define NAMEIP_SIZE 256

/** @brief Prints character ch at the current location
 *         of the cursor.
 *
 *  @param fd_struct structure that contains the fds to check
 *  @param read_set set that has the fds that have been set
 *  @return Void.
 */
void parse_args(int argc, char** argv, char** _siip, int* _sipt);

/** @brief Prints character ch at the current location
 *         of the cursor.
 *
 *  @param fd_struct structure that contains the fds to check
 *  @param read_set set that has the fds that have been set
 *  @return Void.
 */
void show_latest_messages(int n, char* msgserv_ip, int msgserv_upt);

/** @brief Prints character ch at the current location
 *         of the cursor.
 *
 *  @param fd_struct structure that contains the fds to check
 *  @param read_set set that has the fds that have been set
 *  @return Void.
 */
char* get_servers(char* siip, int sipt);

/** @brief Prints character ch at the current location
 *         of the cursor.
 *
 *  @param fd_struct structure that contains the fds to check
 *  @param read_set set that has the fds that have been set
 *  @return Void.
 */
void pick_server(char* buffer, char* msgserv_name, char* msgserv_ip, int* msgserv_upt, int* msgserv_tpt);

/** @brief Prints character ch at the current location
 *         of the cursor.
 *
 *  @param fd_struct structure that contains the fds to check
 *  @param read_set set that has the fds that have been set
 *  @return Void.
 */
void print_servers(char* siip, int sipt);

/** @brief Prints character ch at the current location
 *         of the cursor.
 *
 *  @param fd_struct structure that contains the fds to check
 *  @param read_set set that has the fds that have been set
 *  @return Void.
 */
void publish_msg(char* message, char* msgserv_ip, int msgserv_upt);