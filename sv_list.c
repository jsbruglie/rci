#include "sv_list.h"

ServerID* server_list_push(ServerID* head, char* si_name, char* si_ip, int si_upt, int si_tpt, int fd) {
    ServerID * new;
    new = (ServerID*) malloc(sizeof(ServerID));
    if(new == NULL){
        perror("server list error: ");
        exit(EXIT_FAILURE);
    }
    strcpy(new->name, si_name);
    strcpy(new->ip, si_ip);
    new->tpt = si_tpt;
    new->upt = si_upt;
    new->fd = fd;
    new->next = head;
    debug_print("PUSH %zu/%zu\n", sizeof(new->name) + sizeof(new->ip) + sizeof(new->upt) + sizeof(new->fd) + sizeof(new->next), sizeof(ServerID));
    return new;
}

void print_server_list(ServerID* head){
    ServerID* current;
    debug_print("PRINTING CURRENT SERVER LIST\n");
    for (current = head; current != NULL; current = current->next){
        debug_print("SERVER ID: %s %s %d %d\n", current->name, current->ip, current->upt, current->tpt);
    }

}

int tcp_connect(char* ip, int tpt){
    int fd = socket(AF_INET,SOCK_STREAM,0);
    struct hostent *hostptr;
    struct sockaddr_in server_address;
    hostptr = gethostbyname(ip);
    memset((void*)&server_address,(int)'\0',sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = ((struct in_addr *)(hostptr->h_addr_list[0]))->s_addr;
    server_address.sin_port = htons((u_short)tpt);
    int rv = connect(fd, (struct sockaddr*)&server_address,sizeof(server_address)); //blocked here
    debug_print("TCP connection: %d\n", rv);
    if(!rv)
        return fd;
    else
        return -1;
}

ServerID* create_server_list(char* server_string, char* name, char* ip, int upt, int tpt){
    
    ServerID* first = NULL;
    char* token;
    const char delimiter[2] = "\n";
    
    char si_name[NAMEIP_SIZE], si_ip[NAMEIP_SIZE];
    int si_upt;
    int si_tpt;
    int new_fd;

    char buffer[BUFFER_SIZE];

    fflush(stdin);
    token = strtok(server_string, delimiter);

    while(token != NULL){
        if(strcmp("SERVERS",token) != 0){
            
            sscanf(token,"%256[^;];%256[^;];%d;%d", si_name, si_ip, &si_upt, &si_tpt);  // NAMEIP_SIZE is 256
            // Prevent from adding myself to the server list
            if(strcmp(si_name, name) && si_upt != upt && si_tpt != tpt){            
                // printf("Token: %s\n", token);
                new_fd = tcp_connect(si_ip,si_tpt);
                if(new_fd != -1){
                    debug_print("INSERTING %d - %s %s %d %d\n", new_fd, si_name, si_ip, si_upt, si_tpt);
                    first = server_list_push(first, si_name, si_ip, si_upt, si_tpt, new_fd);
                    sprintf(buffer, "%s\n%s;%s;%d;%d", "ID", name, ip, upt, tpt);
                    write(new_fd, buffer, strlen(buffer) + 1);
                    //debug_print("CONNECTING TO %s %d\n\t%s\n", si_ip, si_upt, buffer);
                }
            }
        }
        token = strtok(NULL, delimiter);
    }
    return first;
}

void free_server_list(ServerID* server_list){
    ServerID* aux;
    for(aux = server_list; aux != NULL; aux = aux->next){
        close(aux->fd);
        free(aux);
    }
}

void delete_from_server_list(int del_fd, ServerID** server_list){
    ServerID* aux, *prev, *local;
    
    if (server_list != NULL){

        local = *(server_list);

        /* Linear search until the desired node is found */
        for(aux = local; aux != NULL ; aux = aux->next){
            if(aux->fd == del_fd){
                /* If the node to delete is the first one */
                if(aux == local){
                    *(server_list) = local->next;
                }else{
                    prev->next = aux->next;
                    *(server_list) = local;
                }
                debug_print("DELETING %d %s %s\n", aux->fd, aux->name, aux->ip);
                close(aux->fd);
                free(aux);
                return; //We can immediately return since we assume fd's are unique
            }
            prev = aux;
        }
    }

    /*
        if ((*server_list)->fd == del_fd){
            
            *(server_list) = (*server_list)->next;
            debug_print("DELETING %d %s %s\n", aux->fd, aux->name, aux->ip);
            close(aux->fd);
            free(aux);
            return;
        }

        local = (*server_list)->next;
        prev = *(server_list);
        
        while(local != NULL && prev != NULL){
            if (local->fd == del_fd){
                aux = local;
                prev->next = local->next;
                debug_print("DELETING %d %s %s\n", aux->fd, aux->name, aux->ip);
                close(aux->fd);
                free(aux);
                return;     
            }
            prev = local;
            local = local->next;
        }
    */
}