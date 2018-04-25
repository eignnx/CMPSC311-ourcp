#include "proj.h"

int main(int argc, char *argv[]){
	char sfile[MAX_FILENAME_SIZE];
	char mtype[10];
	
	struct sockaddr_in temp;
	
	prompt_for_address(&temp,"client");
	prompt_for_port(&temp,"client");
	


	
	printf("Enter 1 for send or 2 for receive => ");
	int choice;
	scanf("%d", &choice);
	struct send_msg p1;
	struct resp_msg p2;
	if(choice == 1){
		p1.msg_type = CMD_SEND;
	}else if(choice == 2){
		p1.msg_type = CMD_RECV;
	}else{
		printf("Incorrect input\n");
		exit(1);
	}

	printf("Enter name of file to send => ");
	scanf("%s", sfile);
	
	int sd = socket(AF_INET, SOCK_STREAM, 0);
	temp.sin_family = AF_INET;
	int errcode = connect(sd, (struct sockaddr *) &temp, sizeof(temp));
	if(errcode == -1){
		perror("Connect Error\n");
		exit(1);
	}
			
	int fd;
	strcpy(p1.filename,sfile);
	
	if(choice == 1){
		if(locate_file(sfile,&fd)){
			
			printf("File exists\n");
			p1.file_size = size_of_file(fd);;
			send_msg(sd, (union any_msg *) &p1);
			recv_msg(sd, (union any_msg *) &p2, CMD_RESP);
		
			if(p2.status == 0){	
				fd = open(sfile, O_RDWR);
				send_file(sd, fd);
				close(fd);
			}else{
				printf("Send Error\n");
			}
		}else{
			printf("File does not exist\n");
			exit(1);
		}
		
		
	}else{
		p1.msg_type = CMD_RECV;
		send_msg(sd, (union any_msg *) &p1);
		recv_msg(sd, (union any_msg *) &p2, CMD_RESP);
		
		if(p2.status == 0){
			int temp = p2.file_size;
			recv_file(sd, sfile, temp);
		}else{
			printf("Recieve Error\n");
		}
	}
	close(sd);
}
