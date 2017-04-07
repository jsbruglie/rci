#include "sv_list.h"

ServerID* server_list_push(ServerID* head, char* sv_name, char* sv_ip, int sv_upt, int sv_tpt, int fd) {
    
    ServerID * new;
    new = (ServerID*) malloc(sizeof(ServerID));
    if(new == NULL){
        err_print("Malloc failed. Memory full");
        exit(EXIT_FAILURE);
    }
    strcpy(new->name, sv_name);
    strcpy(new->ip, sv_ip);
    new->tpt = sv_tpt;
    new->upt = sv_upt;
    new->fd = fd;
    new->next = head;
    new->flag = !DELETE;
    return new;
}

void print_server_list(ServerID* head){
    
    ServerID* current;
    printf("SERVER LIST:\n");
    for (current = head; current != NULL; current = current->next){
        printf("%s %s %d %d\n", current->name, current->ip, current->upt, current->tpt);
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
    
    int rv = connect(fd, (struct sockaddr*)&server_address,sizeof(server_address));

    debug_print("TCP connection: %d", rv);
    if(!rv)
        return fd;
    else
        return -1;
}

ServerID* create_server_list(char* server_string, char* name, char* ip, int upt, int tpt){
    
    ServerID* first = NULL;
    char* token;
    const char delimiter[2] = "\n";
    
    char si_name[NAMEIP_SIZE] = {0};
    char si_ip[NAMEIP_SIZE] = {0};
    int si_upt;
    int si_tpt;
    int new_fd;

    char buffer[BUFFER_SIZE] = {0};

    fflush(stdin);
    token = strtok(server_string, delimiter);

    while(token != NULL){
        if(strcmp("SERVERS",token) != 0){    
            sscanf(token, SSCANF_SERVERS_PARSING, si_name, si_ip, &si_upt, &si_tpt);
            /* Make sure to not add current msgserv instance to its own list */
            if(strcmp(si_name, name) && si_upt != upt && si_tpt != tpt){
                new_fd = tcp_connect(si_ip,si_tpt);
                if(new_fd != -1){
                    sprintf(buffer, "%s\n%s;%s;%d;%d", "SREG", name, ip, upt, tpt);
                    int nbytes = write(new_fd, buffer, strlen(buffer));
                    if(nbytes == -1){
                        err_print("Write failed. Exiting...");
                        exit(EXIT_FAILURE);
                    }
                    first = server_list_push(first, si_name, si_ip, si_upt, si_tpt, new_fd);
                    debug_print("Inserting %d - %s %s %d %d", new_fd, si_name, si_ip, si_upt, si_tpt);
                }
            }
        }
        token = strtok(NULL, delimiter);
    }
    return first;
}

void free_server_list(ServerID* first){
    
    ServerID* aux, *next;
    for(aux = first; aux != NULL; aux = next){
        next = aux->next;
        close(aux->fd);
        free(aux);
    }
}

void flag_for_deletion(int fd, ServerID* first){
    
    ServerID* aux;
    for (aux = first; aux != NULL; aux = aux->next){
        if (aux->fd == fd){
            aux->flag = DELETE;
            return;
        }
    }
}

ServerID* delete_scheduled(ServerID* first){

    ServerID* delete, *aux = first;

    // Remove items from the begining of the list
    while (first != NULL && first->flag == DELETE){
        
        delete = first;
        first = first->next;
        close(delete->fd);
        free(delete);
    }

    // Remove non-initial matching elements
    for (aux = first; aux != NULL; aux = aux->next)
    {
        while (aux->next != NULL && aux->next->flag == DELETE)
        {
            delete = aux->next;
            aux->next = delete->next;
            close(delete->fd);
            free(delete);
        }
    }
    return first;
}