#include "rmb_utils.h"

int main(int argc, char* argv[]){
    /* Optional Parameters */
    char* siip = "tejo.tecnico.ulisboa.pt";   // Identity Server IP
    int sipt = 59000;                           // Identity Server UDP Port
    parse_args(argc, argv, &siip, &sipt);

    /*Parameters needed for publish*/
    char msgserv_name[NAMEIP_SIZE] = "", msgserv_ip[NAMEIP_SIZE] = ""; //Name and ip of the message server we pick
    int msgserv_upt = -1, msgserv_tpt = -1; //UDP and TCP port of the message server we pick

    /*Protocol variables + Interface variables*/
    int n, end = 0; //Number of messages you may want to get | ending variable for IO
    char message[MESSAGE_SIZE], command[COMMAND_SIZE], line[BUFFER_SIZE]; //Message you may want to publish | Command line variables
   
    /* Command Line Interface */
    while(!end){
        if(fgets(line, sizeof(line), stdin)){
            if(sscanf(line, "%s %d",command,&n) == 2){
                if(!strcmp(command,"show_latest_messages")){
                        char* buffer = get_servers(siip, sipt); //Get servers
                        pick_server(buffer,msgserv_name, msgserv_ip, &msgserv_upt, &msgserv_tpt); //Pick one of the servers
                        if(msgserv_upt != -1 && msgserv_ip != NULL) //If there is a server
                            show_latest_messages(n, msgserv_ip, msgserv_upt); 
                        else
                            printf("Need an IP and UDP port from a msgserv. At least one msgserv must exist\n");
                        
                }else
                    printf("Please input a valid command\n");
                
            }else if(sscanf(line, "%s %140[^\n]",command,message) == 2){
                if(!strcmp(command,"publish")){
                    char* buffer = get_servers(siip, sipt); //Get servers
                    pick_server(buffer,msgserv_name,msgserv_ip,&msgserv_upt,&msgserv_tpt); //Pick one of the servers
                    debug_print("Name: %s IP: %s UDP: %d TCP: %d\n",msgserv_name,msgserv_ip,msgserv_upt,msgserv_tpt);
                    
                    if(msgserv_upt != -1 && msgserv_ip != NULL) //If there is a server
                        publish_msg(message, msgserv_ip, msgserv_upt); //Publish message
                    else
                        printf("Need an IP and UDP port from a msgserv. At least one msgserv must exist\n");                   
                }else
                    printf("Please input a valid command\n");
                
            }else if(sscanf(line, "%s",command) == 1){
                if(!strcmp(command,"show_servers"))
                    print_servers(siip, sipt);         
                else if(!strcmp(command,"exit"))
                    end=1;
                else
                    printf("Please input a valid command\n");
            }else
                printf("Please input a valid command\n");
        }
    }

    exit(EXIT_SUCCESS);
}