#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include "proj.h"

//#define test (0xAAAA0000 | 4)
//#define test2 (0xBBBB0000 | 4)

int main(char argc, int *argv[]){
	char ip[25];
	char port[10];
	char sfile[MAX_FILENAME_SIZE];
	char mtype[10];
	
	printf("Enter IP address of destination => ");
	scanf("%s", ip);
	printf("Enter port of destination => ");
	scanf("%s", port);
	printf("Enter name of file to send => ");
	scanf("%s", sfile);
	printf("Enter copy type => ");
	scanf("%s", mtype);
	
	
	struct sockaddr_in temp;
	temp.sin_family = AF_INET;
	int sport = atoi(port);
	temp.sin_port = htons(sport);
	int s = inet_pton(AF_INET, ip, &(temp.sin_addr.s_addr));
	int ltemp = sizeof(temp);
	
	//prompt_for_address(,"client");
	//prompt_for_port(,"client");
	
	FILE *fp = fopen(sfile,"r");
	
	if(strcmp(mtype,"CMD_SEND") == 0){
		if(fp){
			printf("File exists\n");
			
			if((sendto(s, sfile, sizeof(sfile), 0, (struct sockaddr *)&temp, ltemp)) == -1){
				perror("Send Error");
			}else{
				printf("Send file name: %s\n", sfile);
				printf("Send file length %lu\n", sizeof(sfile));
				printf("Connection accepted from localhost\n");
			}
		}else{
			printf("File does not exists\n");
			exit(1);
		}
		
	}else{
	
	}
	
	fclose(fp);
}
