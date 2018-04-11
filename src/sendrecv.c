#include "proj.h"

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
    char buffer[BUFFER_BYTES];
};

