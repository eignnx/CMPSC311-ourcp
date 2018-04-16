#include <stdio.h>      // printf, sprintf
#include <stdlib.h>     // exit
#include <string.h>     // strcmp
#include <stdbool.h>    // true, false

#include <unistd.h>     // read, write, close

#include <sys/types.h>  // open
#include <sys/stat.h>   // open
#include <fcntl.h>      // open

#include <sys/types.h>  // COMPATIBILITY (see `man socket`)
#include <sys/socket.h> // socket
#include <netinet/in.h> // struct sockaddr_in
#include <arpa/inet.h>  // htons, inet_pton


extern int errno;

// Define permissions for writing files.
#define OPEN_PERMS 0644


////// Message Structures //////

#define MAX_FILENAME_SIZE 128
#define MAX_DATA_SIZE 1024


// First four hex digits chosen to make debugging easier (should
// we need to read hex dumps). Second four digits specify the size
// of the assocated struct. Assumes sizeof all the structs are less
// than 16^4 bytes.

// Sending file from CLIENT to SERVER
#define CMD_SEND (0xAAAA0000 | sizeof(struct send_msg))
// Sending file from SERVER to CLIENT
#define CMD_RECV (0xBBBB0000 | sizeof(struct send_msg))
// Sent by server to client as acknowledgement
#define CMD_RESP (0xCCCC0000 | sizeof(struct resp_msg))
// Header for a data message
#define CMD_DATA (0xDDDD0000 | sizeof(struct data_msg))

// Macro that extracts the `sizeof` the associated struct from
// a `msg_type` value.
#define MSG_SIZE(msg_type) ((msg_type) & 0x0000FFFF)


// To be used in the `status` field of a resp_msg structure.
#define OK 0

// Used by client to tell server either:
//      "I will be sending a file"
//      "I'd like to recieve a file"
struct send_msg {

    // Either CMD_SEND or CMD_RECV
    // Note: defined in proj.h
    int msg_type;

    // Size (in bytes) of file to be sent (or 0 if not applicable)
    int file_size;

    // Name of file to be send/recieved
    char filename[MAX_FILENAME_SIZE];
};

// Used by server to respond to a client's requests
struct resp_msg {

    // Either CMD_SEND or CMD_RECV
    int msg_type;

    // Either OK or else some value of `errno`
    // Note: `OK` is defined as 0 in proj.h
    int status;

    // Only used by server when transferring a file
    // from server to client (and when status is OK)
    int file_size;
};

struct data_msg {
    
    // Always CMD_DATA
    int msg_type;

    // Number of bytes of `buffer` that are actually filled
    int data_leng;

    // Data buffer
    char buffer[MAX_DATA_SIZE];
};


struct tag_msg {
    int msg_type;
};

// Generic message type. Could be either a send, resp, or data
// msg. The `tag_msg` variant is just there to provide a variant
// to switch on.
union any_msg {
    struct send_msg   send;
    struct resp_msg   resp;
    struct data_msg   data;

    // Not used except when switching on `msg_type`.
    struct tag_msg any;
};

////// Common Functions //////

// User input functions common to both client and server.
// The `who` parameter is used for display purposes. It's value
// should be either "server" when called from server.c, or
// "client" when called from client.c.
void prompt_for_address(struct sockaddr_in *address, char *who);
void prompt_for_port(struct sockaddr_in *address, char *who);

// Attempts to open a file called `filename` in the CWD. If successful,
// `*fd` is set to the file descriptor of the open file and the
// function returns true. If unsucessful, `*fd` is set to `errno`
// and the function returns false.
bool locate_file(const char *filename, int *fd);


// Generic message sending and recieving functions. To use, cast
// argument like:
//      (union any_msg *) &my_msg_stuct_of_any_type
// The argument `sd` is the socket descriptor to recv/send from/to.
// The argument `msg_type` accepts the type of the message you expect
// to recieve.
void send_msg(int sd, union any_msg *to_send);
void recv_msg(int sd, union any_msg *receiving_buf, int msg_type);

// Specialized send/recv functions for sending data messages.
// Only use with `struct data_msg` pointers, don't cast!
// The argument `sd` is the socket descriptor to recv/send from/to.
void send_data(int sd, struct data_msg *to_send);
void recv_data(int sd, struct data_msg *receiving_buf);

// Sends the file opened under file descriptor `fd` accross the
// network to the peer located by socket descriptor `sd`.
// Assumes both arguments have been opened already.
void send_file(int sd, int fd);

// Recieves a file from the peer located by socket descriptor `sd`
// and saves it to the file called `filename`. Assumes that the
// socket has been opened.
void recv_file(int sd, char *filename, int file_size);

