/* Copyright 2019 Tronlong Elec. Tech. Co. Ltd. All Rights Reserved. */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ipc.h>
#include <errno.h>
#include <sys/shm.h>
#include <time.h>
#include <pthread.h>
#include <arpa/inet.h>

#define BUFFER_SIZE     1024    // buf size
#define LISTENQ         1       // the max quote

#define SIZE_SHMADD 2048

int main(int argc, char *argv[]) {
    // check the arguments.
    if (argc != 2) {
        printf("Usage:\n");
        printf("    %s <port number>\n", argv[0]);
        return -1;
    }

    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;

    char recv_buf[BUFFER_SIZE];
    char send_buf[BUFFER_SIZE];

    pid_t pid;
    int sock_fd, conn_fd = -1;
    int optval = 1;
    int port = atoi(argv[1]);

    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        return -1;
    }

    // setting bind port crycled.
    if ((setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR,
                                    (void *)&optval, sizeof(int))) < 0) {
        perror("setsockopt");
        goto err;
    }

    // initized address.
    bzero(&server_addr, sizeof(struct sockaddr_in));
    server_addr.sin_family=AF_INET;
    server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    // bind the port.
    if (bind(sock_fd, (struct sockaddr *)(&server_addr),
                                        sizeof(struct sockaddr)) == -1) {
        perror("bind");
        goto err;
    }
    
    // transform the socket into listen socket.
    if (listen(sock_fd, LISTENQ) == -1) {
        perror("listen");
        goto err;
    }

    printf("Waiting for client connectionn...\n");

    socklen_t cli_len = sizeof(struct sockaddr_in);
    conn_fd = accept(sock_fd, (struct sockaddr *)(&client_addr), &cli_len);
    if (conn_fd == -1) {
        perror("accept");
        close(sock_fd);
        return -1;
    }

    printf("Client accepted : %s:%d \n",
            inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    pid = fork();
    if (pid > 0) {
        // The parent process is responsible for sending messages.
        while (1) {
            bzero(send_buf, sizeof(send_buf));
            char *str = fgets(send_buf, sizeof(send_buf), stdin);
            if (str == NULL) {
                perror("fgets");
                goto err;
            }

            if (send(conn_fd, send_buf, strlen(send_buf) - 1, 0) < 0) {
                perror("send");
                goto err;
            }
        } 
    }
    else if (pid == 0) {
        // The child process is responsible for receiving messages.
        while (1) {
            bzero(recv_buf, sizeof(recv_buf));
            if ((recv(conn_fd, recv_buf, BUFFER_SIZE, 0)) <= 0) {
                perror("recv");
                goto err;
            }

            printf("[Recv from client %s:%d] : %s\n",
                    inet_ntoa(server_addr.sin_addr),
                    htons(server_addr.sin_port),
                    recv_buf);
        }
    }
    else {
        perror("fork");
        goto err;
    }

    close(conn_fd);
    close(sock_fd);
    return 0;

err:
    close(conn_fd);
    close(sock_fd);
    return -1;
}
