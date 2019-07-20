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

/**
 * about usage of gzip or zlip ,please refer to https://zlib.net/zlib_how.html
 * **/

#ifndef STL_CLONE_Z_LIB_H
#define STL_CLONE_Z_LIB_H

#include "encode_.h"
#include "../core/log.h"
#include <zlib.h>

#define CHUNK_SIZE 16384
#define  WINDOW_BITS 15
#define  GZIP_ENABLE 16
#define  DEFAULT_MEMORY_LEVEL 8

#define ZlibStrerr(error)  (error==Z_MEM_ERROR?("No enough Memory"):(error==Z_STREAM_ERROR?"Invalid Parameters":(error==Z_VERSION_ERROR?"Incompatible Version":"Unknown Reason")))

struct ZlibEncode {
    struct Encode *encode;
    z_stream stream;
    struct Log *log;//for debug
    size_t chunk_size;
    short isGzip;
};


int EncodeZlibDeflate(void *data, unsigned char *input, size_t il, unsigned char *output, size_t ol, short last);

int EncodeZlibInflate(void *data, char *input, size_t il, char *output, size_t ol);

struct ZlibEncode *EncodeCreateZlib(size_t chunk, struct Log *log, short isGzip);

static void ZlibPrintMessage(struct ZlibEncode *encode, int error_no);

#endif //STL_CLONE_Z_LIB_H
