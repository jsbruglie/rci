#ifndef MSG_TABLE
#define MSG_TABLE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Message struct type */
typedef struct _Message{
	char text[140];
	int clock;
}Message;

/* Message Table struct type */
typedef struct _MessageTable{
	int size;
	int items;
	Message** table;
}MessageTable;

/* Functions */

MessageTable* create_msg_table(int size);
void delete_msg_table(MessageTable* msg_table, int size);
int get_msg_clock(Message** msg_table, int i);
void swap_msg(Message** msg_table, int a, int b);

/* Quick Sort */
void sort_msg_table(MessageTable* msg_table);
void quick_sort(Message** msg_table, int l, int r);
int partition(Message** msg_table, int l, int r, int pivot);

/* DEBUG */
void print_msg_table(MessageTable* msg_table);

#endif