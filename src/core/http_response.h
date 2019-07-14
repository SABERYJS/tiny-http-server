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

#ifndef STL_CLONE_HTTP_RESPONSE_H
#define STL_CLONE_HTTP_RESPONSE_H

#include "global_header.h"
#include "output_buffer.h"
#include "http.h"


#define HTTP_RESPONSE_STATUS_NORMAL 1
#define HTTP_RESPONSE_STATUS_ERROR 2

struct HttpResponse {
    struct Client *client;//current http client
    struct OutputBuffer *output;//buffer for output
    short status;//response status code
    struct Log *log;//debug purpose
};


void HttpResponseHandleInternalError(struct HttpResponse *response);

struct HttpResponse *HttpResponseCreate(struct Client *client, int readFd, struct Log *log);

int HttpResponseRegisterEvent(struct HttpResponse *response, struct EventDepositary *depositary);

int HttpResponseRegisterReadEvent(struct HttpResponse *response, struct EventDepositary *depositary);

int HttpResponseRegisterWriteEvent(struct HttpResponse *response, struct EventDepositary *depositary);

void HttpResponseReadableEventCallback(int type, void *data);

void HttpResponseWritableEventCallback(int type, void *data);

void HttpResponseRemoveReadableEvent(struct HttpResponse *response, struct EventDepositary *depositary);

void HttpResponseRemoveWritableEvent(struct HttpResponse *response, struct EventDepositary *depositary);

#endif //STL_CLONE_HTTP_RESPONSE_H
