#include "msg_table.h"

/* Message Table */

Message** create_msg_table(int size){

	Message** message_table = (Message**) malloc(sizeof(Message*) * size);
	int i;
	for(i = 0; i < size; i++){
		message_table[i] = NULL;
		//message_table[i] = (Message*) malloc(sizeof(Message));
		//message_table[i]->clock = size - i;
	}
	return message_table;
}

void delete_msg_table(Message** msg_table, int size){

	int i;
	for(i = 0; i < size; i++){
		if(msg_table[i] != NULL)
			free(msg_table[i]);
	}
	free(msg_table);
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

void sort_msg_table(Message** msg_table, int size, int items){
	quick_sort(msg_table, 0, items - 1);
}

/* Quick Sort */
void quick_sort(Message** msg_table, int l, int r){
	int partition_point, pivot;
	if(r > l){
		pivot = get_msg_clock(msg_table, r);
		partition_point = partition(msg_table, l, r, pivot);
		printf("QUICK: l - %d r - %d p - %d.\n", l, r, pivot);
		quick_sort(msg_table, l, partition_point - 1);
		quick_sort(msg_table, partition_point + 1, r);
	}
}

int partition(Message** msg_table, int l, int r, int pivot){
	int i = l - 1;
	int j = r;

	while(1){
   		while(get_msg_clock(msg_table, ++i) < pivot){}
		while(j > 0 && get_msg_clock(msg_table, --j) > pivot){}
      	if(i >= j)
			break;
      	swap_msg(msg_table, i, j);
	}
	swap_msg(msg_table, i, r);
	return i;
}

/* DEBUG */
void print_msg_table(Message** msg_table, int size){

	int i;
	for (i = 0; i < size; i++){
		if(msg_table[i] != NULL)
			printf("MSG: %d\n", msg_table[i]->clock);
	}
}