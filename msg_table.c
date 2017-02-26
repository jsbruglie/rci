#include "msg_table.h"

MessageTable* create_msg_table(int size){

	MessageTable* message_table = (MessageTable*) malloc(sizeof(MessageTable));
	Message** table = (Message**) malloc(sizeof(Message*) * size);
	message_table->size = size;
	message_table->items = 0;
	//message_table->items = size;

	int i;
	for(i = 0; i < size; i++){
		table[i] = NULL;
		//table[i] = (Message*) malloc(sizeof(Message)); // DEBUG INIT
		//table[i]->clock = i;
	}
	message_table->table = table;

	return message_table;
}

void delete_msg_table(MessageTable* msg_table, int size){

	if (msg_table != NULL){
		int i;
		for(i = 0; i < size; i++){
			if(msg_table->table[i] != NULL)
				free(msg_table->table[i]);
		}
		free(msg_table);
	}
}

int get_msg_clock(Message** msg_table, int i){
	if (msg_table != NULL){
		if(msg_table[i] != NULL){
			return msg_table[i]->clock;
		}
	}
	return -1;
}

void swap_msg(Message** msg_table, int a, int b){
	if (msg_table != NULL || a == b){
		if(msg_table[a] != NULL && msg_table[b] != NULL){
			Message* temp = msg_table[a];
			msg_table[a] = msg_table[b];
			msg_table[b] = temp;
		}
	}
}

/* Quicksort table by descending value  of message clock */
void sort_msg_table(MessageTable* msg_table){
	
	if (msg_table != NULL)
		quick_sort(msg_table->table, 0, msg_table->items - 1);
}

void quick_sort(Message** m, int l, int r){
	int partition_point, pivot;
	if(r > l){
		pivot = get_msg_clock(m, r);
		partition_point = partition(m, l, r, pivot);
		quick_sort(m, l, partition_point - 1);
		quick_sort(m, partition_point + 1, r);
	}
}

int partition(Message** m, int l, int r, int pivot){
	int i = l - 1;	// left iterator
	int j = r;		// right iterator

	while(1){
   		while(get_msg_clock(m, ++i) > pivot){}
		while(j > 0 && get_msg_clock(m, --j) < pivot){}
      	if(i >= j)
			break;
      	swap_msg(m, i, j);
	}
	swap_msg(m, i, r);
	return i;
}

/* DEBUG */
void print_msg_table(MessageTable* msg_table){

	int i;
	if (msg_table != NULL){
		for (i = 0; i < msg_table->size; i++){
			if(msg_table->table[i] != NULL)
				printf("MSG: %d\n", msg_table->table[i]->clock);
		}
	}
}