//gcc msgserv.c -o msgserv
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "msgserv_utils.h"

int main(int argc, char* argv[]){

	/* Main Parameters */
	char* name = NULL;	// Client name
	char* ip = NULL;	// Client IP	
	int upt = -1;		// UDP Port
	int tpt = -1;		// TCP Port

	parse_args(argc, argv, &name, &ip, &upt, &tpt);

	int e = 0;
	char command[256], line[256];

	//Create UDP Server - TO DO
	int fd;
	struct hostent *hostptr;
	struct sockaddr_in server_address, client_address;
	int address_length;
	char msg[140], buffer[140];

	fd = socket(AF_INET,SOCK_DGRAM,0);
	memset((void*)&server_address, (int)'\0',sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons((u_short)upt);
	bind(fd,(struct sockaddr*)&server_address,sizeof(server_address));


	/*Thread 1 - Interface*/
	while(!e){
		if(fgets(line, sizeof(line), stdin)){
			if(sscanf(line, "%s",command) == 1){
				if(!strcmp(command,"join")){
					//printf("Register in message server");
				}else if(!strcmp(command,"show_servers")){


				}else if(!strcmp(command,"show_messages")){

					
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