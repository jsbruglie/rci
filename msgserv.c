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
    
    FdStruct* fd_struct = create_fd_struct(upt, tpt); 
    
    /*==========Initial Request & Server List Connections*========*/
    refresh(fd_struct->si_udp, name, ip, siip, sipt, upt, tpt); //Connect
    char* server_string = get_servers(siip, sipt); //Get servers
    server_list = create_server_list(server_list, server_string, name, upt, tpt); //Create a list + connect to previous servers  
    if(server_list != NULL){
        printf("Printing server list...\n");
        print_server_list(server_list);
        /*Request message table to the first server in the list*/
        char buffer[BUFFER_SIZE];
        int fd = server_list->fd;
        write(fd,"SGET_MESSAGES\n",sizeof("SGET_MESSAGES\n"));
        
        read(fd, buffer, sizeof(buffer));
        printf("Received: %s\n", buffer);
        
        /*Fill our table with what we received*/
        fill_table(message_table,buffer,&LogicClock);
    }
    /*============================================================*/

    int select_ret = -1;

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
    char line[BUFFER_SIZE], command[COMMAND_SIZE];
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

    char buffer[BUFFER_SIZE], protocol[PROTOCOL_SIZE], message[MESSAGE_SIZE];   // String buffers
    int n;

    // Blocking recvfrom call - Waits for a request
    recvfrom(fd_rmb_udp, buffer, sizeof(buffer), 0, (struct sockaddr*) &client_address, &address_length);
    debug_print("UDP: Received '%s'\n", buffer); 
    
    if(sscanf(buffer, "%s %d", protocol, &n) == 2){
        if(!strcmp(protocol,"GET_MESSAGES")){
            send_messages(fd_rmb_udp, &client_address, message_table, n);
        }
    }else if(sscanf(buffer, "%s %140[^\n]", protocol, message) == 2){ //If you change MESSAGE_SIZE, change this aswell
        if(!strcmp(protocol,"PUBLISH")){
            insert_in_msg_table(message_table, message, LogicClock++);
            // for all servers in msg_server list
            //  send single message
        }
    }
}

void handle_msg_connect(int fd_msg_tcp){

    //read from fd
    // if SMESSAGES - insert_in_msg_table(message_table, message, LC); 
    //return
    //if read times out, we go into accept
    
    char buffer[BUFFER_SIZE];

    int new_fd;
    struct sockaddr_in client_address; 
    int client_length = sizeof(client_address);

    new_fd = accept(fd_msg_tcp, (struct sockaddr*) &client_address, &client_length);
    char clientAddr[COMMAND_SIZE];

    inet_ntop(AF_INET, &(client_address.sin_addr), clientAddr, sizeof(clientAddr));
    printf("%s\n", clientAddr);
    //Do a get servers, search for their ip and add it to my server list if we don't have a connection to it



    read(new_fd, buffer, sizeof(buffer));
    printf("%s\n", buffer);
    // Interpret command
    // if SGET_MESSAGES - send_msg_table(message_table, new_fd);
    char response[BUFFER_SIZE] = "";
    if(!strcmp("SGET_MESSAGES\n",buffer)){
        //Answer with a message table
        printf("Answering...\n");
        char temp[BUFFER_SIZE];
        int i;
        for(i=0;i<m;i++){
            if(message_table->table[i] != NULL){
                sprintf(temp, "%d;%s\n",message_table->table[i]->clock,message_table->table[i]->text);
                strcat(response, temp);    
            }
        }
        write(new_fd, response, sizeof(response));
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

void cleanup(){
    free_msg_table(message_table);
    free_server_list(server_list);
    free(name);
    free(ip);
}

