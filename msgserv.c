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