#include "msgserv_utils.h"

int main(int argc, char* argv[]){

	/* Mandatory Parameters */
	char* name = NULL;						// Message Server Name
	char* ip = NULL;						// Message Server IP	
	int upt = -1;							// UDP Port for Terminal Requests
	int tpt = -1;							// TCP Port for Session Requests
	/* Optional Parameters */
	char* siip = "tejo.tecnico.ulisboa.pt";	// Identity Server IP
	int sipt = 5900;						// Identity Server UDP Port
	int m = 200;							// Maximum number of stored messages
	int r = 10;								// Time interval between registry entries

	parse_args(argc, argv, &name, &ip, &upt, &tpt, &siip, &sipt, &m, &r);
	printf("ARGS: name %s ip %s upt %d tpt %d siip %s sipt %d m %d r %d\n", name, ip, upt, tpt, siip, sipt, m, r);

	int e = 0;
	char command[256], line[256];
	
	//Create UDP setup 
	int fd;
	struct hostent *hostptr;
	struct sockaddr_in server_address, client_address;
	int address_length;
	char msg[140], buffer[2048], registration[1026];

	fd = socket(AF_INET,SOCK_DGRAM,0);
	memset((void*)&server_address, (int)'\0',sizeof(server_address));
	server_address.sin_family = AF_INET;

	/*Thread 1 - Interface*/
	while(!e){
		if(fgets(line, sizeof(line), stdin)){
			if(sscanf(line, "%s",command) == 1){
				if(!strcmp(command,"join")){
					printf("Register in message server");
					
					server_address.sin_port = htons((u_short)sipt);
					fd = socket(AF_INET,SOCK_DGRAM,0);
					hostptr = gethostbyname(siip);
					server_address.sin_addr.s_addr = ((struct in_addr *)(hostptr->h_addr_list[0]))->s_addr;
					
					//Create registration string
					sprintf(registration, "REG %s;%s;%d;%d",name,ip,upt,tpt);
					//Register
					address_length = sizeof(server_address);
					sendto(fd, registration, strlen(registration)+1,0,(struct sockaddr*)&server_address,address_length);
					

					close(fd);
					printf("%s\n",buffer);
					/*Do a get servers after join*/
					fd = socket(AF_INET,SOCK_DGRAM,0);
					hostptr = gethostbyname(siip);
					
					server_address.sin_addr.s_addr = ((struct in_addr *)(hostptr->h_addr_list[0]))->s_addr;
					
					address_length = sizeof(server_address);
					sendto(fd, "GET_SERVERS", strlen("GET_SERVERS")+1,0,(struct sockaddr*)&server_address,address_length);

					address_length = sizeof(server_address);
					recvfrom(fd,buffer,sizeof(buffer),0,(struct sockaddr*)&server_address,&address_length);
					close(fd);
					printf("%s\n",buffer);


				}else if(!strcmp(command,"show_servers")){
					/*List all the servers with which this server has a TCP session*/
					


				}else if(!strcmp(command,"show_messages")){
					/*List all the messages on this server, ordered by logic times*/
					
				}else if(!strcmp(command,"exit")){
					e=1;
				}else
					printf("Please input a valid command\n");
						
			}else
				printf("Please input a valid command\n");

		}
	}
	/*Thread 2*/



	/*HAD SOME INVALID FREE's HERE*/

    exit(EXIT_SUCCESS);
}