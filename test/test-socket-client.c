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

int main() {
    printf("socket client start\n");
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        printf("create socket failed\n");
        exit(1);
    } else {
        int on = 1;
        setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &on, 1);
        struct sockaddr_in serverAddr;
        memset(&serverAddr, 0, sizeof(struct sockaddr_in));//it is necessary
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(81);
        inet_aton("127.0.0.1", &serverAddr.sin_addr);
        if (connect(fd, (const struct sockaddr *) &serverAddr, sizeof(struct sockaddr_in)) < 0) {
            printf("connect server failed\n");
            exit(1);
        } else {
            char buf[10];
            int ws;
            memset(buf, 0, 10);
            while (gets(buf)) {
                if (strncmp(buf, "exit", 4) == 0) {
                    printf("client input finished\n");
                    close(fd);
                    exit(0);
                } else {
                    printf("write %s,size:%lu\n", buf, strlen(buf));
                    ws = write(fd, buf, strlen(buf));
                    if (ws <= 0) {
                        printf("write failed\n");
                        exit(1);
                    }
                }
            }
        }
    }
}
