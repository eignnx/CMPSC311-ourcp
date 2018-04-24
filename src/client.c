#include "proj.h"

int main(int argc, char *argv[]){
	char sfile[MAX_FILENAME_SIZE];
	char mtype[10];
	
	struct sockaddr_in *temp;
	
	prompt_for_address(temp,"client");
	prompt_for_port(temp,"client");
	
	printf("Enter copy type => ");
	scanf("%s", mtype);
	printf("Enter name of file to send => ");
	scanf("%s", sfile);
	
	
	//union any_msg p1;
	struct send_msg p1;
	struct resp_msg p2;
	if(strcmp(mtype, "CMD_SEND") == 0){
		p1.msg_type = CMD_SEND;
	}else if(strcmp(mtype,"CMD_RECV") == 0){
		p1.msg_type = CMD_RECV;
	}else if(strcmp(mtype,"CMD_RESP") == 0){
		p1.msg_type = CMD_RESP;
	}else if(strcmp(mtype, "CMD_DATA")== 0){
		p1.msg_type = CMD_DATA;
	}else{
		printf("Incorrect input\n");
		exit(1);
	}
	//p1.msg_type = mtype;
	//union any_msg p2;
	
	int sd = socket(AF_INET, SOCK_STREAM, 0);
	temp->sin_family = AF_INET;
	int errcode = connect(sd, (struct sockaddr *) &temp, sizeof(temp));
			
	//int d = temp.sin_port;
	int fd;
	
	if(strcmp(mtype, "CMD_SEND") == 0){
		if(locate_file(sfile,&fd)){
			printf("File exists\n");
			send_msg(sd, (union any_msg *) &p1);
			recv_msg(sd, (union any_msg *) &p2, CMD_RESP);
		
			if(p2.status == 0){		
				sd = temp->sin_port; 
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
