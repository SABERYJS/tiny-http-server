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

#ifndef STL_CLONE_BUFFER_H
#define STL_CLONE_BUFFER_H

#include "global_header.h"

#define CLIENT_RECEIVE_BUFFER_SIZE 1024*1024

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

struct ClientBuffer *BufferCreate();

int WriteToBufferManual(struct ClientBuffer *buffer, const char *src, size_t len);

int WriteOneByteToBufferManual(struct ClientBuffer *buffer, char c);


#endif //STL_CLONE_BUFFER_H
