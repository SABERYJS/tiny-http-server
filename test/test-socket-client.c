//
// Created by Administrator on 2019/7/1.
//

#include "test_header.h"

int main() {
    printf("socket client\n");
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
        serverAddr.sin_port = htons(80);
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
                    // memset(buf,0,10);
                }
            }
        }
    }
}
