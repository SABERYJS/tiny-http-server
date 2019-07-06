//
// Created by Administrator on 2019/7/5 0005.
//

#ifndef STL_CLONE_CLIENT_H
#define STL_CLONE_CLIENT_H

#include "global_header.h"
#include "buffer.h"

#define  REQUEST_METHOD_GET 1
#define  REQUEST_METHOD_POST 2

#define STATUS_RECEIVING_HEADER  1
#define STATUS_HEADER_RECEIVED_FINISHED 2
#define STATUS_RECEIVING_BODY 3
#define STATUS_BODY_RECEIVED_FINISHED 4

#define HTTP_REQUEST_LINE_METHOD 1
#define HTTP_REQUEST_LINE_URL 2
#define HTTP_REQUEST_LINE_HTTP_VERSION 3

struct HttpHeader {
    char *name;
    char *value;
};

struct HttpQueryParam {
    char *name;
    char *value;
};

struct Client {
    struct sockaddr_in *addr; //only support ipv4
    char *ip; //client ip address
    struct HashTable *headers; //client headers
    float http_version; //http version
    char *request_url; //full request url
    int method; //request method
    int port; //request port
    char *host; //request host
    char *path; //request path
    int sock; //client  socket  file descriptor,
    unsigned int content_length;// request body length
    short status;//current client parse status
    struct ClientBuffer *buffer;//buffer that store client input
    short request_line_parse_status;//request  line parse status,
    short ssl;//ssl request
};

struct Client *ClientCreate(int sock, struct sockaddr_in *clientAddr);

#endif //STL_CLONE_CLIENT_H
