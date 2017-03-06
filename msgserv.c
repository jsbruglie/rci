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
/* Global variables */
MessageTable* message_table;            // Table with message structs
ServerID* server_list;                  // Server ID Lists
int LogicClock = 0;                     // Logic Clock mechanism to ensure causality            
int end = 0;                            // Global exit variable
volatile int timer = 0;                 // Timer for ID server refresh with UDP

/* Main application */
int main(int argc, char* argv[]){

    parse_args(argc, argv, &name, &ip, &upt, &tpt, &siip, &sipt, &m, &r);
    debug_print("ARGS: name %s ip %s upt %d tpt %d siip %s sipt %d m %d r %d\n", name, ip, upt, tpt, siip, sipt, m, r);

    signal(SIGALRM, handle_alarm);  // Alarm for refresh handle
    alarm(r);                       // First setup of the alarm

    message_table = create_msg_table(m);
    
    FdStruct* fd_struct = create_fd_struct(upt, tpt); 
    
    /* Register message server in ID server */
    refresh(fd_struct->si_udp, name, ip, siip, sipt, upt, tpt); 						// Connect
    char* server_string = get_servers(siip, sipt); 										// Get servers
    server_list = create_server_list(server_list, server_string, name, ip, upt, tpt);	// Create a list and connect to previous servers  
    
    if(server_list != NULL){
        // printf("Printing server list...\n");
        // print_server_list(server_list);
        /* Request message table to the first server in the list */
        
        char buffer[BUFFER_SIZE];
        int fd = server_list->fd;

        write(fd,"SGET_MESSAGES\n",sizeof("SGET_MESSAGES\n"));
        read(fd, buffer, sizeof(buffer));

        // printf("Received: %s\n", buffer);
        
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
    ServerID* id;

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
            for(id = server_list; id != NULL; id = id->next){
                send_messages_tcp(id->fd, message_table, 1, 0);
            }
        }
    }
}

void handle_msg_connect(int fd_msg_tcp){

    char buffer[BUFFER_SIZE], protocol[PROTOCOL_SIZE];
    char client_name[NAMEIP_SIZE], client_ip[NAMEIP_SIZE];
    int client_upt, client_tpt;

    int new_fd;
    struct sockaddr_in client_address; 
    int client_length = sizeof(client_address);

    new_fd = accept(fd_msg_tcp, (struct sockaddr*) &client_address, &client_length);
   
    /* Read the connecting server identity */
    read(new_fd, buffer, sizeof(buffer));

    if (sscanf(buffer, "%256[^\n]%256[^;];%256[^;];%d;%d", protocol, client_name, client_ip, &client_upt, &client_tpt)){
    	/* Insert new server in identity list */    
    	if (!strcmp(protocol, "ID")){
    		debug_print("MSG_CONNECT: %s %s %d %d registered.\n", client_name, client_ip, upt, tpt);
    		server_list_push(server_list, client_name, client_ip, client_upt, client_upt, new_fd);
    	}
    }
}

void handle_msg_activity(int fd_msg_tcp){

	char buffer[BUFFER_SIZE*100], protocol[PROTOCOL_SIZE], messages[BUFFER_SIZE*100];
	char* response;
	int size;

    read(fd_msg_tcp, buffer, sizeof(buffer));

    if (sscanf(buffer, "%256s", protocol)){
    	if(!strcmp("SGET_MESSAGES\n",buffer)){
        	size = size_latest_messages(message_table, 0, ALL_MSGS, INCLUDE_CLK);
        	response = (char*) malloc(sizeof(char) * size);
			write(fd_msg_tcp, response, size);
		}
	}	
	if (sscanf(buffer, "%256s\n%s", protocol, messages)){
		if(!strcmp("SMESSAGES", protocol)){
			return;
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

void cleanup(){
    free_msg_table(message_table);
    free_server_list(server_list);
    free(name);
    free(ip);
}
