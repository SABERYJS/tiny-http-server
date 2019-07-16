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


#ifndef STL_CLONE_CLIENT_H
#define STL_CLONE_CLIENT_H

#include "global_header.h"
#include "buffer.h"
#include "http.h"
#include "log.h"
#include "http_response.h"

#define  REQUEST_METHOD_GET 1
#define  REQUEST_METHOD_POST 2

#define STATUS_RECEIVING_HEADER  1
#define STATUS_HEADER_RECEIVED_FINISHED 2
#define STATUS_RECEIVING_BODY 3
#define STATUS_RECEIVED_FROM_CLIENT_FINISHED 4

#define HTTP_REQUEST_LINE_METHOD 1
#define HTTP_REQUEST_LINE_URL 2
#define HTTP_REQUEST_LINE_HTTP_VERSION 3

#define HTTP_REQUEST_LINE_FINISHED 4

struct HttpHeader {
    char *name;
    char *value;
};

struct HttpQueryParam {
    char *name;
    char *value;
};

struct Client {
    struct HttpRequest *request;//belong to which http request
    struct sockaddr_in *addr; //only support ipv4
    char *ip; //client ip address
    struct HashTable *headers; //client headers
    float http_version; //http version
    char *request_url; //full request url
    int method; //request method
    char *port; //request port
    char *host; //request host
    char *uri; //request url except domain and query string
    int sock; //client  socket  file descriptor,
    unsigned int content_length;// request body length
    short status;//current client parse status
    struct ClientBuffer *buffer;//buffer that store client input
    short request_line_parse_status;//request  line parse status,
    short ssl;//ssl request,
    struct HashTable *query;//params in query string
    char *hasTag;//hash tag
    struct Log *log;//for debug
    struct HttpResponse *response;
    char *query_string;//full query string
    char *protocol_version;//protocol version
    char *tMethod;//method text
    char *entry_file;//entry file
    char *script_name;//script_name for cgi
    char *path_info;//path info for cgi
};

struct Client *ClientCreate(int sock, struct sockaddr_in *clientAddr);

#endif //STL_CLONE_CLIENT_H
