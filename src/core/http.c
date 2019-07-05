//
// Created by Administrator on 2019/7/5 0005.
//

#include "http.h"

struct HttpRequest *HttpRequestCreate(int socket, struct sockaddr_in *addr) {
    struct HttpRequest *request = MemAlloc(sizeof(struct HttpRequest));
    if (!request) {
        return NULL;
    } else {
        request->client = ClientCreate(socket, addr);
        if (!request->client) {
            free(request);
            return NULL;
        } else {
            return request;
        }
    }
}

/**
 * parse request  url
 * **/
int HttpParseRequestLine(struct HttpRequest *request) {
    struct Client *client = request->client;
    struct ClientBuffer *buffer = client->buffer;
    int i;
    char *url;
    int start, end;
    short matchStart = 0;
    short prevIsSpace = 0;
    if (client->request_line_parse_status == HTTP_REQUEST_LINE_METHOD) {
        return HttpParseRequestMethod(request);
    } else if (client->request_line_parse_status == HTTP_REQUEST_LINE_URL) {
        url = MemAlloc(sizeof(char) * MAX_REQUEST_URL_LENGTH);
        if (!url) {
            return -1;
        } else {
            for (i = 0; i < buffer->size; ++i) {
                if (CharIsSpace(BufferCharAtPos(buffer, i))) {
                    if (!matchStart) {
                        continue;
                    } else {
                        end = i;
                        //match a non empty string,but url format  is not checked,so validity is uncertain
                        if (!HttpUrlCheckValidity(BufferSubstr(buffer, start), (end - start))) {
                            //url is invalid
                            return -1;
                        } else {

                        }
                    }
                } else {
                    if (!matchStart) {
                        matchStart = i;
                        start = i;
                    }
                }
            }
        }
    }
}

int HttpParseRequestMethod(struct HttpRequest *request) {
    struct Client *client = request->client;
    struct ClientBuffer *buffer = client->buffer;
    if (client->request_line_parse_status != HTTP_REQUEST_LINE_METHOD) {
        return 1;
    }
    int i = 0;
    char c;
    char *method = MemAlloc(sizeof(char) * 10);
    if (!method) {
        return -1;
    }
    //suffix \r\n
    for (i = 0; i < buffer->size; i++) {
        c = BufferCharAtPos(buffer, i);
        if (CharIsValid(c)) {
            method[i] = c;
        } else if (CharIsSpace(c)) {
            if (!strlen(method)) {
                goto clear_error;
            } else {
                char *sl = strlwr(method);
                if (strncmp(sl, HTTP_METHOD_GET_LABEL, i) == 0) {
                    client->method = HTTP_METHOD_GET;
                } else if (strncmp(sl, HTTP_METHOD_POST_LABEL, i) == 0) {
                    client->method = HTTP_METHOD_POST;
                } else if (strncmp(sl, HTTP_METHOD_HEAD_LABEL, i) == 0) {
                    client->method = HTTP_METHOD_HEAD;
                } else if (strncmp(sl, HTTP_METHOD_PUT_LABEL, i) == 0) {
                    client->method = HTTP_METHOD_PUT;
                } else if (strncmp(sl, HTTP_METHOD_DELETE_LABEL, i) == 0) {
                    client->method = HTTP_METHOD_DELETE;
                } else if (strncmp(sl, HTTP_METHOD_OPTIONS_LABEL, i) == 0) {
                    client->method = HTTP_METHOD_OPTIONS;
                } else if (strncmp(sl, HTTP_METHOD_TRACE_LABEL, i) == 0) {
                    client->method = HTTP_METHOD_TRACE;
                } else if (strncmp(sl, HTTP_METHOD_PATCH_LABEL, i) == 0) {
                    client->method = HTTP_METHOD_PATCH;
                } else {
                    goto clear_error;
                }
                BufferDiscard(buffer, i);
                client->request_line_parse_status = HTTP_REQUEST_LINE_URL;
                return 1;
            }
        } else {
            goto clear_error;
        }
    }

    //no enough bytes to parse,so parse next time
    free(method);
    return 1;
    clear_error:
    free(method);
    return -1;
}

void HttpEventHandleCallback(int type, void *data) {
    struct HttpRequest *request = (struct HttpRequest *) data;
    struct Client *client = request->client;
    if (type == EVENT_READABLE) {
        if (client->status == STATUS_RECEIVING_HEADER) {
            HttpParseRequestLine(request);
        }
    }
}

/**
 * url as follow: http://xxx.com:81/order/delete?id=1#uncertain
 * **/
short HttpUrlCheckValidity(const char *url, size_t len) {
    if (len <= 8) {
        return -1;
    }
    if (strncmp(url, "http://", 7) != 0 || strncmp(url, "https://", 8) != 0) {
        return -1;
    }
    const char *p = strncmp(url, "http://", 7) == 0 ? (url + 7) : (url + 8);
    //check char '/' or '#' or '?' or '&'
    int i = 0;
    while (i < len) {

        i++;
    }
}

#include "http.h"
