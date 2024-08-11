
#include <iostream>
#include "udpserver.h"

#define SERVERPORT 12345 

/* one very simple job: start the server */
int main(int argc, char *argv[]) {
    std::cerr << std::unitbuf;
    UDPServer server(SERVERPORT);
}
