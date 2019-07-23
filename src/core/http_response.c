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
 * called   before CGI script was executed
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
            if (!(response->headers = createList(HttpResponseHeaderCompare, NULL))) {
                if (flag) {
                    MemFree(log);
                }
                MemFree(response->output);
                MemFree(response);
                return NULL;
            }
            response->client = client;
            client->response = response;
            response->status = HTTP_RESPONSE_STATUS_NORMAL;
            response->pipe_closed = 0;
            response->client_closed = 0;
            response->status_line_sent = 0;
            response->header_parse_finished = 0;
            response->status_parsed = 0;
            response->response_status_code = 0;
            response->content_type_checked = 0;
            memset(response->header_buffer, 0, HTTP_RESPONSE_HEADER_BUFFER_SIZE);
            return response;
        }
    }
}


static int HttpResponseHeaderCompare(struct ListNode *node, void *data) {
    struct HttpHeader *hd1 = (struct HttpHeader *) node->data;
    struct HttpHeader *hd2 = (struct HttpHeader *) data;
    return strcmp(hd1->name, hd2->name);
}


static int HttpResponseParseHeader(struct HttpResponse *response) {
    struct ClientBuffer *buffer = response->output->buffer;
    struct Log *log = response->log;
    int i = 0;
    char c = 0;
    short colonMatched = 0;
    int colonPosition = 0;
    char *ptr = NULL;
    size_t len = 0;
    short status_code_parsed = 0;
    char *header_name = NULL, *header_value = NULL;
    int start = 0;
    short startMatched = 0;
    struct HttpHeader *header = NULL;
    int processed = 0;
    short lastReached = 0;
    char *html_header = "text/html";
    short shouldAsHeader = 1;
    int header_name_len = 0, header_value_len = 0;
    //return directly,if there is no data ro read
    if (buffer->size <= 0) {
        return 1;
    }
    if (response->header_parse_finished) {
        //header has been parse finished
        return 1;
    } else {
        for (; i < buffer->size; i++) {
            c = BufferCharAtPos(buffer, i);
            lastReached = (i == (buffer->size - 1));
            if (!CharIsEnter(c) && !CharIsColon(c)) {
                //we trust data from CGI script,so data checking is unnecessary
                if (!startMatched) {
                    startMatched = 1;
                    start = i;
                }
                continue;
            } else if (CharIsColon(c)) {
                colonMatched = 1;
                colonPosition = i;
            } else {
                if (!startMatched) {
                    //case is: continuously '\r\n' detected
                    if (lastReached) {
                        //missing character '\n'
                        goto finished;
                    } else {
                        processed += 2;//header last two \r\n
                        //http response header parse finished
                        response->header_parse_finished = 1;
                        if (!response->content_type_checked) {
                            //CGI did not return Content Type,so we provide default text/html media type
                            header = MemAlloc(sizeof(struct HttpHeader));
                            if (!header) {
                                return -1;
                            } else {
                                header_name_len = strlen(HTTP_HEADER_CONTENT_TYPE(1));
                                header_value_len = strlen(html_header);
                                if (!(header->name = MemAlloc(header_name_len + 1)) ||
                                    !(header->value = MemAlloc(header_value_len + 1))) {
                                    return -1;
                                } else {
                                    memcpy(header->name, HTTP_HEADER_CONTENT_TYPE(1), header_name_len);
                                    memcpy(header->value, html_header, header_value_len);
                                    LogInfo(log, "app default content-type provided:%s\n", html_header);
                                }
                            }
                        }
                        goto finished;
                    }
                }
                if (lastReached) {
                    //we enter here because  char c is ENTER,but we also want to match '\n'
                    goto finished;
                } else {
                    if (!response->status_parsed) {
                        if (colonMatched) {
                            //check Status header
                            if (strncmp("Status", BufferSubstr(buffer, 0), 6) == 0) {
                                //parse status code
                                ptr = trim(BufferSubstr(buffer, colonPosition + 1), (i - colonPosition - 1),
                                           CHARSPACE);
                                if (!ptr) {
                                    return -1;
                                }
                                response->response_status_code = atoi(ptr);
                                LogInfo(log, "Parse Status from CGI Response:%d\n", response->response_status_code);
                                status_code_parsed = 1;
                                shouldAsHeader = 0;
                            }
                        } else {
                            //whether CGI script return http status line is uncertain
                            len = strlen(response->client->protocol_version);
                            if (strncmp(response->client->protocol_version, BufferSubstr(buffer, 0), len) == 0) {
                                //first line is status line
                                response->response_status_code = atoi(BufferSubstr(buffer, len));
                                status_code_parsed = 1;
                                LogInfo(log, "Parse response code from status line:%d\n",
                                        response->response_status_code);
                                shouldAsHeader = 0;
                            }
                        }
                        //status code parse finished ,but it maybe 0,so we provide status code 200
                        response->status_parsed = 1;
                        if (!status_code_parsed) {
                            response->response_status_code = 200;
                            LogInfo(log, "because no status code return,so default 200 replaced\n");
                        }
                    }
                    if (shouldAsHeader) {
                        header_name = MemAlloc((colonPosition - start) + 1);
                        header_value = MemAlloc((i - colonPosition));
                        header = MemAlloc(sizeof(struct HttpHeader));
                        if (!header_name || !header_value | !header) {
                            return -1;
                        }
                        memcpy(header_name, BufferSubstr(buffer, start), (colonPosition - start));
                        memcpy(header_value, BufferSubstr(buffer, colonPosition + 1), (i - colonPosition - 1));
                        header->name = header_name;
                        header->value = header_value;
                        if (appendNode(response->headers, header) < 0) {
                            return -1;
                        } else {
                            if (ContentTypeDetect(header_name)) {
                                response->content_type_checked = 1;
                            }
                            LogInfo(log, "CGI response header[%s]:%s\n", header->name, header->value);
                        }
                    }
                    processed += (i - start + 2);//include character '\n'
                    startMatched = 0;
                    colonMatched = 0;
                    shouldAsHeader = 1;
                    i++;//jump after character '\n'
                }
            }
        }
    }

    finished:
    BufferDiscard(buffer, processed);
    return 1;
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
 * called by event system when CGI script  wrote some data to stdout
 * **/
void HttpResponseReadableEventCallback(int type, void *data) {
    struct HttpResponse *response = (struct HttpResponse *) data;
    struct OutputBuffer *buffer = response->output;
    if (response->pipe_closed > 0) {
        //maybe it is possible,but who know
        return;
    }
    int ret;
    if (response->status == HTTP_RESPONSE_STATUS_ERROR) {
        //for safety reason
        return;
    }
    if ((ret = OutputBufferCanRead(response->output)) < 0) {
        response->status = HTTP_RESPONSE_STATUS_ERROR;
        return;
    } else {
        if (ret == 0) {
            //can not read more data,so close related pipe
            close(response->output->providerFd);
            //pipe closed
            response->pipe_closed = 1;
            //remove pipe readable event
            EventRemove(server.depositary, EVENT_READABLE, response->output->providerFd);
            EventRemove(server.depositary, EVENT_WRITEABLE, response->output->providerFd);
            if (response->output->buffer->size <= 0) {
                //close client connection
                close(buffer->targetFd);
                //remove readable and writeable event
                EventRemove(server.depositary, EVENT_READABLE, buffer->targetFd);
                EventRemove(server.depositary, EVENT_WRITEABLE, buffer->targetFd);
            }
        } else {
            //we need to parse CGI return headers,for example:Content-Type or Http Response Status line missing
            HttpResponseParseHeader(response);
        }
    }
}

/**
 * called by event system when client socket can write
 * **/
void HttpResponseWritableEventCallback(int type, void *data) {
    struct HttpResponse *response = (struct HttpResponse *) data;
    struct OutputBuffer *buffer = response->output;
    struct List *headers = response->headers;
    struct HttpHeader *header = NULL;
    struct ListNode *node = NULL;
    int ret;
    if (response->status == HTTP_RESPONSE_STATUS_ERROR) {
        //for safety reason
        return;
    }
    if (response->output->buffer->size <= 0) {
        return;
    }
    if (!response->header_parse_finished) {
        return;
    } else {
        //http header parse finished
        node = headers->header;
        while (node) {
            header = (struct HttpHeader *) node->data;
            if (node == headers->tail) {
                HttpResponseWriteLastHeader(response, header->name, header->value);
            } else {
                HttpResponseWriteHeader(response, header->name, header->value);
            }
            node = node->next;
        }
    }
    //send status line

    if ((ret = OutputBufferCanWrite(response->output)) < 0) {
        response->status = HTTP_RESPONSE_STATUS_ERROR;
        return;
    } else {
        //no  more data to write and pipe connected to CGI was closed
        if (buffer->buffer->size <= 0 && response->pipe_closed) {
            //close client connection
            close(buffer->targetFd);
            //remove readable and writeable event
            EventRemove(server.depositary, EVENT_READABLE, buffer->targetFd);
            EventRemove(server.depositary, EVENT_WRITEABLE, buffer->targetFd);
        }
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
    char codeBuf[4] = {TAIL};
    struct ClientBuffer *buffer = response->output->buffer;
    //itoa(code, codeBuf, 4);
    sprintf(codeBuf, "%d", code);
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
