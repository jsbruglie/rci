#include "rmb_utils.h"

int main(int argc, char* argv[]){

    /* Optional Parameters */
    char* siip = "tejo.tecnico.ulisboa.pt"; // Identity Server IP
    int sipt = 59000;                       // Identity Server UDP Port
    
    parse_args(argc, argv, &siip, &sipt);

    int n;
    int e = 0;
    char command[256], line[256], message[140];

    char msgserv_name[256] = "";
    char msgserv_ip[256] = "";
    int msgserv_upt = -1, msgserv_tpt = -1;

    /* UDP Setup */
    int fd;
    struct hostent *hostptr;
    struct sockaddr_in server_address, client_address;
    int address_length;
    char buffer[2048];

    fd = socket(AF_INET,SOCK_DGRAM,0);
    memset((void*)&server_address, (int)'\0',sizeof(server_address));
    server_address.sin_family = AF_INET;
    
    printf("Entering here...");
    /* Command Line Interface */
    while(!e){
        if(fgets(line, sizeof(line), stdin)){
            if(sscanf(line, "%s %d",command,&n) == 2){
                if(!strcmp(command,"show_latest_messages")){
                    //show_latest_messages(n, msgserv_ip, msgserv_upt);
                    show_latest_messages(n, "127.0.0.1", 9000); 
                }else
                    printf("Please input a valid command\n");
                
            }else if(sscanf(line, "%s %s",command,message) == 2){
                if(!strcmp(command,"publish")){
                    printf("%s\n", message);
                    
                    char protocol_msg[200] = "PUBLISH ";
                    strcat(protocol_msg,message);

                    /*This is just for testing*/
                    msgserv_upt = 9000;
                    strcpy(msgserv_ip,"127.0.0.1");

                    /*Testing ended*/
                    
                    if(msgserv_upt != -1 && msgserv_ip != NULL){

                        printf("%s\n", msgserv_ip);
                        
                        server_address.sin_port = htons((u_short)msgserv_upt); 

                        fd = socket(AF_INET,SOCK_DGRAM,0);
                        hostptr = gethostbyname(msgserv_ip);
                        server_address.sin_addr.s_addr = ((struct in_addr *)(hostptr->h_addr_list[0]))->s_addr;
                        
                        address_length = sizeof(server_address);
                        printf("Sending...\n");
                        sendto(fd, protocol_msg, strlen(protocol_msg)+1,0,(struct sockaddr*)&server_address,address_length);
                        close(fd);

                    }else{
                        printf("Need an ip and udp port, otherwise\n");
                    }

                }else
                    printf("Please input a valid command\n");
                

            }else if(sscanf(line, "%s",command) == 1){
                if(!strcmp(command,"show_servers")){
                    //printf("Showing servers...");
                    server_address.sin_port = htons((u_short)sipt);
                    fd = socket(AF_INET,SOCK_DGRAM,0);
                    hostptr = gethostbyname(siip);
                    
                    server_address.sin_addr.s_addr = ((struct in_addr *)(hostptr->h_addr_list[0]))->s_addr;
                    
                    address_length = sizeof(server_address);
                    sendto(fd, "GET_SERVERS", strlen("GET_SERVERS")+1,0,(struct sockaddr*)&server_address,address_length);

                    address_length = sizeof(server_address);
                    recvfrom(fd,buffer,sizeof(buffer),0,(struct sockaddr*)&server_address,&address_length);
                    close(fd);
                    printf("%s\n",buffer);

                    /*Getting data - we just need to get the first msgserv in the list of servers we get and publish to that one, it will then propagate*/
                    char* p1 = buffer + 8; 
                    char p2[256];
                    sscanf(p1,"%[^\n]",p2);
                    debugPrint1("%s\n", p2);
                    sscanf(p2,"%256[^;];%256[^;];%d;%d",msgserv_name,msgserv_ip,&msgserv_upt,&msgserv_tpt);
                    
                    //printf("Name: %s IP: %s UDP: %d TCP: %d",msgserv_name,msgserv_ip,msgserv_upt,msgserv_tpt);


                    
                }else if(!strcmp(command,"exit")){
                    e=1;
                }else
                    printf("Please input a valid command\n");
                        
            }else
                printf("Please input a valid command\n");

        }
    }

    free(siip);
    exit(EXIT_SUCCESS);
}