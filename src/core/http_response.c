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
