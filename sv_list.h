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

#define DELETE 1 //For deletion flag

#include "debug.h"

/* Message Server ÍD structure */

typedef struct _ServerID{
    char name[NAMEIP_SIZE];
    char ip[NAMEIP_SIZE];
    int tpt;
    int upt;
    int fd;
    int flag;				// Flag entry for deletion 
    struct _ServerID* next;
}ServerID;

/** @brief Prints character ch at the current location
 *         of the cursor.
 *
 *  @param fd_struct structure that contains the fds to check
 *  @param read_set set that has the fds that have been set
 *  @return Void.
 */
ServerID* server_list_push(ServerID * head, char* si_name, char* si_ip, int si_upt, int si_tpt, int fd);

/** @brief Prints character ch at the current location
 *         of the cursor.
 *
 *  @param fd_struct structure that contains the fds to check
 *  @param read_set set that has the fds that have been set
 *  @return Void.
 */
void print_server_list(ServerID * head);

/** @brief Prints character ch at the current location
 *         of the cursor.
 *
 *  @param fd_struct structure that contains the fds to check
 *  @param read_set set that has the fds that have been set
 *  @return Void.
 */
int tcp_connect(char* ip, int tpt);

/** @brief Prints character ch at the current location
 *         of the cursor.
 *
 *  @param fd_struct structure that contains the fds to check
 *  @param read_set set that has the fds that have been set
 *  @return Void.
 */
ServerID* create_server_list(char* server_string, char* name, char* ip, int upt, int tpt);

/** @brief Prints character ch at the current location
 *         of the cursor.
 *
 *  @param fd_struct structure that contains the fds to check
 *  @param read_set set that has the fds that have been set
 *  @return Void.
 */
void free_server_list(ServerID* server_list);

/** @brief Prints character ch at the current location
 *         of the cursor.
 *
 *  @param fd_struct structure that contains the fds to check
 *  @param read_set set that has the fds that have been set
 *  @return Void.
 */
void flag_for_deletion(int fd, ServerID* first);

/** @brief Prints character ch at the current location
 *         of the cursor.
 *
 *  @param fd_struct structure that contains the fds to check
 *  @param read_set set that has the fds that have been set
 *  @return Void.
 */
ServerID* delete_scheduled(ServerID* first);

#endif