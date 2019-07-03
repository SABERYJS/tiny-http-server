//
// Created by Administrator on 2019/7/1.
//
#include "test_header.h"
#include "../src/core/event.h"

int main() {
    printf("socket server\n");
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        printf("create socket failed\n");
        exit(1);
    } else {
        int on = 1;
        setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, 1);
        struct sockaddr_in serverAddr, clientAddr;
        memset(&serverAddr, 0, sizeof(struct sockaddr_in));//it is necessary
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(80);
        serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        if (bind(fd, (const struct sockaddr *) &serverAddr, sizeof(struct sockaddr_in)) < 0) {
            printf("bind socket addr failed,%s\n", strerror(errno));
            exit(1);
        } else {
            if (listen(fd, 10) < 0) {
                printf("listen failed\n");
                close(fd);
                exit(1);
            }
            int ret;
            socklen_t len;
            char buffer[1024];
            int rs;
            char *p;
            while (1) {
                acceptAgain:
                ret = accept(fd, (struct sockaddr *) &clientAddr, &len);
                if (ret == -1) {
                    printf("call accept failed,%s", strerror(errno));
                    if (errno == EINTR) {
                        goto acceptAgain;
                    }
                    exit(1);
                } else {
                    printf("client addr:%s\n", inet_ntoa(clientAddr.sin_addr));
                    memset(buffer, 0, 1024);
                    p = buffer;
                    while (1) {
                        readAgain:
                        rs = read(ret, p, 1024);
                        printf("here");
                        if (rs < 0) {
                            if (errno == EINTR) {
                                goto readAgain;
                            } else {
                                printf("read sys called failed\n");
                                close(fd);
                                exit(1);
                            }
                        } else {
                            if (rs == 0) {
                                printf("client close connection,receive from client:%s\n", buffer);
                                close(fd);
                                continue;
                            } else {
                                printf("read bytes:%d\n", rs);
                                p += rs;
                            }
                        }
                    }
                }
            }
        }
    }
}
