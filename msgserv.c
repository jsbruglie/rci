#include "msgserv.h"

/* Mandatory Parameters */
char* name = NULL;                      // Message Server Name
char* ip = NULL;                        // Message Server IP    
int tpt = -1;                           // TCP Port for Session Requests
int upt = -1;                           // UDP Port for Terminal Requests, is global so the udp server thread can get it
/* Optional Parameters */
char* siip = "tejo.tecnico.ulisboa.pt"; // Identity Server IP
int sipt = 59000;                       // Identity Server UDP Port
int m = 200;                            // Maximum number of stored messages
int r = 10;                             // Time interval between registry entries

/* Global variables - so they can be accessed by concurrent threads */
MessageTable* message_table;            // Table with message structs
ServerID* server_list;
int LogicClock = 0;                     // Logic Clock mechanism to ensure causality            
int end = 0;                        // Global exit variable

volatile int timer = 0;    // Timer for ID server refresh with UDP

/* Main application */
int main(int argc, char* argv[]){

    parse_args(argc, argv, &name, &ip, &upt, &tpt, &siip, &sipt, &m, &r);
    debug_print("ARGS: name %s ip %s upt %d tpt %d siip %s sipt %d m %d r %d\n", name, ip, upt, tpt, siip, sipt, m, r);

    signal(SIGALRM, handle_alarm);  // Alarm for refresh handle
    alarm(r);                       // First setup of the alarm

    message_table = create_msg_table(m);
    
    /*==========Initial Request*========*/
    char* server_string = get_servers(siip, sipt);
    server_list = create_server_list(server_list, server_string, name, upt, tpt);    
    if(server_list != NULL){
        printf("Printing server list...\n");
        print_server_list(server_list);
        /*Request to the first server in the list*/
        char buffer[2048];
        int fd = server_list->fd;
        write(fd,"SGET_MESSAGES\n",sizeof("SGET_MESSAGES\n"));
        read(fd, buffer, sizeof(buffer));
        printf("%s\n", buffer);
        /*Fill the table with what we received*/
        fill_table(message_table,buffer,&LogicClock);
    }
    /*==================================*/

    int select_ret = -1;

    FdStruct* fd_struct = create_fd_struct(upt, tpt); 
    fd_set read_set; // Set of file descriptors to be monitored

    while(!end){
        
        /* Initialize the file descriptor set */
        init_fd_set(&read_set, fd_struct);

        /* Blocking select() call - unblocks when there is input to read */
        select_ret = select(fd_max(fd_struct) + 1, &read_set, NULL, NULL, NULL);
        if (select_ret > 0){
            check_fd(fd_struct, &read_set);
        }

        /* Refresh connection with Identity Server */
        handle_si_refresh(fd_struct);
    }

    cleanup();
    exit(EXIT_SUCCESS);
}

void check_fd(FdStruct* fd_struct, fd_set* read_set){
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
    }
}

void handle_terminal(FdStruct* fd_struct){
    char line[2048], command[128];
    if(fgets(line, sizeof(line), stdin)){
        if(sscanf(line, "%s",command) == 1){
            if(!strcmp(command,"join")){
                 refresh(fd_struct->si_udp, name, ip, siip, sipt, upt, tpt);
            }else if(!strcmp(command,"show_servers")){
                /*List all the servers with which this server has a TCP session*/
                print_server_list(server_list);
            }else if(!strcmp(command,"show_messages")){
                print_msg_table(message_table);
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

    char buffer[BUFFER], protocol[256], message[140];   // String buffers
    int n;

    // Blocking recvfrom call - Waits for a request
    recvfrom(fd_rmb_udp, buffer, sizeof(buffer), 0, (struct sockaddr*) &client_address, &address_length);
    debug_print("UDP: Received '%s'\n", buffer); 
    
    if(sscanf(buffer, "%s %d", protocol, &n) == 2){
        if(!strcmp(protocol,"GET_MESSAGES")){
            send_messages(fd_rmb_udp, &client_address, message_table, n);
        }
    }else if(sscanf(buffer, "%s %s", protocol, message) == 2){
        if(!strcmp(protocol,"PUBLISH")){
            insert_in_msg_table(message_table, message, LogicClock++);
            // for all servers in msg_server list
            //  connect
            //  send single message
            //  close connection
        }
    }
}

void handle_msg_connect(int fd_msg_tcp){

    char buffer[BUFFER];

    int new_fd = accept_tcp_connection(fd_msg_tcp);
    read(new_fd, buffer, sizeof(buffer));
    
    // Interpret command
    // if SGET_MESSAGES - send_msg_table(message_table, new_fd);
    // if SMESSAGES - insert_in_msg_table(message_table, message, LC);

    close(new_fd);
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

void cleanup(){
    delete_msg_table(message_table);
    free(name);
    free(ip);
}
