/** @file rmb_utils.h
 *  @brief Function prototypes for rmb_utils.c
 *
 *  Contains the utility functions for a 
 *  terminal user, the RMB - Reliable Message Board App
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
#include "defs.h"

#define PRINT 1 // Flag for get servers 

/** @brief Parses the optional arguments for execution from 
 *         the command line arguments. 
 *
 *  @param argc
 *  @param argv
 *  @param _siip ID server ip string pointer
 *  @param _sipt ID server UDP port pointer
 *  @return Void.
 */
void parse_args(int argc, char** argv, char** _siip, int* _sipt);

/** @brief Shows the latest n messages in the message server with 
 *         which the terminal has a connection.
 *
 *  @param n number of messages to retrieve
 *  @param msgserv_ip message server ip string
 *  @param msgserv_upt message server UDP port
 *  @return Void.
 */
void show_latest_messages(int n, char* msgserv_ip, int msgserv_upt);

/** @brief Fetches the identity of the active servers from the 
 *         ID server.
 *
 *  @param siip ID server ip string
 *  @param sipt ID server UDP port
 *  @param server_list list of servers returned by ID server; set internally
 *  @param print whether the function should print the server response or not
 *  @return Void.
 */
void get_servers(char* siip, int sipt, char* server_list, int print);


/** @brief Generic UDP request. Performs a request and awaits a response.
 *
 *  @param ip server ip
 *  @param upt server UDP port
 *  @param send message to be sent
 *  @param send_size size of message to be sent
 *  @param recv server response
 *  @param recv_size maximum size of server response
 *  @return Void.
 */
void request_udp(char* ip, int upt, char* send, int send_size, char* recv, int recv_size);

/** @brief Choose a message server from a non-empty list of servers.
 *
 *  @param buffer
 *  @param msgserv_name message server name; set internally
 *  @param msgserv_ip message server ip; set internally
 *  @param msgserv_upt message server UDP port; set internally
 *  @param msgserv_tpt message server TCP port; set internally  
 *  @return 0 on success, -1 otherwise.
 */
int pick_server(char* buffer, char* msgserv_name, char* msgserv_ip, int* msgserv_upt, int* msgserv_tpt);

/** @brief Publishes a message to a given message server.
 *
 *  @param message message to be published
 *  @param msgserv_ip message server ip
 *  @param msgserv_ip message server UDP port
 *  @return Void.
 */
void publish_msg(char* message, char* msgserv_ip, int msgserv_upt);