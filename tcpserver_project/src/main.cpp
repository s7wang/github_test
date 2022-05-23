#include <stdio.h>
#include <unistd.h>
#include "TcpServer.h"

#define PATH    "/home/wangs7/localsocket"

int main(int argc, const char** argv) {
    
    TcpServer server;

    server.tcp_server_start(PATH);

    sleep(1000);

    return 0;
}