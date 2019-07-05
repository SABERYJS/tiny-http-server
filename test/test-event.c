//
// Created by Administrator on 2019/7/1.
//
#include "test_header.h"
#include "../src/core/event.h"


struct ListenSocket {
    int fd;
    struct EventDepositary *depositary;
};

struct ClientSocket {
    int fd;
    struct EventDepositary *depositary;
};

void clientSocketHandleCallback(int type, void *data) {
    struct ClientSocket *csk = (struct ClientSocket *) data;
    char buffer[100];
    read(csk->fd, buffer, 100);
    printf("received from client:%s\n", buffer);
    //write(csk->fd, buffer, strlen(buffer));
}


void ListenSocketEventCallback(int type, void *data) {
    printf("listen socket called\n");
    struct ListenSocket *s = (struct ListenSocket *) data;
    struct sockaddr_in client;
    socklen_t l;
    int cfd = accept(s->fd, (struct sockaddr *) &client, &l);
    struct ClientSocket *csk = malloc(sizeof(struct ClientSocket));
    csk->fd = cfd;
    csk->depositary = s->depositary;
    EventAdd(s->depositary, EVENT_READABLE, cfd, csk, clientSocketHandleCallback);
}


int main() {
    printf("socket server start bootstrap\n");
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
        serverAddr.sin_port = htons(81);
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
            struct EventDepositary *depositary = EventCreateDepositary(NULL);
            if (!depositary) {
                printf("create depositary failed\n");
                exit(1);
            } else {
                struct ListenSocket socket1;
                socket1.fd = fd;
                socket1.depositary = depositary;
                ret = EventAdd(depositary, EVENT_READABLE, fd, &socket1, ListenSocketEventCallback);
                if (!ret) {
                    printf("add event failed\n");
                } else {
                    while (1) {
                        EventLoop(depositary);
                    }
                }
            }
        }
    }
}
