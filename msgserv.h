/** @file msgserv.h
 *  @brief Function prototypes fot msgserv.c
 *
 *  @author João Borrego
 *  @author Pedro Abreu
 *  @bug No known bugs.
 */

#include "msgserv_utils.h"
#include "defs.h"

/** @brief Checks and handles different inputs.
 *
 *  Checks for activity in the monitored input channels, namely
 *  those mapped by file descriptors in `fd_struct` and the 
 *  server ID list structures.
 *
 *  @param fd_struct Structure that contains the fds to check
 *  @param read_set The list of monitored file descriptors
 *  @return Void.
 */
void check_fd(FdStruct* fd_struct, fd_set* read_set);

/** @brief Handle to set the alarm
 *
 *  Sets the global variable `timer` to 1.
 *
 *  @param signal parameter
 *  @return Void.
 */
void handle_alarm(int signal);

/** @brief Handles to refresh the registration with the identity
 *  server.
 *
 *  Uses the timer set in `handle_alarm` in order to be repeated periodically
 *  ideally, with an interval of `r` seconds between registry attempts.
 *
 *  @param fd_struct Structure that contains monitored file descriptors 
 *  @return Void.
 */
void handle_si_refresh(FdStruct* fd_struct);

/** @brief Handle input from the terminal (`stdin`)
 *  
 *  Valid commands include
 *  -`join`
 *  -`show_servers`
 *  -`show_messages`
 *  -`exit`
 *
 *  @param fd_struct Structure that contains monitored file descriptors
 *  @return Void.
 */
void handle_terminal(FdStruct* fd_struct);

/** @brief Handle incoming requests from terminals.
 *
 *  Valid requests include:
 *  -`get_messages`
 *  -`publish`
 *
 *  @param fd_rmb_udp file descriptor for monitoring incoming messages
 *  @return Void.
 */
void handle_rmb_request(int fd_rmb_udp);

/** @brief Handle attempts from other message servers to connect via TCP
 * 
 *  @param fd_msg_tcp file descriptor for monitoring TCP connect requests
 *  @return Void.
 */
void handle_msg_connect(int fd_msg_tcp);

/** @brief Handle other message server instances' requests
 *
 *  @param fd_msg_tcp file descriptor for monitoring TCP incoming messages
 *  @ return Void.
 */
void handle_msg_activity(int fd_msg_tcp);

/** @brief Cleanup function to free allocated memory
 *
 *  @param fd_struct structure that contains monitored file descriptors
 *  @return Void.
 */
void cleanup(FdStruct* fd_struct);

void debug_lc();