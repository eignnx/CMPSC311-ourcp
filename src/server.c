#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "proj.h"
#include <errno.h>
#include <arpa/inet.h>



int main(int argc, char *argv[]) {
    int socketfd;
    char sfile[MAX_FILENAME_SIZE];
    char mtype[10];

    struct sockaddr_in server_addr;

    //prompt for ip address and port
    prompt_for_address(&server_addr, "server");
    prompt_for_port(&server_addr, "server");


    server_addr.sin_family = AF_INET;

    //Only accept TCP connections
    if ((socketfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("Error: cannot create socket of type TCP\n");
        exit(EXIT_FAILURE);
    }
    //bind socket
    if(bind(socketfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        printf("Error: cannot bind socket to port\n");
        exit(EXIT_FAILURE);
    }
    //infinite loop
    while (1) {
        //accept incoming connections
        int clientfd = 0;
        struct sockaddr_in client_addr;
        int addrlen=sizeof(client_addr);

        listen(socketfd, 1);
        clientfd = accept(socketfd, (struct sockaddr*)&client_addr, &addrlen);
        printf("%s:%d connected\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));


        //reads input string for type of copy(client-to-server or server-to-client) and sends the proper response to the client
        int sd = server_addr.sin_port;
        MsgType msgType ;
        union any_msg buffer;
        recv_msg(sd, &buffer, msgType);

        //if the copy type is from client to server: enters a loop that reads the network input for the file contents, and writes the bytes read to the output file;
        if(buffer.send.msg_type == CMD_SEND){
            send_msg(sd, "OK");
            recv_file(sd, "receivedFile", buffer.send.file_size);
        }
        // if the copy type is from server to client: enters a loop that writes the file contents to the network output
        else if (buffer.send.msg_type == CMD_RECV){
            recv_msg(sd, &buffer, CMD_RECV);
            recv_file(sd, "sentFile", buffer.send.file_size);
            int fd = open(clientfd, O_RDWR);
            if(locate_file(buffer.send.filename, clientfd)){
                send_file(sd, fd);
            }
            else{
                printf("Error: file does not exist on server\n");
                send_msg(CMD_RESP, errno);
            }
        }


        //when done closes the file and the remote sd.
        close(clientfd);
    }
}

