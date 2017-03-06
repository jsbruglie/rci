#include "sv_list.h"

ServerID* server_list_push(ServerID * head, char* si_name, char* si_ip, int si_upt, int si_tpt, int fd) {
    ServerID * new;
    new = (ServerID*) malloc(sizeof(ServerID));
    if(new == NULL){
        perror("server list error: ");
        exit(EXIT_FAILURE);
    }
    new->name = (char*)malloc(sizeof(char) * NAMEIP_SIZE);
    strcpy(new->name, si_name);
    new->ip = (char*)malloc(sizeof(char) * NAMEIP_SIZE);
    strcpy(new->ip, si_ip);
    new->tpt = si_tpt;
    new->upt = si_upt;
    new->fd = fd;
    new->next = head;
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
    printf("TCP connection: %d\n", rv);
    if(!rv)
        return fd;
    else
        return -1;
}

ServerID* create_server_list(ServerID* server_list, char* server_string, char* name, char* ip, int upt, int tpt){
    
    ServerID* first = server_list;
    char* token;
    const char delimiter[2] = "\n";
    
    char si_name[NAMEIP_SIZE], si_ip[NAMEIP_SIZE];
    int si_upt;
    int si_tpt;
    int fd;

    char buffer[BUFFER_SIZE];

    fflush(stdin);
    token = strtok(server_string, delimiter);

    while(token != NULL){
        if(strcmp("SERVERS",token) != 0){
            
            sscanf(token,"%256[^;];%256[^;];%d;%d", si_name, si_ip, &si_upt, &si_tpt);  // NAMEIP_SIZE is 256
            // Prevent from adding myself to the server list
            if(strcmp(si_name, name) && si_upt != upt && si_tpt != tpt){            
                // printf("Token: %s\n", token);
                fd = tcp_connect(si_ip,si_tpt);
                if(fd != -1){
                    first = server_list_push(first, si_name, si_ip, si_upt, si_tpt, fd);
                }
                sprintf(buffer, "%s %s;%s;%d;%d", "ID", name, ip, upt, tpt);
                write(fd, buffer, strlen(buffer) + 1);
            }
        }
        token = strtok(NULL, delimiter);
    }
    return first;
}

void free_server_list(ServerID* server_list){
    ServerID* next;
    ServerID* p;
    for(p=server_list;p!=NULL;p=next){
        next = p->next;
        free(p->name);
        free(p->ip);
        free(p);
    }
}

void delete_server_list(int del_fd, ServerID* server_list){
    ServerID* aux, *prev;
    ServerID* local = server_list;

    for(aux = server_list; aux != NULL ; aux = aux->next){
        if(aux->fd == del_fd){
            if(aux == server_list){
                server_list = local->next;
            }else{
                prev->next = aux->next;
                server_list = local;
            }
            free(aux->name);
            free(aux->ip);
            free(aux);
            return; //We can immediately return since we assume fd's are unique
        }
        prev = aux;
    }
}