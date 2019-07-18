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

#include "http_response.h"

/**
 * called   before cgi script was executed
 * **/
struct HttpResponse *HttpResponseCreate(struct Client *client, int readFd, struct Log *log) {
    struct HttpResponse *response = MemAlloc(sizeof(struct HttpResponse));
    int flag = 0;
    if (!response) {
        return NULL;
    } else {
        if (!log) {
            log = LogCreate(0, STDOUT_FILENO, NULL, LOG_LEVEL_INFO);
            flag = 1;
            if (!log) {
                return NULL;
            }
        }
        response->output = OutputBufferCreate(readFd, client->sock, log);
        response->log = log;
        if (!response->output) {
            if (flag) {
                MemFree(log);
            }
            MemFree(response);
            return NULL;
        } else {
            response->client = client;
            client->response = response;
            response->status = HTTP_RESPONSE_STATUS_NORMAL;
            return response;
        }
    }
}

/**
 * handle server internal error
 * **/
void HttpResponseHandleInternalError(struct HttpResponse *response) {
    if (response->status == HTTP_RESPONSE_STATUS_ERROR) {
        //server internal error happened

    }
}

/**
 * register readable and writable event of current HttpResponse object
 * **/
int HttpResponseRegisterEvent(struct HttpResponse *response, struct EventDepositary *depositary) {
    //firstly register readable event
    if (!EventAdd(depositary, EVENT_READABLE, response->output->providerFd, response,
                  HttpResponseReadableEventCallback)) {
        return -1;
    }
    if (!EventAdd(depositary, EVENT_WRITEABLE, response->output->targetFd, response,
                  HttpResponseWritableEventCallback)) {
        return -1;
    }
    return 1;
}

/**
 * called by event system when cgi script  wrote some data to stdout
 * **/
void HttpResponseReadableEventCallback(int type, void *data) {
    struct HttpResponse *response = (struct HttpResponse *) data;
    if (response->status == HTTP_RESPONSE_STATUS_ERROR) {
        //for safety reason
        return;
    }
    if (OutputBufferCanRead(response->output) < 0) {
        response->status = HTTP_RESPONSE_STATUS_ERROR;
        return;
    }
}

/**
 * called by event system when client socket can write
 * **/
void HttpResponseWritableEventCallback(int type, void *data) {
    struct HttpResponse *response = (struct HttpResponse *) data;
    if (response->status == HTTP_RESPONSE_STATUS_ERROR) {
        //for safety reason
        return;
    }
    if (OutputBufferCanWrite(response->output) < 0) {
        response->status = HTTP_RESPONSE_STATUS_ERROR;
        return;
    }
}

void HttpResponseRemoveReadableEvent(struct HttpResponse *response, struct EventDepositary *depositary) {
    if (!EventRemove(depositary, EVENT_READABLE, response->output->providerFd)) {
        if (response->status != HTTP_RESPONSE_STATUS_ERROR) {
            response->status = HTTP_RESPONSE_STATUS_ERROR;
        }
    }
}


void HttpResponseRemoveWritableEvent(struct HttpResponse *response, struct EventDepositary *depositary) {
    if (!EventRemove(depositary, EVENT_WRITEABLE, response->output->targetFd)) {
        if (response->status != HTTP_RESPONSE_STATUS_ERROR) {
            response->status = HTTP_RESPONSE_STATUS_ERROR;
        }
    }
}

int HttpResponseRegisterReadEvent(struct HttpResponse *response, struct EventDepositary *depositary) {
    if (!EventAdd(depositary, EVENT_READABLE, response->output->providerFd, response,
                  HttpResponseReadableEventCallback)) {
        return -1;
    } else {
        return 1;
    }
}

int HttpResponseRegisterWriteEvent(struct HttpResponse *response, struct EventDepositary *depositary) {
    if (!EventAdd(depositary, EVENT_WRITEABLE, response->output->targetFd, response,
                  HttpResponseWritableEventCallback)) {
        return -1;
    }
    return 1;
}


int HttpResponseWriteStatusLine(struct HttpResponse *response, int code) {
    struct Client *client = response->client;
    char *overview;
    switch (code) {
        case HTTP_STATUS_CODE_CONTINUE:
            overview = HTTP_STATUS_CODE_OVERVIEW_CONTINUE;
            break;
        case HTTP_STATUS_CODE_SWITCH_PROTOCOL:
            overview = HTTP_STATUS_CODE_OVERVIEW_SWITCH_PROTOCOL;
            break;
        case HTTP_STATUS_CODE_OK:
            overview = HTTP_STATUS_CODE_OVERVIEW_OK;
            break;
        case HTTP_STATUS_CODE_CREATED:
            overview = HTTP_STATUS_CODE_OVERVIEW_CREATED;
            break;
        case HTTP_STATUS_CODE_ACCEPTED:
            overview = HTTP_STATUS_CODE_OVERVIEW_ACCEPTED;
            break;
        case HTTP_STATUS_CODE_NO_AUTHORITATIVE_INFORMATION:
            overview = HTTP_STATUS_CODE_OVERVIEW_NO_AUTHORITATIVE_INFORMATION;
            break;
        case HTTP_STATUS_CODE_NO_CONTENT:
            overview = HTTP_STATUS_CODE_OVERVIEW_NO_CONTENT;
            break;
        case HTTP_STATUS_CODE_RESET_CONTENT:
            overview = HTTP_STATUS_CODE_OVERVIEW_RESET_CONTENT;
            break;
        case HTTP_STATUS_CODE_PARTIAL_CONTENT:
            overview = HTTP_STATUS_CODE_OVERVIEW_PARTIAL_CONTENT;
            break;
        case HTTP_STATUS_CODE_MULTI_CHOICES:
            overview = HTTP_STATUS_CODE_OVERVIEW_MULTI_CHOICES;
            break;
        case HTTP_STATUS_CODE_MOVED_PERMANENTLY:
            overview = HTTP_STATUS_CODE_OVERVIEW_MOVED_PERMANENTLY;
            break;
        case HTTP_STATUS_CODE_FOUND:
            overview = HTTP_STATUS_CODE_OVERVIEW_FOUND;
            break;
        case HTTP_STATUS_CODE_SEE_OTHER:
            overview = HTTP_STATUS_CODE_OVERVIEW_SEE_OTHER;
            break;
        case HTTP_STATUS_CODE_NOT_MODIFIED:
            overview = HTTP_STATUS_CODE_OVERVIEW_NOT_MODIFIED;
            break;
        case HTTP_STATUS_CODE_USE_PROXY:
            overview = HTTP_STATUS_CODE_OVERVIEW_USE_PROXY;
            break;
        case HTTP_STATUS_CODE_TEMPORARY_REDIRECT:
            overview = HTTP_STATUS_CODE_OVERVIEW_TEMPORARY_REDIRECT;
            break;
        case HTTP_STATUS_CODE_BAD_REQUEST:
            overview = HTTP_STATUS_CODE_OVERVIEW_BAD_REQUEST;
            break;
        case HTTP_STATUS_CODE_UNAUTHORIZED:
            overview = HTTP_STATUS_CODE_OVERVIEW_UNAUTHORIZED;
            break;
        case HTTP_STATUS_CODE_PAYMENT_REQUIRED:
            overview = HTTP_STATUS_CODE_OVERVIEW_PAYMENT_REQUIRED;
            break;
        case HTTP_STATUS_CODE_FORBIDDEN:
            overview = HTTP_STATUS_CODE_OVERVIEW_FORBIDDEN;
            break;
        case HTTP_STATUS_CODE_NOT_FOUND:
            overview = HTTP_STATUS_CODE_OVERVIEW_NOT_FOUND;
            break;
        case HTTP_STATUS_CODE_METHOD_NOT_ALLOWED:
            overview = HTTP_STATUS_CODE_OVERVIEW_METHOD_NOT_ALLOWED;
            break;
        case HTTP_STATUS_CODE_NOT_ACCEPTABLE:
            overview = HTTP_STATUS_CODE_OVERVIEW_NOT_ACCEPTABLE;
            break;
        case HTTP_STATUS_CODE_PROXY_AUTHENTICATION_REQUIRED:
            overview = HTTP_STATUS_CODE_OVERVIEW_PROXY_AUTHENTICATION_REQUIRED;
            break;
        case HTTP_STATUS_CODE_REQUEST_TIMEOUT:
            overview = HTTP_STATUS_CODE_OVERVIEW_REQUEST_TIMEOUT;
            break;
        case HTTP_STATUS_CODE_CONFLICT:
            overview = HTTP_STATUS_CODE_OVERVIEW_CONFLICT;
            break;
        case HTTP_STATUS_CODE_GONE:
            overview = HTTP_STATUS_CODE_OVERVIEW_GONE;
            break;
        case HTTP_STATUS_CODE_LENGTH_REQUIRED:
            overview = HTTP_STATUS_CODE_OVERVIEW_LENGTH_REQUIRED;
            break;
        case HTTP_STATUS_CODE_PRECONDITION_FAILED:
            overview = HTTP_STATUS_CODE_OVERVIEW_PRECONDITION_FAILED;
            break;
        case HTTP_STATUS_CODE_REQUEST_ENTITY_TOO_LARGE:
            overview = HTTP_STATUS_CODE_OVERVIEW_REQUEST_ENTITY_TOO_LARGE;
            break;
        case HTTP_STATUS_CODE_REQUEST_URL_TOO_LONG:
            overview = HTTP_STATUS_CODE_OVERVIEW_REQUEST_URL_TOO_LONG;
            break;
        case HTTP_STATUS_CODE_UNSUPPORTED_MEDIA_TYPE:
            overview = HTTP_STATUS_CODE_OVERVIEW_UNSUPPORTED_MEDIA_TYPE;
            break;
        case HTTP_STATUS_CODE_REQUEST_RANGE_NOT_SATISFIABLE:
            overview = HTTP_STATUS_CODE_OVERVIEW_REQUEST_RANGE_NOT_SATISFIABLE;
            break;
        case HTTP_STATUS_CODE_EXPECTATION_FAILED:
            overview = HTTP_STATUS_CODE_OVERVIEW_EXPECTATION_FAILED;
            break;
        case HTTP_STATUS_CODE_INTERNAL_SERVER_ERROR:
            overview = HTTP_STATUS_CODE_OVERVIEW_INTERNAL_SERVER_ERROR;
            break;
        case HTTP_STATUS_CODE_BAD_GATEWAY:
            overview = HTTP_STATUS_CODE_OVERVIEW_BAD_GATEWAY;
            break;
        case HTTP_STATUS_CODE_HTTP_VERSION_NOT_SUPPORTED:
            overview = HTTP_STATUS_CODE_OVERVIEW_HTTP_VERSION_UNSUPPORT;
            break;
        default:
            return -1;
    }

    size_t protocolLen = strlen(client->protocol_version);
    char codeBuf[4];
    struct ClientBuffer *buffer = response->output->buffer;
    itoa(code, codeBuf, 4);
    size_t overLen = strlen(overview);
    WriteToBufferManual(buffer, client->protocol_version, protocolLen);
    WriteOneByteToBufferManual(buffer, CHARSPACE);
    WriteToBufferManual(buffer, codeBuf, 3);
    WriteOneByteToBufferManual(buffer, CHARSPACE);
    WriteToBufferManual(buffer, overview, overLen);
    WriteToBufferManual(buffer, RESPONSE_LINE_TAIL, 2);
    return 1;
}

int HttpResponseWriteHeader(struct HttpResponse *response, char *header, char *value) {
    struct ClientBuffer *buffer = response->output->buffer;
    WriteToBufferManual(buffer, header, strlen(header));
    WriteToBufferManual(buffer, ": ", 2);
    WriteToBufferManual(buffer, value, strlen(value));
    WriteToBufferManual(buffer, RESPONSE_LINE_TAIL, 2);
    return 1;
}

/**
 * write last  response header
 * **/
int HttpResponseWriteLastHeader(struct HttpResponse *response, char *header, char *value) {
    HttpResponseWriteHeader(response, header, value);
    WriteToBufferManual(response->output->buffer, RESPONSE_LINE_TAIL, 2);
    return 1;
}
