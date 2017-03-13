/** @file msg_table.h
 *  @brief Function prototypes for msg_table.c
 *
 *  Contains the definition of message and message table structures
 *  as well the functions for insertion, filling and freeing
 *
 *  @author João Borrego
 *  @author Pedro Abreu
 *  @bug No known bugs.
 */

#ifndef MSG_TABLE
#define MSG_TABLE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "debug.h"
#include "defs.h"

#define CLK_MAX_SIZE 20             /**< The maximum number of chars to represent the `clock` field as a string */

#define ALL_MSGS 1                  /**< Parameter to specify whether we want all the messages or not */
#define INCLUDE_CLK 1               /**< Parameter to specify if we want to include the `clock` field in exported data */


/**
 * @brief Structure for a single message
 */
typedef struct _Message{
    char text[MESSAGE_SIZE];    /**< The actual text whitin a message. It is limited to `MESSAGE_SIZE` characters */
    int clock;                  /**< The respective clock data. Serves a synchronisation purpose. */
}Message;

/**
 * @brief Structure for the message table
 */
typedef struct _MessageTable{
    int size;           /**< The size of the table: maximum number of stored messages. */
    int items;          /**< The number of occupied position in the table. */
    Message** table;    /**< The actual messsage table. */
}MessageTable;


/* Create/free message table struct */

/** @brief Creates a new message table from size
 *
 *  @param size Size of the table you desire
 *  @return The message table structure correctly alloced
 */
MessageTable* create_msg_table(int size);

/** @brief Deletes and frees a message table structure
 *
 *  @param msg_table The message table you want to return
 *  @return Void.
 */
void free_msg_table(MessageTable* msg_table);

/** @brief Get the clock of a certain message at a certain index
 *
 *  @param msg_table The table to search in
 *  @param i The index of the message you want the clock
 *  @return The head of the new list.
 */
int get_msg_clock(Message** msg_table, int i);

/** @brief Swap two messages in the table
 *
 *  @param msg_table The table to do the swap in
 *  @param a index of the first message
 *  @param b index of the second message
 *  @return Void.
 */
void swap_msg(Message** msg_table, int a, int b);

/* Insert message in table */

/** @brief Inserts a new message into the message table at the 
 *  last available place with a check if the table is full, and if it
 *  is removes the oldest before inserting the new. Also does a sorting
 *  after insertion to keep the table ordered.
 *
 *  @param msg_table The table to insert in
 *  @param text The text content of the message
 *  @param clock The message timestamp or "clock"
 *  @return 0 if successful, else -1
 */
int insert_in_msg_table(MessageTable* msg_table, char* text, int clock);

/** @brief Inserts a message in the table, called by insert_in_msg_table,
 *  simply allocs memory and creates the message
 *
 *  @param msg_table The table to insert in
 *  @param text The text content of the message
 *  @param clock The message timestamp or "clock"
 *  @return 0 if successful, else -1
 */
int insert_msg(MessageTable* msg_table, char* text, int clock);

/** @brief Checks if a table is full
 *
 *  @param msg_table The table to check
 *  @return 1 if table is full, else 0
 */
int msg_table_full(MessageTable* msg_table);

/** @brief Removes the oldest element from a table
 *
 *  @param fmsg_table The table from which you want to remove the oldest element 
 *  @return 0 if successful, else -1
 */
int remove_oldest(MessageTable* msg_table);

/* Fetch messages from table */

/** @brief Returns the cumulative size of the latest n messages (or all messages)
 *
 *  @param msg_table Message to go through
 *  @param n number of messages to get
 *  @param all this is the flag to set all parameters
 *  @param include_clk set the flag to include clocks in the count
 *  @return The size, returns -1 if failed
 */
int size_latest_messages(MessageTable* msg_table, int n, int all, int include_clk);

/** @brief Get the latest messages, using size_latest_messages, flags are passed to it
 *
 *  @param msg_table Message to go through
 *  @param n number of messages to get
 *  @param all this is the flag to set all parameters
 *  @param include_clk set the flag to include clocks in the count
 *  @param output string passed by reference, filled with the list
 *  @return 0 if succesfful, -1 if not.
 */
int get_latest_messages(MessageTable* msg_table, int n, int all, int include_clk, char* output);

/* Quick Sort */

/** @brief Inserts a server identity in a `ServerID` list
 *
 *  @param head Head of the list
 *  @return The head of the new list.
 */
void sort_msg_table(MessageTable* msg_table);

/** @brief Inserts a server identity in a `ServerID` list
 *
 *  @param head Head of the list
 *  @param
 *  @param 
 *  @return The head of the new list.
 */
void quick_sort(Message** msg_table, int l, int r);

/** @brief Inserts a server identity in a `ServerID` list
 *
 *  @param head Head of the list
 *  @param si_name Server name
 *  @param si_ip Server ip
 *  @return The head of the new list.
 */
int partition(Message** msg_table, int l, int r, int pivot);

/* DEBUG */

/** @brief Prints the list of messages on the msgserv and also the 
 *  the current Logic Clock of the msgserv, this function is mainly for
 *  debug.
 *
 *  @param msg_table Table to print
 *  @param LogicClock logic clock of the msgserv
 *  @return Void.
 */
void print_msg_table(MessageTable* msg_table, int LogicClock);

/* Update message table when a message is received */

/** @brief Fills the message table with a string that comes
 *  from the communication protocol between message servers
 *
 *  @param msg_table The table to fill
 *  @param buffer This is the list of messages you received and want to use to fill the table
 *  @param LogicClock logic clock of the msgserv, will have to be altered after filling
 *  @return Void.
 */
void fill_msg_table(MessageTable* msg_table, char* buffer, int* LogicClock);

#endif