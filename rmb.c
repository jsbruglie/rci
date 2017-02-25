#include "rmb_utils.h"

int main(int argc, char* argv[]){

	char* siip = "tejo.tecnico.ulisboa.pt";
	int sipt = 59000;
	
	parse_args(argc, argv, &siip, &sipt);

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