#include <stdio.h>      // printf, sprintf
#include <stdlib.h>     // exit
#include <string.h>     // strcmp
#include <stdbool.h>    // true, false

#include <sys/types.h>  // COMPATIBILITY (see `man socket`)
#include <sys/socket.h> // socket
#include <netinet/in.h> // struct sockaddr_in
#include <arpa/inet.h>  // htons, inet_pton


// For use in `sendrecv.c`
#define MAX_FILENAME_SIZE 128
#define MAX_DATA_SIZE 1024

// Used in msg structures.
// Values are arbitrary, but may help in debugging.
#define CMD_SEND 0xDEADBEEF // Sending file from CLIENT to SERVER
#define CMD_RECV 0x1337C0DE // Sending file from SERVER to CLIENT
#define CMD_RESP 0xC0FFEE00 // Sent by server to client as acknowledgement
#define CMD_DATA 0xFACEFEED // Header for a data message

#define OK 0


// User input functions common to both client and server.
// The `who` parameter is used for display purposes. It's value
// should be either "server" when called from server.c, or
// "client" when called from client.c.
void prompt_for_address(struct sockaddr_in *address, char *who);
void prompt_for_port(struct sockaddr_in *address, char *who);


// Sends the file opened under `file_descriptor` accross the
// network to the peer located by `socket_descriptor`.
// Assumes both arguments have been opened already.
void send_file(int socket_descriptor, int file_descriptor);

// Recieves a file from the peer located by `socket_descriptor`
// and saves it to the file called `filename`. Assumes that the
// socket has been opened, and the file to be recieved is
// `file_size` bytes in length.
void recv_file(int socket_descriptor, char *filename, int file_size);

