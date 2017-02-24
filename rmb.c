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