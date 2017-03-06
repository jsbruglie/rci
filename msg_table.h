#ifndef MSG_TABLE
#define MSG_TABLE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define CLK_MAX_SIZE 20

#define ALL_MSGS 1					// Send the whole message table
#define INCLUDE_CLK 1				// Include clock parameter
#define MSG_FORMAT_CLK "%d;%s\n"	// [clk];[text]\n
#define MSG_FORMAT_CLK_SIZE 2
#define MSG_FORMAT "%s\n" 			// [text]\n
#define MSG_FORMAT_SIZE 1

#define BUFFER_SIZE 2048 	// Length of buffer for requests
#define PROTOCOL_SIZE 256	// Length of protocol macros
#define MESSAGE_SIZE 140	// Length of messages
#define COMMAND_SIZE 128	// Length of commands from the terminal

/* Message struct type */
typedef struct _Message{
    char text[MESSAGE_SIZE];
    int clock;
}Message;

/* Message Table struct type */
typedef struct _MessageTable{
    int size;
    int items;
    Message** table;
}MessageTable;

/* Functions */

/* Create/free message table struct */
MessageTable* create_msg_table(int size);
void free_msg_table(MessageTable* msg_table);
int get_msg_clock(Message** msg_table, int i);
void swap_msg(Message** msg_table, int a, int b);

/* Insert message in table */
int insert_in_msg_table(MessageTable* msg_table, char* text, int clock);
int insert_msg(MessageTable* msg_table, char* text, int clock);
int msg_table_full(MessageTable* msg_table);
int remove_oldest(MessageTable* msg_table);

/* Fetch messages from table */
int size_latest_messages(MessageTable* msg_table, int n, int all, int include_clk);
int get_latest_messages(MessageTable* msg_table, int n, int all, int include_clk, char* output);

/* Quick Sort */
void sort_msg_table(MessageTable* msg_table);
void quick_sort(Message** msg_table, int l, int r);
int partition(Message** msg_table, int l, int r, int pivot);

/* DEBUG */
void print_msg_table(MessageTable* msg_table);

/* Initial filling*/
void fill_table(MessageTable* msg_table, char* buffer, int* LogicClock);
/* Update message table when a message is received */
void update_msg_table(MessageTable* message_table, char* buffer, int* LogicClock);

#endif