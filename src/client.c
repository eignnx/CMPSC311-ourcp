#include "proj.h"

int main(char argc, int *argv[]){
	char sfile[MAX_FILENAME_SIZE];
	char mtype[10];
	
	struct sockaddr_in temp;
	
	prompt_for_address(temp,"client");
	prompt_for_port(temp,"client");
	
	printf("Enter copy type => ");
	scanf("%s", mtype);
	printf("Enter name of file to send => ");
	scanf("%s", sfile);
	
	
	union any_msg p1;
	p1->any.msg_type = mtype;
	union any_msg p2;
	
	int sd = temp.sin_port;
	int fd;
	
	if(strcmp(mtype, "CMD_SEND") == 0){
		if(locate_file(sfile,fd)){
			printf("File exists\n");
		}else{
			printf("File does not exist\n");
			exit(1);
		}
		
		send_msg(sd, p1);
		recv_msg(sd, p2, CMD_RESP);
		
		if(p2->resp_msg == 0){
			sd = temp.sin_port; 
			fd = open(sfile, O_RDWR);
			send(file(sd, fd);
			close(fd);
		}else{
			printf("Send Error\n");
		}
	}else{
		p1->msg_type = "CMD_RECV";
		send_msg(sd, p1);
		recv_msg(sd, p2, CMD_RESP);
		
		if(p2->resp_msg == 0){
			int temp = p2->resp_msg.file_size;
			recv_file(sd, sfile, temp);
		}else{
			printf("Recieve Error\n");
		}
	}
	close(sd);
}
