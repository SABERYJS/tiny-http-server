/*Copyright (c) 2019 , saberyjs@gmail.com
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. All advertising materials mentioning features or use of this software
   must display the following acknowledgement:
   This product includes software developed by the <organization>.
4. Neither the name of the <organization> nor the
   names of its contributors may be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ''AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "test_header.h"
#include "../src/core/event.h"
#include "../src/core/http.h"
#include "../src/core/server.h"
#include "../src/core/config.h"


struct ListenSocket {
    int fd;
    struct EventDepositary *depositary;
};

struct ClientSocket {
    int fd;
    struct EventDepositary *depositary;
    struct sockaddr_in clientAddr;
};

void clientSocketHandleCallback(int type, void *data) {
    struct ClientSocket *csk = (struct ClientSocket *) data;
    struct EventDepositary *depositary = csk->depositary;
    EventRemove(depositary, EVENT_READABLE, csk->fd);
    struct HttpRequest *request = HttpRequestCreate(csk->fd, &csk->clientAddr);
    if (!request) {
        printf("create request failed\n");
        return;
    }
    request->log = LogCreate(0, STDOUT_FILENO, NULL, LOG_LEVEL_INFO);
    if (!request->log) {
        printf("create log failed\n");
        return;
    }
    EventAdd(depositary, EVENT_READABLE, csk->fd, request, HttpEventHandleCallback);
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
    memcpy(&csk->clientAddr, &client, l);
    EventAdd(s->depositary, EVENT_READABLE, cfd, csk, clientSocketHandleCallback);
}


int main() {
    printf("start parse config file\n");
    struct Config *config = ConfigCreate("../app.conf");
    if (!config) {
        printf("Create config file failed,%s\n", strerror(errno));
        exit(0);
    }
    ConfigFileParse(config);
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
                server.depositary = depositary;
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
