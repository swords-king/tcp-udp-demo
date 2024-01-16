/* Copyright 2019 Tronlong Elec. Tech. Co. Ltd. All Rights Reserved. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    // check the arguments.
    if (argc != 3) {
        printf("Usage:\n");
        printf("    %s <server addr> <port number>\n", argv[0]);
        return -1;
    }

    char *addr = argv[1];
    short port = atoi(argv[2]);

    int sock_fd;
    int ret;
    char recv_buf[BUFFER_SIZE], send_buf[BUFFER_SIZE];
    struct sockaddr_in server_addr;

    // initialize address/
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(addr);

    // create socket fd.
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd == -1) {
        perror("socket");
        return -1;
    }

    // connect.
    ret = connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));
    if (ret == -1) {
        perror("connect");
        goto err;
    }

    printf("connect server(IP:%s Port: %d).\n", addr, port);

    pid_t pid = fork();
    if (pid > 0) {
        // The parent process is responsible for sending messages.
        while (1) {
            bzero(send_buf, sizeof(send_buf));
            char *str = fgets(send_buf, sizeof(send_buf), stdin);
            if (str == NULL) {
                perror("fgets");
                goto err;
            }

            if (send(sock_fd, send_buf, strlen(send_buf) - 1, 0) < 0) {
                perror("send");
                goto err;
            }
        }
    }
    else if (pid == 0) {
        // The child process is responsible for receiving messages.
        while (1) {
            bzero(recv_buf, sizeof(recv_buf));
            if (sock_fd > 0) {
                if ((recv(sock_fd, recv_buf, BUFFER_SIZE, 0)) <= 0) {
                    perror("recv");
                    goto err;
                }

                printf("[Recv from server %s:%d] : %s\n",
                        inet_ntoa(server_addr.sin_addr),
                        htons(server_addr.sin_port),
                        recv_buf);
            }
        }
    }
    else {
        perror("fork");
        goto err;
    }

    close(sock_fd);
    return 0;

err:
    close(sock_fd);
    return -1;
}
