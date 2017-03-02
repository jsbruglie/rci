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
MessageTable* message_table;    // Table with message structs
int LogicClock = 0;             // Logic Clock mechanism to ensure causality            
int end = 0;                    // Global exit variable

/* Main application */
int main(int argc, char* argv[]){

    parse_args(argc, argv, &name, &ip, &upt, &tpt, &siip, &sipt, &m, &r);
    debug_print("ARGS: name %s ip %s upt %d tpt %d siip %s sipt %d m %d r %d\n", name, ip, upt, tpt, siip, sipt, m, r);

    message_table = create_msg_table(m);
    get_servers(siip, sipt);
    
    int fd_stdin = STDIN;
    int max_fd = fd_stdin;
    int fd_si_udp = -1; 
    int fd_rmb_udp = create_udp_server(upt);
    max_fd = (fd_rmb_udp > max_fd)? fd_rmb_udp : max_fd;
    int select_ret = -1;

    FdStruct* fd_struct = create_fd_struct(upt);
    fd_set read_set; // Set of file descriptors to be monitored

    while(!end){
        
        /* Initialize the file descriptor set */
        init_fd_set(&read_set, fd_stdin, fd_si_udp, fd_rmb_udp);

        /* Blocking select() call - unblocks when there is input to read */
        select_ret = select(max_fd + 1, &read_set, NULL, NULL, NULL);
        if (select_ret != -1){
            if (FD_ISSET(fd_stdin, &read_set)){
                handle_terminal();
            }
            if (FD_ISSET(fd_rmb_udp, &read_set)){
                handle_rmb_request(fd_rmb_udp);
            }
        }
    }
    exit(EXIT_SUCCESS);
}

void handle_terminal(){
    char line[2048], command[128];
    if(fgets(line, sizeof(line), stdin)){
        if(sscanf(line, "%s",command) == 1){
            if(!strcmp(command,"join")){
                register_in_server(name, ip, siip, sipt, upt, tpt);
            }else if(!strcmp(command,"show_servers")){
                /*List all the servers with which this server has a TCP session*/
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
        }
    }
}

void* refresh_registration(){
    debug_print("REFRESH: Starting refresh_registration.\n");
    while(!end){
        register_in_server(name, ip, siip, sipt, upt, tpt);
        sleep(r);
    }
    return NULL;
}