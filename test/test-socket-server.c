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
