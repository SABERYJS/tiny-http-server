//
// Created by Administrator on 2019/7/5 0005.
//

#include "buffer.h"

int ReadFromSocket(int socket, struct ClientBuffer *buffer) {
    BufferMoveForward(buffer);
    void *pos = buffer->buf + buffer->write_pos;//write from the position
    // bytes  count that can be read , because buffer size is limited
    size_t cread = CLIENT_RECEIVE_BUFFER_SIZE - buffer->size;
    if (cread == 0) {
        //buffer is full
        return 0;
    }
    int rb = read(socket, pos, CLIENT_RECEIVE_BUFFER_SIZE);
    again:
    if (rb == -1) {
        if (errno == EINTR) {
            //system call interrupted
            goto again;
        } else {
            return -1;
        }
    } else {
        buffer->size += rb;
        buffer->write_pos += rb;
        return rb;
    }
}

void BufferDiscard(struct ClientBuffer *buffer, size_t off) {
    buffer->read_pos += off;
    buffer->size -= off;
}

void BufferMoveForward(struct ClientBuffer *buffer) {
    if (buffer->size < CLIENT_RECEIVE_BUFFER_SIZE / 3) {
        return;
    }
    memcpy(buffer->buf, buffer->buf + buffer->read_pos, buffer->size);
    buffer->read_pos = 0;
    buffer->write_pos = buffer->size;
}