#include "msgserv_utils.h"
#include <pthread.h>

//Exit is global
int e = 0;
/* Mandatory Parameters */
char* name = NULL;						// Message Server Name
char* ip = NULL;						// Message Server IP	

int tpt = -1;							// TCP Port for Session Requests
int upt = -1;							// UDP Port for Terminal Requests, is global so the udp server thread can get it
/* Optional Parameters */
char* siip = "tejo.tecnico.ulisboa.pt";	// Identity Server IP
int sipt = 5900;						// Identity Server UDP Port
int m = 200;							// Maximum number of stored messages
int r = 10;								// Time interval between registry entries


Message** message_table;
int LogicClock = 0;

void* udp_server(){
	printf("Starting to listen....\n");
	int fd;
	struct hostent *hostptr;
	struct sockaddr_in server_address, client_address;
	int address_length;

	char buffer[256];

	char protocol[256];
	char message[140];
	int n;

	fd = socket(AF_INET,SOCK_DGRAM,0);
	memset((void*)&server_address, (int)'\0',sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);

	int test_port = 58000;

	server_address.sin_port = htons((u_short)test_port);
	bind(fd, (struct sockaddr*)&server_address,sizeof(server_address));
	
	while(!e){
		address_length = sizeof(client_address);
		recvfrom(fd,buffer,sizeof(buffer),0,(struct sockaddr*)&client_address,&address_length);
		//Blocks here until we get a message
		printf("%s\n", buffer); 
		if(sscanf(buffer, "%s %d",protocol,&n) == 2){
			if(!strcmp(protocol,"GET_MESSAGES")){
				//Send messages back
				printf("got messages\n");
				char protocol[4096] = "MESSAGE\n";
				//Append messages 
				int i;
				char message_list[2048];
				if(n<m){ //Assume the user isn't evil
					for(i=0;i<n;i++){
						if(message_table[i]!=NULL){
							strcat(message_list,message_table[i]->text);
							strcat(message_list,"\n");
						}
					}	
				}


				strcat(protocol,message_list);
				//Send them
				address_length = sizeof(client_address);
				sendto(fd,protocol,strlen(protocol)+1,0,(struct sockaddr*)&client_address,address_length);
				memset(message_list,0,strlen(message_list));
			}
		}else if(sscanf(buffer, "%s %s",protocol,message) == 2){
			if(!strcmp(protocol,"PUBLISH")){
				//Add this message to the message list
				printf("Received message: %s\n", message);
				int i, free_spot = 0;
				for(i=0;i<m;i++){
					if(message_table[i] == NULL){ //find the first not null position to insert a message
						message_table[i] = (Message*)malloc(sizeof(Message));
						strcpy(message_table[i]->text,message);

						LogicClock++;
						message_table[i]->clock = LogicClock;
						free_spot = 1;
						break;
					}
				}
				if(!free_spot){ //Message list is full, find the oldest message and replace that one with the new message
					int i;
					for(i=0;i<m;i++){
						int min = INT_MAX;
						int oldest_idx;
						if(message_table[i]->clock < min){
							oldest_idx = i;
							min = message_table[i]->clock;
						}
					}
					strcpy(message_table[oldest_idx]->text,message);
					LogicClock++;
					message_table[oldest_idx]->clock = LogicClock;				
				}
				

			}
		}

	}
	close(fd);
	return NULL;
}

void* interface(){
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



	printf("Starting loop\n");
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
	exit(EXIT_SUCCESS);
}





int main(int argc, char* argv[]){

	parse_args(argc, argv, &name, &ip, &upt, &tpt, &siip, &sipt, &m, &r);
	printf("ARGS: name %s ip %s upt %d tpt %d siip %s sipt %d m %d r %d\n", name, ip, upt, tpt, siip, sipt, m, r);

	message_table = (Message**)malloc(sizeof(Message*)*m);
	int i;
	for(i=0;i<m;i++)
		message_table[i] = NULL;

	pthread_t interface_thread;
	if(pthread_create(&interface_thread,NULL,interface,0)){
		fprintf(stderr, "Error creating thread\n");
		return 1;

		
	}
	pthread_t udp_server_thread;
	if(pthread_create(&udp_server_thread,NULL,udp_server,0)){
		fprintf(stderr, "Error creating thread\n");
		return 1;

		
	}


	pthread_join(interface_thread,NULL);
	pthread_join(udp_server_thread,NULL);

    exit(EXIT_SUCCESS);
}