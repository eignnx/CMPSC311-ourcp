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
#define BUFFER_BYTES 1024

// Used in msg structures.
// Values are arbitrary, but may help in debugging.
#define CMD_SEND 0xDEADBEEF
#define CMD_RECV 0x1337C0DE
#define CMD_DATA 0xFACEFEED

#define OK 0


// User input functions common to both client and server.
// The `who` parameter is used for display purposes. It's value
// should be either "server" when called from server.c, or
// "client" when called from client.c.
void prompt_for_address(struct sockaddr_in *address, char *who);
void prompt_for_port(struct sockaddr_in *address, char *who);


