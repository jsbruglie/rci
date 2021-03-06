#include "rmb_utils.h"

int main(int argc, char* argv[]){
    
    /* Optional ID Server Parameters */
    char* siip = "tejo.tecnico.ulisboa.pt"; // Identity Server IP
    int sipt = 59000;                       // Identity Server UDP Port
    parse_args(argc, argv, &siip, &sipt);
    debug_print("ARGS: siip %s sipt %d", siip, sipt);

    /* Selected Message Server Parameters */
    char msgserv_name[NAMEIP_SIZE] = {0};   // Message server name string
    char msgserv_ip[NAMEIP_SIZE] = {0};     // Message server ip string 
    int msgserv_upt = -1, msgserv_tpt = -1; // UDP and TCP ports of the selected message server

    /* Protocol variables and Interface variables */
    int n = -1;     // Number of messages to fetch
    int end = 0;    // Exit variable

    /* Buffer strings */
    char message[MESSAGE_SIZE] = {0};
    char command[COMMAND_SIZE] = {0}; 
    char line[BUFFER_SIZE] = {0};
    char buffer[LARGE_BUFFER_SIZE] = {0};

    /* Command Line Interface */
    while(!end){
        printf(CMD_PROMPT);
        if(fgets(line, sizeof(line), stdin)){
            if(sscanf(line, "%s %d", command, &n) == 2){
                if(!strcmp(command, "show_latest_messages")){
                    get_servers(siip, sipt, buffer, !PRINT);
                    if(pick_server(buffer, msgserv_name, msgserv_ip, &msgserv_upt, &msgserv_tpt) == 0){
                        show_latest_messages(n, msgserv_ip, msgserv_upt); 
                    }else{
                        printf("Could not connect to a message server.\n");
                    }
                }else{
                    printf("Please input a valid command.\n");
                }
            }else if(sscanf(line, SSCANF_MESSAGE_PUBLISH, command, message) == 2){
                if(!strcmp(command, "publish")){
                    if(valid_msg(message)){
                        get_servers(siip, sipt, buffer, !PRINT);
                        if(pick_server(buffer, msgserv_name, msgserv_ip, &msgserv_upt, &msgserv_tpt) == 0){
                            publish_msg(message, msgserv_ip, msgserv_upt);
                        }else{
                            printf("Could not connect to a message server.\n");
                        }
                    }else{
                        printf("Please input a valid message.\n");
                    }    
                }else{
                    printf("Please input a valid command.\n");
                }
            }else if(sscanf(line, "%s", command) == 1){
                if(!strcmp(command,"show_servers")){
                    get_servers(siip, sipt, buffer, PRINT);       
                }else if(!strcmp(command, "exit")){
                    end = 1;
                }else{
                    printf("Please input a valid command.\n");
                }
            }else{
                printf("Please input a valid command.\n");
            }
        }
    }

    /* Cleanup */
    free(siip);

    exit(EXIT_SUCCESS);
}