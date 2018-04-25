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
            exit(EXIT_SUCCESS);
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
            exit(EXIT_SUCCESS);
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


bool locate_file(const char *filename, int *fd)
{
    *fd = open(filename, O_RDONLY);
    
    if (*fd == -1) {
        *fd = errno;
        return false;
    }

    return true;
}

int size_of_file(int fd)
{
    struct stat filestatus;

    int err = fstat(fd, &filestatus);
    if (err == -1) {
        perror("size_of_file: Could not get stat structure when "
               "given a file descriptor");
        exit(EXIT_FAILURE);
    }

    return (int) filestatus.st_size;
}


void send_msg(int sd, union any_msg *to_send)
{
    MsgType type = to_send->any.msg_type;
    
    bool valid =
        type == CMD_SEND ||
        type == CMD_RECV ||
        type == CMD_RESP ||
        type == CMD_DATA;

    if (valid) {
        int err = send(sd, (void *) to_send, MSG_SIZE(type), 0);
        if (err == -1) {
            perror("send_msg: could not send message");
            fprintf(stderr, "\tMessage type was '%x'\n", type);
            exit(EXIT_FAILURE);
        }
    }
    else {
        fprintf(stderr, "send_msg: unrecognized message type '%x'!\n",
                type);
        exit(EXIT_FAILURE);
    }

}

void recv_msg(int sd, union any_msg *receiving_buf, MsgType msg_type)
{
    int bytes_expected = MSG_SIZE(msg_type);
    int bytes_recvd = recv(sd, (void *) receiving_buf, bytes_expected, 0);

    if (bytes_recvd == 0) {
        fprintf(stderr, "recv_msg: no data available, peer may have "
                        "been shutdown\n");
        exit(EXIT_FAILURE);
    }
    else if (bytes_recvd == -1) {
        // errno will be generated, so just print that.
        perror("recv_msg: could not recieve data from peer");
        exit(EXIT_FAILURE);
    }
    else if (bytes_recvd != bytes_expected) {
        fprintf(stderr, "recv_msg: expected %d bytes, recv'd %d bytes\n",
                bytes_expected, bytes_recvd);
    }
}


void send_file(int sd, int fd)
{
    struct data_msg msg = {
        .msg_type = CMD_DATA,
        .data_leng = 0
    };

    const int buf_size = sizeof(msg.buffer);

    while ((msg.data_leng = read(fd, msg.buffer, buf_size)) != 0) {

        if (msg.data_leng == -1) { // Must inform peer.
            msg.data_leng = -errno; // Send errno to peer.
            perror("send_msg: an error occured while reading file");
            fprintf(stderr, "Aborting file transmission.\n");
            break;
        }

        send_msg(sd, (union any_msg *) &msg);
        printf("Sent %d bytes from file\n", msg.data_leng);
    }

    // If all went well, msg.data_leng == 0 here.
    // Send final data message with nothing in buffer to
    // signal end of transmission.
    //
    // If an error occurred, msg.data_leng == -errno.
    // This will be caught and handled by `recv_file`.
    send_msg(sd, (union any_msg *) &msg);
}


// Exits with error message if error occurs.
// Pass the `callee` parameter the value `__func__` for better error info.
static int safe_open_for_write(const char *filename, const char *callee)
{
    int fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, OPEN_PERMS);

    if (fd == -1) {
        char err_msg[MAX_FILENAME_SIZE + 40];
        sprintf(err_msg, "%s: Could not create/overwrite file "
                         "with name '%s'", callee, filename);
        perror(err_msg);
        exit(EXIT_FAILURE);
    }

    return fd;
}

// Exits with error message if error occurs.
// Pass the `callee` parameter the value `__func__` for better error info.
static void safe_close(int fd, const char *callee)
{
    if (close(fd) == -1) {
        char msg[100];
        sprintf(msg, "%s: An error occured while trying to close the "
                     "output file", callee);
        perror(msg);
        exit(EXIT_FAILURE);
    }
}

// Exits with error message if error occurs.
// Pass the `callee` parameter the value `__func__` for better error info.
static int safe_write(int fd, const char *buf, int count,
                      const char* callee)
{

    int bytes_written = write(fd, buf, count);

    if (bytes_written == -1) {
        char msg[100];
        sprintf(msg, "%s: Unable to write to output file", callee);
        perror(msg);
        safe_close(fd, __func__);
        exit(EXIT_FAILURE);
    }
    
    if (bytes_written != count) {
        fprintf(stderr,
                "%s: Could not write all %d bytes to output file.\n"
                "\tExpected: %d\n"
                "\tWrote:    %d\n",
                callee, count, count, bytes_written);
        safe_close(fd, __func__);
        exit(EXIT_FAILURE);
    }

    return bytes_written;
}


void recv_file(int sd, char *filename, int file_size)
{
    struct data_msg buf;
    int total_bytes = 0;

    int fd = safe_open_for_write(filename, __func__);

    while (1) {
        recv_msg(sd, (union any_msg *) &buf, CMD_DATA);

        if (buf.msg_type != CMD_DATA) {
            fprintf(stderr, "recv_file: Unexpected msg type '%x'! "
                    "Aborting file transfer.\n", buf.msg_type);
            safe_close(fd, __func__);
            exit(EXIT_FAILURE);
        }

        if (buf.data_leng <= 0)
            break;
        
        printf("Read %d bytes\n", buf.data_leng);

        total_bytes += safe_write(fd, buf.buffer, buf.data_leng, __func__);
    }

    if (buf.data_leng < 0) {
        errno = -buf.data_leng; // Overwrite errno in current process.
        perror("recv_file: An error occurred while recv'ing data from "
               "peer");
        fprintf(stderr, "Aborting file transfer.\n");

        // Deletion of output file (will happen when `fd` is closed).
        if (remove(filename) == -1) {
            perror("recv_file: Removal of incomplete output file failed");
        }
    }
    else if (file_size != total_bytes) {
        fprintf(stderr, "recv_file: File recv'd has different size than "
                "expected. Removing incomplete output file...\n");

        // Deletion of output file (will happen when `fd` is closed).
        if (remove(filename) == -1) {
            perror("recv_file: Removal of incomplete output file failed");
        }
    }

    printf("Wrote %d bytes to output file '%s'\n", total_bytes, filename);
    safe_close(fd, __func__);
}

