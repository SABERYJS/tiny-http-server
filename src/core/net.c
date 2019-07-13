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