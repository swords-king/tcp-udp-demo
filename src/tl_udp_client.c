/* Copyright 2019 Tronlong Elec. Tech. Co. Ltd. All Rights Reserved. */

#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024 // buf size

int main(int argc, char *argv[]) {
    // check the arguments.
    if (argc != 3) {
        printf("Usage:\n");
        printf("    %s <server_addr> <portnumber>\n", argv[0]);
        return -1;
    }

    int sock_fd;
    char send_buf[BUFFER_SIZE];
    char recv_buf[BUFFER_SIZE];
    struct sockaddr_in server_addr;

    char *addr = argv[1];
    short port = atoi(argv[2]);

    // initialize address.
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(addr);
    server_addr.sin_port = htons(port);

    // create udp socket.
    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd == -1) {
        perror("socket");
        return -1;
    }

    pid_t pid = fork();
    if (pid > 0) {
        // The parent process is responsible for sending messages.
        while (1) {
            bzero(send_buf, sizeof(send_buf));    

            // read from stdin.
            char *str = fgets(send_buf, sizeof(send_buf), stdin);
            if (str == NULL) {
                perror("fgets");
                goto err;
            }
            if (strlen(str) <= 1) {
                continue;
            }

            // send data.
	unsigned int cnt1 = 0;
        for(cnt1 = 0; cnt1 < 256; cnt1++)send_buf[cnt1] = cnt1;
        if ((sendto(sock_fd, send_buf, 256, 0,
                            (struct sockaddr *)&server_addr,
                            sizeof(struct sockaddr_in))) < 0)    
	
	 /*if ((sendto(sock_fd, send_buf, strlen(str) - 1, 0,
                            (struct sockaddr *)&server_addr,
                            sizeof(struct sockaddr_in))) < 0)*/
	 {
                perror("sendto");
                goto err;
            }
        }
    }
    else if (pid == 0) {
        // The child process is responsible for receiving messages.
        while (1) {
            bzero(recv_buf, sizeof(recv_buf));
            socklen_t addr_len = sizeof(struct sockaddr_in);
            if ((recvfrom(sock_fd, recv_buf, sizeof(recv_buf), 0,
                            (struct sockaddr *)&server_addr,
                            &addr_len)) <= 0) {
                perror("recvfrom");
                goto err;
            }

            printf("[Recv from server %s:%d] : %s\n",
                    inet_ntoa(server_addr.sin_addr),
                    htons(server_addr.sin_port),
                    recv_buf);
        }
    }

    close(sock_fd);
    return 0;

err:
    close(sock_fd);
    return -1;
}
