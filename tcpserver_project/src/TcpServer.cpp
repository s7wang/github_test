#include "TcpServer.h"
#include <stdio.h>
#include <stdlib.h>

TcpServer::TcpServer()
{
}

TcpServer::~TcpServer()
{
}


void TcpServer::client_add_notify(int fd)
{
    printf("fd:%d linked.--------\n");
}
void TcpServer::client_remove_notify(int fd)
{
    printf("fd:%d unlinked.--------\n");
}

void printf_data(Uint8 *data, int len)
{
    if (data == NULL || len == 0)
    {
        fprintf(stderr, "data == NULL or len == 0. Please check INPUTS\n");
    }
    printf("[ ");
    for (int i = 0; i < len; i++)
    {
        printf("%x ", *(data + i));
    }
    printf("]\n");
}

void TcpServer::on_tcp_data_handle(int fd, Uint8* data, int len)
{
    printf("src fd:%d\tlen = %d\tdata:\n", fd, len);
    printf_data(data, len);
    write_to_send_queue(fd, data, len);
}