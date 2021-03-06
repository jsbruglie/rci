#include "msg_table.h"

MessageTable* create_msg_table(int size){

    MessageTable* message_table = (MessageTable*) malloc(sizeof(MessageTable));
    if(message_table == NULL){
        err_print("Malloc failed. Memory full");
        exit(EXIT_FAILURE);
    }
    message_table->table = (Message**) malloc(sizeof(Message*) * size);
    if(message_table->table == NULL){
        err_print("Malloc failed. Memory full");
        exit(EXIT_FAILURE);
    }
    message_table->size = size;
    message_table->items = 0;
    
    int i;
    for(i = 0; i < size; i++){
        message_table->table[i] = NULL;
    }

    return message_table;
}

void free_msg_table(MessageTable* msg_table){
    if (msg_table != NULL){
        int i;
        for(i = 0; i < msg_table->size; i++){
            if(msg_table->table[i] != NULL)
                free(msg_table->table[i]);
        }
        free(msg_table->table);
        free(msg_table);
    }
}

int insert_in_msg_table(MessageTable* msg_table, char* text, int clock){
    if(msg_table != NULL){
        if (msg_table_full(msg_table)){
            remove_oldest(msg_table);
            sort_msg_table(msg_table);
        } 
        if(insert_msg(msg_table, text, clock) == 0){
            sort_msg_table(msg_table);
            return 0;
        }   
    }
    return -1;
}

int insert_msg(MessageTable* msg_table, char* text, int clock){
    
    Message* new; 
    if (text != NULL && clock >= 0 && strlen(text) < MESSAGE_SIZE){
        new = (Message*) malloc(sizeof(Message));
        strcpy(new->text, text);
        new->clock = clock;
        msg_table->table[msg_table->items] = new;
        msg_table->items++;
        return 0;   
    }
    return -1;
}

int msg_table_full(MessageTable* msg_table){
    if (msg_table != NULL){
        if (msg_table->items != msg_table->size)
            return 0;
        return 1;
    }
    return -1;
}

int remove_oldest(MessageTable* msg_table){
    if (msg_table != NULL){
        free(msg_table->table[msg_table->items - 1]);
        msg_table->items--;
        return 0;
    }
    return -1;
}

/* Fetching messages from struct */

int size_latest_messages(MessageTable* msg_table, int n, int all, int include_clk){
    int i, items, length = 0;
    char clock[CLK_MAX_SIZE];
    if (msg_table != NULL){
        if (all){
            items = msg_table->items;
        }else{
            items = (msg_table->items < n)? msg_table->items : n; // minimum
        }
        if (include_clk){
            for (i = 0; i < items; i++){
                sprintf(clock, "%d", msg_table->table[i]->clock);
                length += (strlen(clock) + strlen(msg_table->table[i]->text) + MSG_FORMAT_CLK_SIZE);
            }
        }else{
            for (i = 0; i < items; i++){
                length += (strlen(msg_table->table[i]->text) + MSG_FORMAT_SIZE);
            }
        }    
        return length;
    }    
    return -1;
}

int get_latest_messages(MessageTable* msg_table, int n, int all, int include_clk, char* output){
    int i, items, length = 0;
    char buffer[MESSAGE_SIZE + CLK_MAX_SIZE + strlen(MSG_FORMAT_CLK) + 1];
    if (msg_table != NULL ){
        if (all){
            items = msg_table->items;
        }else{
            items = (msg_table->items < n)? msg_table->items : n; // minimum
        }
        if (include_clk){
            for (i = items - 1; i >= 0; i--){
                memset(buffer, (int)'\0', sizeof(buffer));
                sprintf(buffer, MSG_FORMAT_CLK, msg_table->table[i]->clock, msg_table->table[i]->text);
                strcat(output, buffer);
            }
        }else{
            for (i = items - 1; i >= 0; i--){
                memset(buffer, (int)'\0', sizeof(buffer));
                sprintf(buffer, MSG_FORMAT, msg_table->table[i]->text);
                strcat(output, buffer);
            }
        }
        return items;    
    }    
    return -1;
}

/* Functions for sorting the Message Table */

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
    int i = l - 1;  // left iterator
    int j = r;      // right iterator

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

void print_msg_table(MessageTable* msg_table, int LogicClock){

    int i;
    if (msg_table != NULL){
        printf("MESSAGE TABLE: %d/%d messages, with LC: %d\n", msg_table->items, msg_table->size, LogicClock);
        for (i = msg_table->items - 1; i >= 0; i--){
            if(msg_table->table[i] != NULL)
                printf("MSG: %d - %s\n", msg_table->table[i]->clock, msg_table->table[i]->text);
        }
    }
}

void fill_msg_table(MessageTable* message_table, char* buffer, int* LogicClock){
    int max_clock = *LogicClock;
    char* token;
    const char delimiter[2] = "\n";
    token = strtok(buffer, delimiter);
    while(token != NULL){
        if(strcmp("SMESSAGES",token) != 0){
            
            char msg[MESSAGE_SIZE];
            int clock;
            sscanf(token, SSCANF_MESSAGE_CLOCK_TABLE_INSERT , &clock, msg);
            if(clock > max_clock){
                max_clock = clock;
            }
            //Insert this message in the table
            insert_in_msg_table(message_table, msg, clock);
        }
        token = strtok(NULL, delimiter);
    }
    if (max_clock >= *LogicClock){
        *LogicClock = max_clock + 1;
    }
}