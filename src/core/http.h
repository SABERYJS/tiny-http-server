//
// Created by Administrator on 2019/7/5 0005.
//

#ifndef STL_CLONE_HTTP_H
#define STL_CLONE_HTTP_H

#include "global_header.h"
#include "client.h"
#include "event.h"
#include "buffer.h"
#include "log.h"

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

#define DomainValidChar(c)  ((c>='a' && c<='z') || (c>='A' &&c<='Z') ||c=='-' ||c=='.' ||(c>='0' &&c<='9') ||c==':')

#define CheckPortValidaty(port)  (port>=1 &&port<=65535)

#define HTTP_DEFAULT_PORT  80


struct HttpRequest {
    struct Client *client;
    struct Log *log;
};

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


#endif //STL_CLONE_HTTP_H
