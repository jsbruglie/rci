#include "sv_list.h"

ServerID* server_list_push(ServerID* head, char* si_name, char* si_ip, int si_upt, int si_tpt, int fd) {
    ServerID * new;
    new = (ServerID*) malloc(sizeof(ServerID));
    if(new == NULL){
        fprintf(stderr, "Malloc failed. Memory full\n");
        exit(EXIT_FAILURE);
    }
    //memset(new, 0, sizeof(ServerID));
    strcpy(new->name, si_name);
    strcpy(new->ip, si_ip);
    new->tpt = si_tpt;
    new->upt = si_upt;
    new->fd = fd;
    new->next = head;
    new->flag = !DELETE;
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
            
            sscanf(token, SSCANF_SERVERS_PARSING, si_name, si_ip, &si_upt, &si_tpt);  // NAMEIP_SIZE is 256
            // Prevent from adding myself to the server list
            if(strcmp(si_name, name) && si_upt != upt && si_tpt != tpt){            
                // printf("Token: %s\n", token);
                new_fd = tcp_connect(si_ip,si_tpt);
                if(new_fd != -1){
                    debug_print("INSERTING %d - %s %s %d %d\n", new_fd, si_name, si_ip, si_upt, si_tpt);
                    first = server_list_push(first, si_name, si_ip, si_upt, si_tpt, new_fd);
                    sprintf(buffer, "%s\n%s;%s;%d;%d", "ID", name, ip, upt, tpt);
                    int nbytes = write(new_fd, buffer, strlen(buffer) + 1);
                    if(nbytes == -1){
                        fprintf(stderr,"Write failed. Exiting...\n");
                        exit(EXIT_FAILURE);
                    }
                    //debug_print("CONNECTING TO %s %d\n\t%s\n", si_ip, si_upt, buffer);
                }else{
                    printf("Connect failed!Exiting...\n");
                    exit(EXIT_FAILURE);
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