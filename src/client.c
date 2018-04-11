#include "proj.h"

int main()
{
    struct sockaddr_in address;
    prompt_for_address(&address, "client");
    prompt_for_port(&address, "client");
}


