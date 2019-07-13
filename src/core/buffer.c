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

#include "buffer.h"

int ReadFromSource(int socket, struct ClientBuffer *buffer) {
    BufferMoveForward(buffer);
    void *pos = buffer->buf + buffer->write_pos;//write from the position
    // bytes  count that can be read , because buffer size is limited
    size_t cread = CLIENT_RECEIVE_BUFFER_SIZE - buffer->size;
    if (cread == 0) {
        //buffer is full
        return 0;
    }
    int rb;
    again:
    rb = read(socket, pos, cread);
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

/**
 * has read some data,so discard them
 * **/
void BufferDiscard(struct ClientBuffer *buffer, size_t off) {
    buffer->read_pos += off;
    buffer->size -= off;
}

/**
 * move left chars ,so we can read more  data from  socket
 * **/
void BufferMoveForward(struct ClientBuffer *buffer) {
    if (buffer->size < CLIENT_RECEIVE_BUFFER_SIZE / 2) {
        //enough data to read
        return;
    }
    memcpy(buffer->buf, buffer->buf + buffer->read_pos, buffer->size);
    buffer->read_pos = 0;
    buffer->write_pos = buffer->size;
}

struct ClientBuffer *BufferCreate() {
    struct ClientBuffer *buffer = MemAlloc(sizeof(struct ClientBuffer));
    if (!buffer) {
        return NULL;
    } else {
        buffer->size = buffer->write_pos = buffer->read_pos = 0;
        buffer->left = 0;
        memset(buffer->buf, 0, CLIENT_RECEIVE_BUFFER_SIZE);
        return buffer;
    }
}
