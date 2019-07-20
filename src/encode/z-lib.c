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

#include "z-lib.h"


/**
 * Create  encode object
 * **/
struct ZlibEncode *EncodeCreateZlib(size_t chunk, struct Log *log, short isGzip) {
    struct ZlibEncode *encode = MemAlloc(sizeof(struct ZlibEncode));
    int ret;
    if (!encode) {
        return NULL;
    } else {
        encode->encode = MemAlloc(sizeof(struct Encode));
        if (!encode->encode) {
            MemFree(encode);
            return NULL;
        }
        encode->encode->deflate = EncodeZlibDeflate;
        encode->encode->inflate = NULL;
        encode->stream.zalloc = NULL;
        encode->stream.zfree = NULL;
        encode->stream.opaque = NULL;
        encode->chunk_size = chunk;
        encode->log = log;
        encode->isGzip = isGzip;
        unsigned int windowsBits = WINDOW_BITS;
        if (encode->isGzip) {
            windowsBits |= GZIP_ENABLE;
        }
        ret = deflateInit2(&encode->stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED, windowsBits,
                           DEFAULT_MEMORY_LEVEL, Z_DEFAULT_STRATEGY);
        if (ret != Z_OK) {
            MemFree(encode);
            ZlibPrintMessage(encode, ret);
            return NULL;
        } else {
            return encode;
        }
    }
}

int EncodeZlibDeflate(void *data, unsigned char *input, size_t il, unsigned char *output, size_t ol, short last) {
    struct ZlibEncode *encode = (struct ZlibEncode *) data;
    z_stream *stream = &encode->stream;
    struct Log *log = encode->log;
    unsigned char *op = output;
    size_t left = ol;
    int count = 0;
    stream->next_in = input;
    stream->avail_in = il;
    int decoded = 0;
    int flush;
    flush = last ? Z_FINISH : Z_NO_FLUSH;
    do {
        stream->next_out = op;
        stream->avail_out = left;
        //we do  not check deflate return value,because it is impossible error will happened
        deflate(stream, flush);
        //how many data consumed by stream
        decoded = left - stream->avail_out;
        count += decoded;
        //next write position
        op += decoded;
        //left space to write
        left -= decoded;
    } while (stream->avail_out == 0);
    return count;
}

static void ZlibPrintMessage(struct ZlibEncode *encode, int error_no) {
    LogError(encode->log, "gzip error:%s\n", ZlibStrerr(error_no));
}