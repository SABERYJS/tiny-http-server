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

#include "output_buffer.h"

struct OutputBuffer *OutputBufferCreate(int rfd, int wfd, struct Log *log) {
    struct OutputBuffer *output = MemAlloc(sizeof(struct OutputBuffer));
    if (!output) {
        return NULL;
    } else {
        output->buffer = BufferCreate();
        output->transfer_station = BufferCreate();
        if (!output->buffer) {
            MemFree(output);
            return NULL;
        } else {
            output->providerFd = rfd;
            output->targetFd = wfd;
            output->log = log;
            return output;
        }
    }
}

/**
 * this method should be called by event system
 * **/
int OutputBufferCanRead(struct OutputBuffer *output) {
    struct ClientBuffer *readBuffer = output->buffer;
    int ret;
    if ((ret = ReadFromSource(output->providerFd, output->buffer)) < 0) {
        return -1;
    } else {
        //ret maybe 0
        return ret;
    }
}

/**
 * this method should be called by event system
 * **/
int OutputBufferCanWrite(struct OutputBuffer *output) {
    struct ClientBuffer *transfer_station = output->transfer_station;
    size_t canRead = transfer_station->size;
    int wt = 0;
    if (canRead <= 0) {
        //no more data to write
        return 1;
    } else {
        while (1) {
            again:
            if ((wt = write(output->targetFd, BufferSubstr(transfer_station, 0), canRead)) < 0) {
                if (errno == EINTR) {
                    goto again;
                } else {
                    return -1;
                }
            } else {
                //we do not care about how  many data has been wrote to client socket
                //discard transfer station buffer some data
                BufferDiscard(transfer_station, wt);
                return 1;
            }
        }
    }
}

/**
 * calculate rest space to write
 * **/
int OutputBufferTransferStationSpaceRest(struct OutputBuffer *output) {
    return (CLIENT_RECEIVE_BUFFER_SIZE - output->transfer_station->write_pos);
}

/**
 * readable bytes for transfer station
 * **/
int OutputBufferTransferStationReadableSize(struct OutputBuffer *output) {
    return output->transfer_station->size;
}

/**
 * readable bytes for internal buffer
 * **/
int OutputBufferInternalReadableSize(struct OutputBuffer *output) {
    return output->buffer->size;
}