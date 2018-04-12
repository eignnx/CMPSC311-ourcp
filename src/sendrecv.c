// sendrecv.c
//
// Holds functions that are common to both client and server.
//

#include "proj.h"


#define INP_BUF_SIZE 100

void prompt_for_address(struct sockaddr_in *address, char *who)
{
    char buf[INP_BUF_SIZE];

    printf("Enter the %s's IP address: ", who);
    while (1) {
        fgets(buf, sizeof(buf), stdin);
        buf[strlen(buf) - 1] = '\0'; // Chop off newline.

        int err = inet_pton(AF_INET,
                            (const char *) &buf,
                            &(address->sin_addr.s_addr));

        // Allow early escape.
        if (buf[0] == 'q' || buf[0] == 'Q') {
            printf("Quitting...\n");
            exit(0);
        }

        if (err > 0) {
            break;
        }
        else if (err == 0) {
            printf("Improperly formatted address '%s'\n", buf);
            printf("Try again: ");
            fflush(stdout);
        }
        else if (err == -1) {
            printf("Could not convert address '%s' to numeric\n", buf);
            printf("Try again: ");
            fflush(stdout);
        }
    }
}

void prompt_for_port(struct sockaddr_in *address, char *who)
{
    char buf[INP_BUF_SIZE];
    int port;

    printf("Enter the %s's port number: ", who);
    while (1) {
        fgets(buf, sizeof(buf), stdin);
        buf[strlen(buf) - 1] = '\0'; // Chop off newline.

        // Allow early escape.
        if (buf[0] == 'q' || buf[0] == 'Q') {
            printf("Quitting...\n");
            exit(0);
        }

        port = atoi(buf);
        if (port >= 1 && port <= 65535) {
            break;
        }
        else {
            printf("Bad port number. Must be within 1-65,535.\n");
            printf("Try again: ");
            fflush(stdout);
        }
    }

    // Convert port number to network byte order.
    address->sin_port = htons(port);
}



void send_msg(int sd, union any_msg *to_send)
{
    int type = to_send->any.msg_type;
    switch (type) {
        case CMD_SEND:
        case CMD_RECV: // Fallthrough allows "or".
        {
            struct send_msg *msgp = (struct send_msg *) to_send;
            // UNIMPLEMENTED
            break;
        }
        case CMD_RESP:
        {
            struct resp_msg *msgp = (struct resp_msg *) to_send;
            // UNIMPLEMENTED
            break;
        }
        case CMD_DATA:
        {
            struct data_msg *msgp = (struct data_msg *) to_send;
            send_data(sd, msgp);
            break;
        }
        default:
            fprintf(stderr, "send_msg: Bad msg_type '%x'!\n", type);
            exit(1);
    }
}

void recv_msg(int sd, union any_msg *receiving_buf, int msg_type)
{
    int bytes_expected = MSG_SIZE(msg_type);    
    int bytes_recvd = recv(sd, (void *) receiving_buf, bytes_expected, 0);

    if (bytes_recvd == 0) {
        fprintf(stderr, "recv_msg: no data available, peer may have "
                        "been shutdown\n");
        exit(1);
    }
    else if (bytes_recvd == -1) {
        perror("recv_msg: could not recieve data from peer");
        exit(1);
    }
    else if (bytes_recvd != bytes_expected) {
        fprintf(stderr, "recv_msg: expected %d bytes, recv'd %d bytes\n",
                bytes_expected, bytes_recvd);
    }
}


void send_data(int sd, struct data_msg *to_send)
{
    int err = send(sd, (void *) msgp, sizeof(*msgp), 0);
    if (err == -1) {
        perror("send_data: could not send data_msg");
        exit(1);
    }
}

void recv_data(int sd, struct data_msg *receiving_buf)
{
    recv_msg(sd, (union any_msg *) recieving_buf, CMD_DATA);
}


void send_file(int sd, int fd)
{

}

void recv_file(int sd, char *filename, int file_size)
{

}

