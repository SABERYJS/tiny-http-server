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

#ifndef STL_CLONE_HTTP_H
#define STL_CLONE_HTTP_H

#include "global_header.h"
#include "client.h"
#include "event.h"
#include "buffer.h"
#include "log.h"
#include "string-tool.h"

#define CHAR_ENTER ('\r')
#define CHAR_NEW_LINE ('\n')
#define CHAR_BACKSLASH ('/')
#define CHAR_QUESTION_MARK ('?')
#define CHAR_HASH_TAG ('#')
#define CHAR_ADDRESS ('&')

#define HTTP_METHOD_GET 1
#define HTTP_METHOD_POST 2
#define HTTP_METHOD_HEAD 3
#define HTTP_METHOD_PUT 4
#define HTTP_METHOD_DELETE 5
#define HTTP_METHOD_OPTIONS 6
#define HTTP_METHOD_TRACE 7
#define HTTP_METHOD_PATCH 8


#define HTTP_METHOD_GET_LABEL  "get"
#define HTTP_METHOD_POST_LABEL  "post"
#define HTTP_METHOD_HEAD_LABEL  "head"
#define HTTP_METHOD_PUT_LABEL  "put"
#define HTTP_METHOD_DELETE_LABEL  "delete"
#define HTTP_METHOD_OPTIONS_LABEL  "options"
#define HTTP_METHOD_TRACE_LABEL  "trace"
#define HTTP_METHOD_PATCH_LABEL  "patch"

#define MAX_REQUEST_URL_LENGTH 1024


#define CharIsValid(c) ((c>='a' &&c<='z') || (c>='A' &&c<='Z'))
#define CharIsEnter(c) (c==CHAR_ENTER)
#define CharIsNewLine(c)  (c==CHAR_NEW_LINE)
#define CharIsSpace(c)  (c==' '|| c=='\t')
#define CharDot(c)  (c=='.')
#define CharIsColon(c)  (c==':')
#define CharIsNumber(c)  (c>='0' && c<='9')
#define IsLocalhost(url)  (strncmp(url,"localhost",9)==0)
#define CharIsAlpha(c)  ((c>='a' && c<='z') || (c>='A' &&c<='Z'))
#define CharBar(c) (c=='-')
#define CharEqual ('=')
#define CHARSPACE (' ')
#define CharIsEqual(c)  (c==CharEqual)

#define DomainValidChar(c)  ((c>='a' && c<='z') || (c>='A' &&c<='Z') ||c=='-' ||c=='.' ||(c>='0' &&c<='9') ||c==':')

#define CheckPortValidaty(port)  (port>=1 &&port<=65535)

#define HTTP_DEFAULT_PORT  80


struct HttpRequest {
    struct Client *client;
    struct Log *log;
};

static inline short HttpCheckVersionValidity(char *version) {
    return (atoi(version) - 1.1) < 0.0001;
}

short HttpParseUrl(const char *url, size_t len, struct Client *client);

/**
 * Create http request client
 * **/
struct HttpRequest *HttpRequestCreate(int socket, struct sockaddr_in *addr);

/**
 * handle socket  descriptor event callback
 * **/
void HttpEventHandleCallback(int type, void *data);

int HttpParseRequestLine(struct HttpRequest *request);

int HttpParseRequestMethod(struct HttpRequest *request);

int HttpParseHostHeader(struct HttpRequest *request, char *header, size_t len);

int HttpParseRequestLineUrl(struct HttpRequest *request);

int HttpParseRequestVersion(struct HttpRequest *request);

int HttpParseHeader(struct HttpRequest *request);

int HttpParseSpecifedHeader(struct HttpRequest *request, char *name, char *value);

int HttpParseHeaderHost(struct HttpRequest *request, char *value);

int HttpParsePath(struct HttpRequest *request, char *path);


#endif //STL_CLONE_HTTP_H
