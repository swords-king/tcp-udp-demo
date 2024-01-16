/* Copyright 2019 Tronlong Elec. Tech. Co. Ltd. All Rights Reserved. */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <pthread.h>

#define BUFFER_SIZE 1024 // buf size

struct CONNECT_INFO 
{
    bool is_connect;
    struct sockaddr_in client_addr;
};

static int sock_fd;

bool udp_server_init(short port) 
{
    struct sockaddr_in server_addr;

    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd == -1) 
    {
        perror("socket");
        return false;
    }

    // initialize address.
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // bind the local port.
    if ((bind(sock_fd, (struct sockaddr *)&server_addr, 
                            sizeof(struct sockaddr_in))) == -1) 
    {
        perror("bind");
        return false;
    }

    return true;
}

void *send_thread(void *arg) 
{
    struct CONNECT_INFO *info = (struct CONNECT_INFO *)arg;
    if (info == NULL) 
    {
        printf("error connect info");
        return (void*)-1;
    }

    while (true) 
    {
        char send_buf[BUFFER_SIZE];
        bzero(send_buf, sizeof(send_buf));

        // input from keyboard.
        char *str = fgets(send_buf, sizeof(send_buf), stdin);
        if (str == NULL) 
        {
            perror("fgets");
            return (void*)-1;
        }
        if (strlen(str) <= 1) 
        {
            continue;
        }

        // send data.
        if ((sendto(sock_fd, send_buf, strlen(str) - 1, 0,
                        (struct sockaddr *)&info->client_addr,
                        sizeof(struct sockaddr_in))) < 0) 
        {
            perror("sendto");
            return (void*)-1;
        }
    }

    return (void*)0;
}

int main(int argc, char *argv[]) 
{
    // check your input.
    if (argc != 2) 
    {
        printf("Usage:\n");
        printf("    %s <port number>\n", argv[0]);
        return -1;
    }

    short port = atoi(argv[1]);

    bool ret = udp_server_init(port);
    if (ret == false) 
    {
        goto err;
    }

    pthread_t t_id;
    struct CONNECT_INFO info;
    socklen_t addr_len = sizeof(struct sockaddr_in);

    printf("Waiting for client connectionn\n");
    while (1) 
    {
        // recive from client.
        char recv_buf[BUFFER_SIZE];
        bzero(recv_buf, sizeof(recv_buf));
        if ((recvfrom(sock_fd, recv_buf, sizeof(recv_buf), 0,
                        (struct sockaddr *)&info.client_addr, &addr_len)) <= 0) 
	{
            perror("recvfrom");
            goto err;
        }

        printf("[Recv from client %s:%d] : %s\n",
                inet_ntoa(info.client_addr.sin_addr),
                htons(info.client_addr.sin_port),
                recv_buf);

        if (info.is_connect) 
	{
            // send to client.
            pthread_create(&t_id, NULL, send_thread, (void *)&info);
            pthread_detach(t_id);
        }
    }

    close(sock_fd);
    return 0;

err:
    close(sock_fd);
    return -1;
}
