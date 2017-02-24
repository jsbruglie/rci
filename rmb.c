//gcc rmb.c -o rmb
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

int main(int argc, char* argv[]){

	char* siip = "tejo.tecnico.ulisboa.pt";
	int sipt = 59000;
	
	/*Get options*/
	int opt;
	while( (opt = getopt(argc, argv, "i:p:")) != -1 ){
		switch(opt){

			case 'i':
				siip = NULL;
				siip = (char*)malloc(sizeof(char) * (sizeof(optarg)/sizeof(optarg[0])));
				strcpy(siip, optarg);
				break;
			case 'p':
				sipt = atoi(optarg);
				break;

			default:
				fprintf(stderr, "Usage: %s [-i siip] [-p sipt] \n", argv[0]);
            	exit(EXIT_FAILURE);
		}
	}

	//printf("sipt=%d; siip=%s\n", sipt, siip);
	
	int n, e = 0;
	char command[256], line[256], message[140];
	//Create UDP setup 
	int fd;
	struct hostent *hostptr;
	struct sockaddr_in server_address, client_address;
	int address_length;
	char msg[140], buffer[2048];

	fd = socket(AF_INET,SOCK_DGRAM,0);
	memset((void*)&server_address, (int)'\0',sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons((u_short)sipt);

	/*Interfaces*/
	while(!e){
		if(fgets(line, sizeof(line), stdin)){
			if(sscanf(line, "%s %d",command,&n) == 2){
				if(!strcmp(command,"show_latest_messages")){
					//printf("%d\n", n);




				}else
					printf("Please input a valid command\n");
				
			}else if(sscanf(line, "%s %s",command,message) == 2){
				if(!strcmp(command,"publish")){
					//printf("%s\n", message);





				}else
					printf("Please input a valid command\n");
				

			}else if(sscanf(line, "%s",command) == 1){
				if(!strcmp(command,"show_servers")){
					//printf("Showing servers...");
					fd = socket(AF_INET,SOCK_DGRAM,0);
					hostptr = gethostbyname(siip);
					
					server_address.sin_addr.s_addr = ((struct in_addr *)(hostptr->h_addr_list[0]))->s_addr;
					
					address_length = sizeof(server_address);
					sendto(fd, "GET_SERVERS", strlen("GET_SERVERS")+1,0,(struct sockaddr*)&server_address,address_length);

					address_length = sizeof(server_address);
					recvfrom(fd,buffer,sizeof(buffer),0,(struct sockaddr*)&server_address,&address_length);
					close(fd);
					printf("%s\n",buffer);

					
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