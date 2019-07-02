//
// Created by Administrator on 2019/7/2.
//

#ifndef QC_NET_H
#define QC_NET_H

#include "global_header.h"

void *SockStrIpToNetByte(int family, void *src);

size_t SockReadBytes(int fd, void *buf, size_t len);

size_t SockWriteBytes(int fd, void *buf, size_t n);

int SockClientGetSockName(int fd, void *buf, socklen_t *n);

#endif //QC_NET_H
