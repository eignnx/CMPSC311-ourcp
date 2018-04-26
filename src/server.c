#include "proj.h"

int main(int argc, char *argv[]) {
    int socketfd;
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
        clientfd = accept(socketfd, (struct sockaddr*)&client_addr, (socklen_t *) &addrlen);
        printf("%s:%d connected\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        
        
        //reads input string for type of copy(client-to-server or server-to-client) and sends the proper response to the client
        int sd = clientfd;
        int fd;
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
            if (buffer.send.file_size >= 0){
            send_msg(sd, &message);
            recv_file(sd, buffer.send.filename, buffer.send.file_size);
            }
        }
        // if the copy type is from server to client: enters a loop that writes the file contents to the network output
        else if (buffer.send.msg_type == CMD_RECV){
            if(locate_file(buffer.send.filename, &fd)){
                message.resp.file_size = size_of_file(fd);
                send_msg(sd, &message);
                send_file(sd, fd);
            }
            else{
                errorMessage.resp.status = errno;
                printf("Error: file does not exist on server\n");
                send_msg(sd, &errorMessage);
            }
        }
        
        
        //when done closes the file and the remote sd.
        printf("closing client connection\n");
        fflush(stdout);
        close(clientfd);
       
    }
}

