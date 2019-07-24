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
#include "http_code.h"
#include "http_header.h"


#define HTTP_RESPONSE_STATUS_NORMAL 1
#define HTTP_RESPONSE_STATUS_ERROR 2

#define RESPONSE_LINE_TAIL "\r\n"

#define HTTP_RESPONSE_HEADER_BUFFER_SIZE 1024  //1kb

#define ContentTypeDetect(header_name)  (strncasecmp(header_name,"Content-Type",12)==0)


struct HttpResponse {
    struct Client *client;//current http client
    struct OutputBuffer *output;//buffer for output
    short status;//response status code
    struct Log *log;//debug purpose
    short pipe_closed;//flag:pipe connected to CGI closed
    short client_closed;//flag:server closed connection manual
    short status_line_sent;//flag:whether http status line has been send
    struct List *headers;//headers to be send to client
    short header_parse_finished;//flag:header has been parse finished
    short status_parsed;//status line parse finished
    char header_buffer[HTTP_RESPONSE_HEADER_BUFFER_SIZE];//buffer to store header
    int response_status_code;//response http status code
    short content_type_checked;//whether content type has been checked
    short header_sent;//whether http response header has been sent
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


int HttpResponseWriteStatusLine(struct HttpResponse *response, int code);

int HttpResponseWriteHeader(struct HttpResponse *response, char *header, char *value);

int HttpResponseWriteLastHeader(struct HttpResponse *response, char *header, char *value);


static int HttpResponseHeaderCompare(struct ListNode *node, void *data);


static int HttpResponseParseHeader(struct HttpResponse *response);

static int HttpResponseWriteBody(struct HttpResponse *response);

static int HttpResponseWriteContentToClient(struct HttpResponse *response);

#endif //STL_CLONE_HTTP_RESPONSE_H
