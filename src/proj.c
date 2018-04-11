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

        if (err == 0) {
            printf("Improperly formatted address '%s'\n", buf);
            printf(": ");
        }
        else if (err == -1) {
            sprintf(buf, "Could not convert address '%s' to numeric", buf);
            perror(buf);
            exit(1);
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

        if (strcmp(buf, "quit") == 0) {
            printf("Quitting...\n");
            exit(0);
        }

        port = atoi(buf);
        if (port >= 1 || port <= 65535) {
            break;
        }
        else {
            printf("Bad port number. Must be within 1-65,535.\n");
            printf(": ");
        }
    }

    // Convert port number to network byte order.
    address->sin_port = htons(port);
}
