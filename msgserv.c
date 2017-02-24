//gcc msgserv.c -o msgserv
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

int main(int argc, char* argv[]){

	/*Non-optional*/
	char* name = NULL;
	char* ip = NULL;
	int upt = -1;
	int tpt = -1;
	/*Optional*/
	char* siip = "tejo.tecnico.ulisboa.pt";
	int sipt = 59000;
	int m = 200;
	int r = 10;

	int opt;
	while( (opt = getopt(argc, argv, "n:j:u:t:i:p:m:r:")) != -1 ){
		switch(opt){
			/*Non-Optional arg processing*/
			case 'n':
				name = (char*)malloc(sizeof(char) * (sizeof(optarg)/sizeof(optarg[0])));
				strcpy(name, optarg);
				break;
			case 'j':
				ip = (char*)malloc(sizeof(char) * (sizeof(optarg)/sizeof(optarg[0])));
				strcpy(ip, optarg);
				break;
			case 'u':
				upt = atoi(optarg);
				break;
			case 't':
				tpt = atoi(optarg);
				break;

			/*Options processing*/
			case 'i':
				siip = NULL;
				siip = (char*)malloc(sizeof(char) * (sizeof(optarg)/sizeof(optarg[0])));
				strcpy(siip, optarg);
				break;
			case 'p':
				sipt = atoi(optarg);
				break;
			case 'm':
				m = atoi(optarg);
				break;
			case 'r':
				r = atoi(optarg);
				break;
			default:
				fprintf(stderr, "Usage: %s –n name –j ip -u upt –t tpt [-i siip] [-p sipt] [–m m] [–r r] \n", argv[0]);
            	exit(EXIT_FAILURE);
		}
	}

	if(name == NULL || ip == NULL || upt == -1 || tpt == -1){ //Check if the arguments were provided
		fprintf(stderr, "Usage: %s –n name –j ip -u upt –t tpt [-i siip] [-p sipt] [–m m] [–r r] \n", argv[0]);
		exit(EXIT_FAILURE);
	}
	if(upt == tpt)
		printf("UDP and TCP port are the same"); 

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
					//printf("Register in message server");
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




				}else if(!strcmp(command,"show_servers")){
					
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