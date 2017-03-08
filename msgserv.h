/** @file msgserv.h
 *  @brief Function prototypes the message server
 *
 *  This contains the functions used in msgserv, namely 
 *  the fuction to check the fds and its handles
 *
 *  @author João Borrego
 *  @author Pedro Abreu
 *  @bug No known bugs.
 */

#include "msgserv_utils.h"
#include "defs.h"

/** @brief Prints character ch at the current location
 *         of the cursor.
 *
 *  @param fd_struct structure that contains the fds to check
 *  @param read_set set that has the fds that have been set
 *  @return Void.
 */
void check_fd(FdStruct* fd_struct, fd_set* read_set);

/** @brief Handle to set the alarm
 *
 *  @param signal parameter
 *  @return Void.
 */
void handle_alarm(int sig);

/** @brief Handle to refresh the registration with the identity
 *  server. Uses the timer set in handle_alarm
 *
 *  @param fd_struct structure that contains the fds to check
 *  @return Void.
 */
void handle_si_refresh(FdStruct* fd_struct);

/** @brief Handle to process input from the terminal
 *  has comands: join, show_servers, show_messages, exit
 *
 *  @param fd_struct structure that contains the fds to check
 *  @return Void.
 */
void handle_terminal(FdStruct* fd_struct);

/** @brief Handle to check for incoming requests from users
 *  they can be get_messages or a publish
 *
 *  @param fd_rmb_udp is the integer to check for incoming
 *  messages
 *  @return Void.
 */
void handle_rmb_request(int fd_rmb_udp);

/** @brief Handle connections from other msg servers via TCP
 * 
 *
 *  @param fd_msg_tcp is the fd used to accept on
 *  @return Void.
 */
void handle_msg_connect(int fd_msg_tcp);

/**
 *
 *
 *
 *
 */
void handle_msg_activity(int fd_msg_tcp);

/** @brief Cleanup function to free memory and exit
 *
 *  @return Void.
 */
void cleanup(FdStruct* fd_struct);