//
// Created by Administrator on 2019/7/5 0005.
//

#ifndef STL_CLONE_BUFFER_H
#define STL_CLONE_BUFFER_H

#include "global_header.h"

#define CLIENT_RECEIVE_BUFFER_SIZE 1024

struct ClientBuffer {
    char buf[CLIENT_RECEIVE_BUFFER_SIZE];//buffer that receive client input
    size_t size; //bytes that has been read
    int read_pos; //next read position
    int write_pos; //next write position
    int left; //space left of buffer
};

#define BufferCharAtPos(buffer, pos) (buffer->buf[buffer->read_pos+pos])

#define BufferSubstr(buffer, start) (buffer->buf+buffer->read_pos+start)

int ReadFromSource(int socket, struct ClientBuffer *buffer);

void BufferDiscard(struct ClientBuffer *buffer, size_t off);

void BufferMoveForward(struct ClientBuffer *buffer);


#endif //STL_CLONE_BUFFER_H
