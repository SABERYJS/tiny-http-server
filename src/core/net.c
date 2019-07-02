//
// Created by Administrator on 2019/7/2.
//

#include "net.h"

/**
 * convert ip string to number
 * **/
void *SockStrIpToNetByte(int family, void *src) {
    if (family != AF_INET && family != AF_INET6) {
        return NULL;
    }
    void *p;
    if (family == AF_INET) {
        p = malloc(sizeof(struct in6_addr));
    } else {
        p = malloc(sizeof(struct in_addr));
    }
    if (!p) {
        return NULL;
    } else {
        int ret = inet_pton(family, src, p);
        if (!ret) {
            free(p);
            return NULL;
        } else {
            return p;
        }
    }
}

/**
 * read n bytes from file descriptor
 * **/
size_t SockReadBytes(int fd, void *buf, size_t n) {
    size_t left = n;
    int ret;
    while (left) {
        readAgain:
        if ((ret = read(fd, buf, left)) < 0) {
            if (errno == EINTR) {
                goto readAgain;
            } else {
                return -1;
            }
        } else {
            if (ret == 0) {
                return (n - left);
            } else {
                left -= ret;
                buf += ret;
            }
        }
    }
}

/**
 * write n bytes to file descriptor
 * **/
size_t SockWriteBytes(int fd, void *buf, size_t n) {
    size_t left = n;
    int ret;
    while (left) {
        writeAgain:
        if ((ret = write(fd, buf, left)) <= 0) {
            if (errno == EINTR) {
                goto writeAgain;
            } else {
                return -1;
            }
        } else {
            left -= ret;
            buf += ret;
        }
    }
    return n;
}

/**
 * @param buf  struct sockaddr(ip4 and ip6 addr)
 * * **/
int SockClientGetSockName(int fd, void *buf, socklen_t *n) {
    return getsockname(fd, buf, n);
}