/** @file sv_list.h
 *  @brief Function prototypes for sv_list.c
 *
 *  Contains the definition of a server identity list structure
 *  as well the functions for its creation, alteration and deletion.
 *
 *  @author João Borrego
 *  @author Pedro Abreu
 *  @bug No known bugs.
 */

#ifndef SV_LIST
#define SV_LIST

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <limits.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>

#include "debug.h"
#include "defs.h"

#define DELETE 1 /**< Flag is set to `DELETE` when the entry is scheduled for deletion. */

#include "debug.h"

/**
 * @brief Structure for storing a list of server identities
 */
typedef struct _ServerID{
    char name[NAMEIP_SIZE]; /**< Server name */
    char ip[NAMEIP_SIZE];   /**< Server ip */
    int tpt;                /**< Server TCP port for monitoring incoming connections */
    int upt;                /**< Server UDP port for monitoring incoming connections */
    int fd;                 /**< Server file descriptor */
    int flag;               /**< Flag used for scheduling an entry for posterior deletion */ 
    struct _ServerID* next; /**< Pointer to the next entry in the list */
}ServerID;

/** @brief Inserts a server identity in a `ServerID` list
 *
 *  @param head Head of the list
 *  @param sv_name Server name
 *  @param sv_ip Server ip
 *  @param sv_upt Server UDP port for monitoring incoming connections
 *  @param sv_tpt Server TCP port for monitoring incoming connections
 *  @param fd Server file descriptor
 *  @return The head of the new list.
 */
ServerID* server_list_push(ServerID * head, char* sv_name, char* sv_ip, int sv_upt, int sv_tpt, int fd);

/** @brief Prints a given `ServerID` list
 *
 *  @param head The head of the list
 *  @return Void.
 */
void print_server_list(ServerID * head);

/** @brief Creates a TCP connection given a message server ip and TCP port
 *
 *  @param ip Message server ip
 *  @param tpt Message server TCP port
 *  @return the fd of the new connection on success, -1 otherwise.
 */
int tcp_connect(char* ip, int tpt);

/** @brief Creates a server list from the response of the ID Server
 *
 *  Given the response string of the Identity Server, it outputs a list of the active 
 *  message servers. The current instance of `msgserv` is not present in this list.
 *
 *  @param server_string The ID Server response to a request for listing active servers
 *  @param name The name of the current instance of `msgserv`
 *  @param ip The ip of the current instance of `msgserv`
 *  @param upt UDP port of the current instance of `msgserv`
 *  @param tpt TCP port of the current instance of `msgserv`
 *  @return The resulting list structure with the identity of the active servers.
 */
ServerID* create_server_list(char* server_string, char* name, char* ip, int upt, int tpt);

/** @brief Free a `ServerID` list structure
 *
 *  @param server_list The server list
 *  @return Void.
 */
void free_server_list(ServerID* server_list);

/** @brief Flags the entry corresponding to a given fd for deletion
 *
 *  @param fd The file descriptor corresponding to the server intended to be removed from the list
 *  @param first The first node of the list
 *  @return Void.
 */
void flag_for_deletion(int fd, ServerID* first);

/** @brief Deletes all entries with the `flag` field set to `DELETE`
 *
 *  @param first The first node of the list
 */
ServerID* delete_scheduled(ServerID* first);

#endif