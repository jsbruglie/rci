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
 *  @return The message table structure correctly allocated.
 */
MessageTable* create_msg_table(int size);

/** @brief Deletes and frees a message table structure
 *
 *  @param msg_table The message table to be deleted
 *  @return Void.
 */
void free_msg_table(MessageTable* msg_table);

/** @brief Get the clock of a certain message at a certain index
 *
 *  @param msg_table The message table to be searched
 *  @param i The index of the message to be obtained
 *  @return The logic clock field.
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

/** @brief Inserts a new message into the message table
 *
 *  If the message table is full, the oldest message
 *  (with highest logic clock) is removed to ensure
 *  there is enough space.
 *  The table is guaranteed to be sorted afterwards
 *
 *  @param msg_table The table in which the message will be inserted
 *  @param text The text content of the message
 *  @param clock The message timestamp or logic clock field value
 *  @return 0 if successful, else -1.
 */
int insert_in_msg_table(MessageTable* msg_table, char* text, int clock);

/** @brief Inserts a message in the table (called by insert_in_msg_table)
 *  
 *  Simply allocates memory and creates the message structure
 *
 *  @param msg_table The table to insert in
 *  @param text The text content of the message
 *  @param clock The message timestamp or "clock"
 *  @return 0 if successful, else -1.
 */
int insert_msg(MessageTable* msg_table, char* text, int clock);

/** @brief Checks if a table is full
 *
 *  @param msg_table The table to check
 *  @return 1 if table is full, else 0.
 */
int msg_table_full(MessageTable* msg_table);

/** @brief Removes the oldest element from a table
 *
 *  @param msg_table The table from which you want to remove the oldest element 
 *  @return 0 if successful, else -1.
 */
int remove_oldest(MessageTable* msg_table);

/* Fetch messages from table */

/** @brief Returns the cumulative size of the latest n messages (or all messages)
 *
 *  Needed to allocate an external buffer, prior to fetching all the messages from the table
 *
 *  @param msg_table The message table
 *  @param n number of messages to retireve
 *  @param all Flag to state if we desire all the messages. Overrides n if set to ALL_MSGS
 *  @param include_clk Flag to state if we desire logic clock information
 *  @return The desired size if successful, else -1.
 */
int size_latest_messages(MessageTable* msg_table, int n, int all, int include_clk);

/** @brief Get the latest messages (or all messages)
 *
 *  @attention The messages are printed by inverse order, i.e., descending logic clock
 *
 *  @param msg_table The message table
 *  @param n number of messages to retireve
 *  @param all Flag to state if we desire all the messages. Overrides n if set to ALL_MSGS
 *  @param include_clk Flag to state if we desire logic clock information
 *  @param output Message list string output, set internally
 *  @return 0 if succesfful, else -1.
 */
int get_latest_messages(MessageTable* msg_table, int n, int all, int include_clk, char* output);

/* Quick Sort */

/** @brief Sorts the message table by ascending logic clock
 *
 *  @param msg_table The message table to be sorted 
 *  @return Void.
 */
void sort_msg_table(MessageTable* msg_table);

/** @brief Recursive quicksort implementation
 *
 *  @param msg_table The message table to be sorted
 *  @param l The left iterator
 *  @param r The right iterator
 *  @return Void.
 */
void quick_sort(Message** msg_table, int l, int r);

/** @brief Partition routine for quicksort
 *
 *  @param msg_table The message table to be sorted
 *  @param l The left iterator
 *  @param r The right iterator
 *  @param pivot The quicksort pivot element
 *  @return The partition point. 
 */
int partition(Message** msg_table, int l, int r, int pivot);

/* Application specific */

/** @brief Prints the list of messages on the msgserv and also the 
 *  the current Logic Clock of the msgserv
 *
 *  @param msg_table The table to print
 *  @param LogicClock logic clock of the msgserv
 *  @return Void.
 */
void print_msg_table(MessageTable* msg_table, int LogicClock);

/* Update message table when a message is received */

/** @brief Fills the message table with a string that comes
 *  from the communication protocol between message servers
 *
 *  @param msg_table The table to be filled
 *  @param buffer Buffer with message table information from a msgserv
 *  @param LogicClock logic clock of the msgserv, altered internally
 *  @return Void.
 */
void fill_msg_table(MessageTable* msg_table, char* buffer, int* LogicClock);

#endif