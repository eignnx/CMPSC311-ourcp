#include "proj.h"

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
        MsgType msgType = CMD_SEND;
        union any_msg buffer;
        recv_msg(sd, &buffer, msgType);
        
        union any_msg message;
        message.resp.status = OK;
        message.resp.msg_type = CMD_RESP;
        
        union any_msg errorMessage;
        errorMessage.resp.msg_type = CMD_RESP;
        
        //if the copy type is from client to server: enters a loop that reads the network input for the file contents, and writes the bytes read to the output file;
        if(buffer.send.msg_type == CMD_SEND){
            send_msg(sd, message);
            recv_file(sd, buffer.send.filename, buffer.send.file_size);
        }
        // if the copy type is from server to client: enters a loop that writes the file contents to the network output
        else if (buffer.send.msg_type == CMD_RECV){
            if(locate_file(buffer.send.filename, &clientfd)){
                message.resp.filesize = size_of_file(fd);
                send_file(sd, fd);
                send_msg(sd, message);
            }
            else{
                errorMessage.resp.status = errno;
                printf("Error: file does not exist on server\n");
                send_msg(sd, errorMessage);
            }
        }


        //when done closes the file and the remote sd.
        close(clientfd);
    }
}

