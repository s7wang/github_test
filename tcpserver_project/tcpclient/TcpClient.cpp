#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/un.h>

#include <thread>

#define PATH    "/home/wangs7/localsocket"

char client_paths[4][32] = {
    "/home/wangs7/clientsocket0", 
    "/home/wangs7/clientsocket1", 
    "/home/wangs7/clientsocket2",
    "/home/wangs7/clientsocket3"
} ;

unsigned char data[3][5] ={
    {0xff, 0x11, 0x22, 0x33, 0x44}, 
    {0x55, 0x66, 0x77, 0x88, 0x99}, 
    {0x12, 0x34, 0x56, 0x78, 0x90}, 
    };

void printf_data(unsigned char* data, int len)
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


void local_socket_send(int thread_id)
{
    // unlink(client_paths[thread_id]);
    // 1.创建套接字
    int cfd = socket(AF_LOCAL, SOCK_STREAM, 0);

    // 3.连接服务器
    struct sockaddr_un seraddr;
    seraddr.sun_family = AF_LOCAL;
    strcpy(seraddr.sun_path, PATH);
    int ret = connect(cfd, (struct sockaddr *)&seraddr, sizeof(seraddr));
    if(ret == -1) {
        perror("connect");
        exit(-1);
    }
    printf("id:%d connect successed!\n", thread_id);
    send(cfd, data[thread_id], 5, 0);
    unsigned char buf[128] = {0};
    int len = recv(cfd, buf, 128, 0);
    
    printf("[Thread%d]:recv:", thread_id);
    printf_data(buf, len);
    
    
    close(cfd);
    printf("id:%d close successed!\n", thread_id);

}


int main(int argc, char* argv[])
{

    std::thread t0(local_socket_send, 0);
    std::thread t1(local_socket_send, 1);
    std::thread t2(local_socket_send, 2);

    t0.join();
    t1.join();
    t2.join();

    return 0;
}