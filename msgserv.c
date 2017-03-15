/** @file msgserv.c
 *  @brief Message Server Application
 *
 *  This is the msgserv.c file to run an
 *  instance of the message server app
 *
 *  @author João Borrego
 *  @author Pedro Abreu
 *  @bug No know bugs.
 */

#include "msgserv.h"

/* Mandatory Parameters */
char* name = NULL;                      /**< Message Server Name */
char* ip = NULL;                        /**< Message Server IP */    
int tpt = -1;                           /**< TCP Port for Session Requests */
int upt = -1;                           /**< UDP Port for Terminal Requests, is global so the udp server thread can get it */
/* Optional Parameters */
char* siip = DEFAULT_SIPT;              /**< Identity Server IP */
int sipt = DEFAULT_SIIP;                /**< Identity Server UDP Port */
int m = DEFAULT_MAX_MESSAGES;           /**< Maximum number of stored messages */
int r = DEFAULT_REFRESH_RATE;           /**< Time interval between registry entries */
/* Global variables */
MessageTable* message_table = NULL;     /**< Table with message structs */
ServerID* server_list = NULL;           /**< Server ID Lists */
int LogicClock = 0;                     /**< Logic Clock mechanism to ensure causality */          
int end = 0;                            /**< Global exit variable */
volatile int timer = 0;                 /**< Timer for ID server refresh with UDP */

/* Main application */
int main(int argc, char* argv[]){

    char server_string[BUFFER_SIZE];

    parse_args(argc, argv, &name, &ip, &upt, &tpt, &siip, &sipt, &m, &r);
    debug_print("ARGS: name %s ip %s upt %d tpt %d siip %s sipt %d m %d r %d", name, ip, upt, tpt, siip, sipt, m, r);

    signal(SIGALRM, handle_alarm);  /**< Alarm for refresh handle */
    signal(SIGPIPE, SIG_IGN);       /**< Handle SIGPIPE signal */
    alarm(r);                       /**< First setup of the alarm */

    message_table = create_msg_table(m);
    FdStruct* fd_struct = create_fd_struct(upt, tpt); 
    
    /* Register message server in ID server */
    refresh(fd_struct->si_udp, name, ip, siip, sipt, upt, tpt);            // Connect to Identity Server
    get_servers(siip, sipt, server_string);                                // Get the Identity of connected servers
    server_list = create_server_list(server_string, name, ip, upt, tpt);   // Create a list and connect to previous servers  
    
    /* If there are already other message servers online */
    if(server_list != NULL){
        int fd = server_list->fd;
        print_server_list(server_list); // DEBUG
        /* Request message table to the first server in the list */   
        write(fd, "SGET_MESSAGES\n", sizeof("SGET_MESSAGES\n"));
    }

    int select_ret = -1;    // Select return value
    fd_set read_set;        // Set of file descriptors to be monitored

    while(!end){
        
        /* Initialize the file descriptor set */
        init_fd_set(&read_set, fd_struct, server_list);

        /* Blocking select() call - unblocks when there is input to read */
        select_ret = select(fd_max(fd_struct, server_list) + 1, &read_set, NULL, NULL, NULL);
        if (select_ret > 0){
            check_fd(fd_struct, &read_set);
        }

        server_list = delete_scheduled(server_list);

        /* Refresh connection with Identity Server */
        handle_si_refresh(fd_struct);
    }

    cleanup(fd_struct);

    exit(EXIT_SUCCESS);
}

void check_fd(FdStruct* fd_struct, fd_set* read_set){
    ServerID* id;
    if (fd_struct != NULL){
        /* Check if there's keyboard input to be processed */
        if (FD_ISSET(fd_struct->std_in, read_set)){
            handle_terminal(fd_struct);
        }
        /* Check if a there is a pending terminal request */
        if (FD_ISSET(fd_struct->rmb_udp, read_set)){
            handle_rmb_request(fd_struct->rmb_udp);
        }
        /* Check if another message server is trying to connect */
        if (FD_ISSET(fd_struct->msg_tcp, read_set)){
            handle_msg_connect(fd_struct->msg_tcp);
        }
        /* Check other message servers requests */
        for(id = server_list; id != NULL; id = id->next){
            if (FD_ISSET(id->fd, read_set)){
                handle_msg_activity(id->fd);
            }
        }
    }
}

void handle_terminal(FdStruct* fd_struct){
    char line[BUFFER_SIZE], command[COMMAND_SIZE];
    if(fgets(line, sizeof(line), stdin)){
        if(sscanf(line, "%s",command) == 1){
            if(!strcmp(command,"join")){
                refresh(fd_struct->si_udp, name, ip, siip, sipt, upt, tpt);
            }else if(!strcmp(command,"show_servers")){
                print_server_list(server_list);
            }else if(!strcmp(command,"show_messages")){
                print_msg_table(message_table, LogicClock);
            }else if(!strcmp(command,"exit")){
                end = 1;
            }else{
                printf("TERMINAL: Please input a valid command.\n");
            }
        }else{
            printf("TERMINAL: Please input a valid command.\n");
        }
    }
}

void handle_rmb_request(int fd_rmb_udp){

    struct sockaddr_in client_address;
    int address_length = sizeof(client_address);

    char buffer[BUFFER_SIZE], protocol[PROTOCOL_SIZE], message[MESSAGE_SIZE];   // String buffers
    int n;
    ServerID* id;

    // Blocking recvfrom call - Waits for a request
    int nbytes = recvfrom(fd_rmb_udp, buffer, sizeof(buffer), 0, (struct sockaddr*) &client_address, &address_length);
    if(nbytes == -1) exit(EXIT_FAILURE);
    debug_print("UDP: Received '%s'", buffer); 
    
    if(sscanf(buffer, "%s %d", protocol, &n) == 2){
        if(!strcmp(protocol,"GET_MESSAGES")){
            send_messages_udp(fd_rmb_udp, &client_address, message_table, n);
        }
    }else if(sscanf(buffer, SSCANF_MESSAGE_PUBLISH, protocol, message) == 2){
        if(!strcmp(protocol,"PUBLISH")){
            LogicClock++;
            insert_in_msg_table(message_table, message, LogicClock);
            for(id = server_list; id != NULL; id = id->next){
                debug_print("\tPROPAGATING TO %s %d", id->ip, id->tpt);
                send_messages_tcp(id->fd, message_table, 1, !ALL_MSGS);
            }            
            // debug_lc()
                
        }
    }
}

/* Debug Logic Clock - blocks the program execution and propagates to a chosen server.
    Only tested locally, i.e. servers running on 127.0.0.1 */
void debug_lc(){
    int port;   // the chosen TCP port 
    ServerID* id;
    while(1){
        scanf("%d", &port);
        for(id = server_list; id != NULL; id = id->next){
            if (id->tpt == port){
                debug_print("LC %d, PROPAGATING TO %s %d", LogicClock, id->ip, id->tpt);
                send_messages_tcp(id->fd, message_table, 1, !ALL_MSGS);
            }
        }
    }
}

void handle_msg_connect(int fd_msg_tcp){

    char buffer[BUFFER_SIZE], protocol[PROTOCOL_SIZE];
    char client_name[NAMEIP_SIZE], client_ip[NAMEIP_SIZE];
    int client_upt, client_tpt;

    struct sockaddr_in client_address; 
    int client_length = sizeof(client_address);

    int new_fd = accept(fd_msg_tcp, (struct sockaddr*) &client_address, &client_length);
    if(new_fd == -1){
        err_print("Accept crashed. Exiting...");
        exit(EXIT_FAILURE);
    }
    /* Read the connecting server identity */
    read(new_fd, buffer, sizeof(buffer));
    debug_print("TCP: RECEIVED %s\n", buffer);

    if (sscanf(buffer, SSCANF_ID, protocol, client_name, client_ip, &client_upt, &client_tpt)){
        /* Insert new server in identity list */    
        if (!strcmp(protocol, "ID")){
            debug_print("MSG_CONNECT: %s %s %d %d registered.", client_name, client_ip, client_upt, client_tpt);
            server_list = server_list_push(server_list, client_name, client_ip, client_upt, client_tpt, new_fd);
        }
    }
}

void handle_msg_activity(int fd_msg_tcp){

    char buffer[LARGE_BUFFER_SIZE], protocol[PROTOCOL_SIZE], messages[LARGE_BUFFER_SIZE];
    
    memset(buffer, (int) '\0', sizeof(buffer)); //To avoid errors
    
    if(read(fd_msg_tcp, buffer, sizeof(buffer)) <= 0){
        flag_for_deletion(fd_msg_tcp, server_list);
        return;
    }
    debug_print("TCP: Received '%s'", buffer);

    if (sscanf(buffer, SSCANF_SGET_MESSAGES, protocol)){
        if(!strcmp("SGET_MESSAGES\n", buffer)){
            send_messages_tcp(fd_msg_tcp, message_table, 0, ALL_MSGS);
        }
    }   
    if (sscanf(buffer, SSCANF_SMESSAGES, protocol)){
        if(!strcmp("SMESSAGES", protocol)){
            fill_msg_table(message_table, buffer, &LogicClock);
        }           
    }
}

/* Alarm interruption functions for regular refresh with ID server */
void handle_alarm(int sig){
    timer = 1;
}

void handle_si_refresh(FdStruct* fd_struct){
    if(timer){
        if (fd_struct != NULL){
            refresh(fd_struct->si_udp, name, ip, siip, sipt, upt, tpt);
            timer = 0;
            alarm(r);
        }
    }
}

void cleanup(FdStruct* fd_struct){
    free_msg_table(message_table);
    free_server_list(server_list);
    delete_fd_struct(fd_struct);
    free(name);
    free(ip);
    free(siip);
}
