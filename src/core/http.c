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
            request->client->request = request;
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
    while (buffer->size) {
        if (client->request_line_parse_status == HTTP_REQUEST_LINE_METHOD) {
            if (!HttpParseRequestMethod(request)) {
                return -1;
            }
        } else if (client->request_line_parse_status == HTTP_REQUEST_LINE_URL) {
            if (!HttpParseRequestLineUrl(request)) {
                return -1;
            }
        } else if (client->request_line_parse_status == HTTP_REQUEST_LINE_HTTP_VERSION) {
            if (!HttpParseRequestVersion(request)) {
                return -1;
            }
        } else if (client->request_line_parse_status == HTTP_REQUEST_LINE_FINISHED &&
                   client->status == STATUS_RECEIVING_HEADER) {
            //parse http headers after parsing full request line
            if (!HttpParseHeader(request)) {
                return -1;
            }
        } else if (client->status == STATUS_RECEIVED_FROM_CLIENT_FINISHED) {
            goto matchFinished;
        }
    }

    matchFinished:
    HttpParseFinished(request);
    return 1;
}

int HttpParseFinished(struct HttpRequest *request) {
    struct Client *client = request->client;
    //no longer receive data from client
    EventRemove(server.depositary, EVENT_READABLE, client->sock);
    struct Log *log = LogCreate(0, STDOUT_FILENO, NULL, LOG_LEVEL_INFO);
    char *entry = request->client->entry_file;
    size_t i = 0;
    size_t uriLen = 0;
    if (!entry) {
        uriLen = strlen(client->uri);
        i = uriLen - 1;
        while (i >= 0) {
            if (CharIsBlackSlash(client->uri[i])) {
                if (!(entry = MemAlloc(uriLen - i))) {
                    return -1;
                } else {
                    memcpy(entry, client->uri + i + 1, (uriLen - (i + 1)));
                    break;
                }
            }
            i--;
        }
    }
    if (entry && strncmp(FileExtension(entry), "php", 3) != 0) {
        return HttpRequestProcessStaticFile(request, entry);
    } else {
        struct Backend *backend = BackendCreate(request->client, NULL, log);
        if (!backend) {
            return -1;
        } else {
            return BackendExecuteCgiScript(backend);
        }
    }
}

/**
 * process static file
 * **/
int HttpRequestProcessStaticFile(struct HttpRequest *request, char *entry) {
    char *root = server.docRoot;
    struct Client *client = request->client;
    size_t rl = strlen(root);
    size_t el = strlen(entry);
    size_t tl = rl + el;
    int fd;
    short is_error = 0;
    char ext[10] = {TAIL};
    char *path = MemAlloc(tl + 2);
    struct HttpResponse *response;
    struct stat st;
    if (!path) {
        return -1;
    } else {
        memcpy(path, root, rl);
        path[rl] = CHAR_BACKSLASH;
        memcpy(path + rl + 1, entry, el);
        if (!FileExist(path)) {
            fd = FileOpen(server.error_page, FILE_OPEN_READ_ONLY, 0);
            is_error = 1;
        } else {
            fd = FileOpen(path, FILE_OPEN_READ_ONLY, 0);
        }
        if (!fd) {
            return -1;
        } else {
            fstat(fd, &st);//get file size
            memcpy(ext, FileExtension(path), 10);
            response = HttpResponseCreate(client, fd, NULL, 1,
                                          (!is_error ? MediaTypeQuery(server.config, ext) : "text/html"),
                                          (is_error ? 404 : 200), st.st_size);
            if (!response) {
                return -1;
            } else {
                if (!HttpResponseRegisterReadEvent(response, server.depositary) ||
                    !HttpResponseRegisterWriteEvent(response, server.depositary)) {
                    return -1;
                } else {
                    return 1;
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
                client->tMethod = MemAlloc(strlen(method) + 1);
                if (!client->tMethod) {
                    goto clear_error;
                } else {
                    memcpy(client->tMethod, method, strlen(method));
                }
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
                BufferDiscard(buffer, i + 1);
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

int HttpParseRequestLineUrl(struct HttpRequest *request) {
    struct Client *client = request->client;
    struct ClientBuffer *buffer = client->buffer;
    int i;
    int start, end;
    short matchStart = 0;

    for (i = 0; i < buffer->size; ++i) {
        if (CharIsSpace(BufferCharAtPos(buffer, i))) {
            if (!matchStart) {
                continue;
            } else {
                end = i;
                if ((end - start) == 0) {
                    //path is Invalid,char count>=1
                    return -1;
                }
                char *url = MemAlloc(end - start + 1);
                if (!url) {
                    return -1;
                } else {
                    memcpy(url, BufferSubstr(buffer, start), (end - start));
                }
                //match a non empty string,but url format  is not checked,so validity is uncertain
                if (!HttpParseUrl(url, (end - start), client)) {
                    //url is invalid
                    MemFree(url);
                    return -1;
                } else {
                    //url parse finished  and no error happened
                    client->request_line_parse_status = HTTP_REQUEST_LINE_HTTP_VERSION;
                    BufferDiscard(client->buffer, end);//discard from i=0
                    MemFree(url);
                    return 1;
                }
            }
        } else {
            if (!matchStart) {
                matchStart = 1;
                start = i;
            }
        }
    }
    //no enough data to parse ,so parse buffer next time
    return 1;
}

void HttpEventHandleCallback(int type, void *data) {
    struct HttpRequest *request = (struct HttpRequest *) data;
    struct Client *client = request->client;
    ReadFromSource(client->sock, client->buffer);
    if (type == EVENT_READABLE) {
        if (client->status == STATUS_RECEIVING_HEADER) {
            HttpParseRequestLine(request);
        }
    }
}

/*int HttpParseHostHeader(struct HttpRequest *request, char *header, size_t len) {
    struct Client *client = request->client;
    int matchDot = 0;
    int matchColon = 0;

    size_t pl = 0;
    int colonPos = 0;
    int port;
    int last = 0;
    int first = 0;
    char localhost[] = "localhost";

    int left = len - 1; //left character except prefix
    char *buf = MemAlloc(sizeof(char) * 6);//store port,extra 0
    const char *p = header;//exclude first  backslash
    char prev;
    int i = 0;
    if (!buf) {
        return -1;
    }
    if (0 == left) {
        //no extra character to parse,just return
        goto error;
    }
    while (i < left) {
        if (i == (left - 1)) {
            last = 1;
        } else if (i == 0) {
            first = 1;
        }
        if (p[i] == CHAR_BACKSLASH || p[i] == CHAR_HASH_TAG || p[i] == CHAR_QUESTION_MARK || p[i] == CHAR_ADDRESS) {
            //url match finished,now check validity
            if (!i) {
                //special case:domain is empty,localhost replace it
                client->host = MemAlloc(10);
                if (!client->host) {
                    return -1;
                }
                memcpy(client->host, localhost, 9);
                goto success;
            }
            //prev non empty
            if (!CharIsNumber(prev) && !CharIsAlpha(prev)) {
                goto error;
            }
            if (!matchDot) {
                if (!IsLocalhost(url)) {
                    //only localhost do not contain dot
                    goto error;
                }
            }
            if (matchColon) {
                if (!pl) {
                    //port is missing
                    goto error;
                } else {
                    if (!CheckPortValidaty((port = atoi(buf)))) {
                        goto error;
                    } else {
                        client->port = port;
                    }
                }
            }
            client->host = MemAlloc(sizeof(char) * (i + 1));//tail 0
            if (!client->host) {
                goto error;
            }
            //url passed the validation
            if (!matchColon) {
                client->port = HTTP_DEFAULT_PORT;
                memcpy(client->host, p, i);
            } else {
                memcpy(client->host, p, colonPos);
            }

            //domain and port parse finished
            //next step:parse path and params
            goto success;
            //check  if  url contain char '.'
        } else if (!DomainValidChar(p[i])) {
            goto error;
        } else {
            if (CharDot(p[i])) {
                if (last || first || (!CharIsNumber(prev) && !CharIsAlpha(prev))) {
                    goto error;
                }
                matchDot = 1;
            } else if (CharIsColon(p[i])) {
                if (last || first || (!CharIsNumber(prev) && !CharIsAlpha(prev))) {
                    goto error;
                }
                if (matchColon) {
                    //colon can only be matched once
                    goto error;
                }
                matchColon = 1;
                colonPos = i;
            } else if (CharBar(p[i])) {
                if (first || last || (!CharIsNumber(prev) && !CharIsAlpha(prev))) {
                    goto error;
                }
            } else {
                if (matchColon) {
                    if (!CharIsNumber(p[i])) {
                        //char after ':' must be numeric
                        goto error;
                    } else {
                        buf[pl++] = p[i];
                    }
                }
                if (last) {
                    //last checked
                    if (!matchDot) {
                        if (!IsLocalhost(url)) {
                            //only localhost do not contain dot
                            goto error;
                        }
                    }
                    if (matchColon) {
                        if (!CheckPortValidaty((port = atoi(buf)))) {
                            goto error;
                        } else {
                            client->port = port;
                        }
                    }

                    client->host = MemAlloc(sizeof(char) * (i + 2));//include current character
                    if (!client->host) {
                        goto error;
                    }
                    //url passed the validation
                    if (!matchColon) {
                        client->port = HTTP_DEFAULT_PORT;
                        memcpy(client->host, p, i + 1);
                    } else {
                        memcpy(client->host, p, colonPos);
                    }
                    goto success;
                } else {
                    i++;
                    prev = p[i];
                }
            }
        }
    }

    error:
    free(buf);
    return -1;
}*/

/**
 * url as follow:/order/delete?id=1
 * url do not contain  host and port
 * when  browser commit user request,has tag is stripped
 * **/
short HttpParseUrl(const char *url, size_t len, struct Client *client) {
    struct Log *log = client->request->log;
    if (!(client->request_url = MemAlloc(len + 1))) {
        return -1;
    } else {
        memcpy(client->request_url, url, len);
    }
    if (url[0] != CHAR_BACKSLASH) {
        //all request url must start with backslash
        return -1;
    }
    const char *p = url;
    //check char '/' or '#' or '?' or '&' or ':'
    int i = 0;
    char prev = 0;
    int last = 0;
    int left = len;
    if (left == 0) {
        //full url parse finished,the case: '/'
        return 1;
    }
    off_t start = 0, delta = 0;
    short parsePathFinished = 0;
    short parseQueryFinished = 0;
    short status = 0; // status value 1,2,3
    char *tb = NULL;
    char c;
    while (i < left) {
        c = p[i];
        if (i == (left - 1)) {
            last = 1;
        } else {
            last = 0;
        }
        if (i == 0 && CHAR_BACKSLASH == c) {
            //if url contain path ,so  the first char must be '/'
            status = 1;
            start = i;
        } else {
            if (i == 0) {
                //url path parse finished
                parsePathFinished = 1;
                status = 1;
            }
        }
        if (parsePathFinished && status == 1 && prev == CHAR_QUESTION_MARK) {
            //url contain query string
            status = 2;
            start = i;
        }

        if ((parsePathFinished && status == 1 && prev == CHAR_HASH_TAG) ||
            (parseQueryFinished && status == 2 && prev == CHAR_HASH_TAG)) {
            //there is  situation that url do not contain query string but contain has tag
            status = 3;
            start = i;
        }

        if (status == 1 && (CHAR_QUESTION_MARK == c || CHAR_HASH_TAG == c || last)) {
            if (CHAR_QUESTION_MARK == c || CHAR_HASH_TAG == c) {
                delta = i - start + 1;
            } else {
                //last char included
                delta = i - start + 2;
            }
            //url path maybe empty,so check it
            // for example: /  or  /?a=1
            if ((delta - 1) > 0) {
                //delta  contain tail 0
                tb = MemAlloc(delta);
                if (!tb) {
                    return -1;
                } else {
                    memcpy(tb, p + start, delta - 1);
                    client->uri = tb;
                    HttpParsePath(client->request, tb);
                }
            }
            parsePathFinished = 1;
            if (last) {
                //if last char ,just return
                return 1;
            }
        }
        if (status == 2 && (CHAR_HASH_TAG == c || last)) {
            if (CHAR_HASH_TAG == c) {
                delta = i - start;
            } else {
                delta = i - start + 1;
            }
            if (!(delta - 1)) {
                //url contain '?' but query string is empty
                parseQueryFinished = 1;
                if (last) {
                    return 1;
                }
            }

            const char *pc = p + start;
            int j = 0;
            int ks = 0;
            short keyStartMatch = 0;
            short reachLast = 0;
            short matchEqual = 0;
            int equalPos = 0;
            int kl = 0, vl = 0;
            struct HttpQueryParam *param;
            for (; j <= (delta - 1); j++) {
                c = pc[j];
                reachLast = (j == (delta - 1));
                if (c == CHAR_ADDRESS || reachLast) {
                    if (!keyStartMatch || !matchEqual) {
                        return -1;
                    }
                    keyStartMatch = 0;//reset start position
                    kl = equalPos - ks;//key length
                    if (!kl) {
                        return -1;
                    }
                    tb = MemAlloc(kl + 1);//tail 0
                    if (!tb) {
                        return -1;
                    }
                    memcpy(tb, pc + ks, kl);
                    param = MemAlloc(sizeof(struct HttpQueryParam));
                    if (!param) {
                        free(tb);
                        return -1;
                    }
                    param->name = tb;
                    if (HashAdd(client->query, tb, param) < 0) {
                        free(param);
                        free(tb);
                        return -1;
                    }
                    vl = (c == CHAR_ADDRESS) ? (j - (equalPos + 1)) : (j - (equalPos + 1) + 1);//value length
                    if (!vl) {
                        param->value = NULL;
                    } else {
                        tb = MemAlloc(vl + 1);
                        if (!tb) {
                            return -1;
                        }
                        memcpy(tb, pc + equalPos + 1, vl);
                        param->value = tb;
                    }
                    if (reachLast) {
                        break;
                    }
                    keyStartMatch = 0;
                    matchEqual = 0;

                } else if (c == CharEqual) {
                    if (!keyStartMatch || matchEqual) {
                        //match '=' more than one time
                        return -1;
                    }
                    matchEqual = 1;
                    equalPos = j;
                } else {
                    //if keyStartMatch is false ,so key match start from here
                    if (!keyStartMatch) {
                        keyStartMatch = 1;
                        ks = j;
                    }
                }
            }
            parseQueryFinished = 1;//query parse finished
        }

        if (status == 3 && last) {
            delta = i - start + 1;
            tb = MemAlloc(delta + 1);
            if (!tb) {
                return -1;
            } else {
                memcpy(tb, p + start, delta);
                client->hasTag = tb;
            }
        }
        prev = p[i];
        i++;
    }
    return 1;
}

int HttpParseRequestVersion(struct HttpRequest *request) {
    struct Client *client = request->client;
    struct ClientBuffer *buffer = client->buffer;
    int start, i, end;
    short hasMatchStart = 0;
    float version;
    char *vb = MemAlloc(4);//1.1\0
    int protocolStart;
    char *protocol;

    if (!vb) {
        return -1;
    }
    char c;
    if (buffer->size < 10) {
        //10 char is required
        //for example:HTTP/1.1\r\n
        return 1;
    } else {
        for (i = 0; i < buffer->size; ++i) {
            c = BufferCharAtPos(buffer, i);
            if (CharIsSpace(c)) {
                continue;
            } else if (CHAR_ENTER == c) {
                end = i;
            } else if (CHAR_NEW_LINE == c) {
                protocol = MemAlloc((end - protocolStart + 1));
                if (!protocol) {
                    MemFree(vb);
                    return -1;
                } else {
                    memcpy(protocol, BufferSubstr(buffer, protocolStart), end - protocolStart);
                    client->protocol_version = protocol;
                }
                //match finished
                memcpy(vb, BufferSubstr(buffer, start), (i - start));
                version = atof(vb);
                MemFree(vb);
                client->http_version = version;
                BufferDiscard(buffer, end + 2);//include tail \r\n
                client->request_line_parse_status = HTTP_REQUEST_LINE_FINISHED;//request line parse finished,next step parse headers

                return 1;
            } else {
                if (!hasMatchStart) {
                    start = i;
                    hasMatchStart = 1;
                    if (strncmp(BufferSubstr(buffer, start), "HTTP/", 5) != 0) {
                        MemFree(vb);
                        return -1;
                    } else {
                        start += 5;
                        protocolStart = i;
                    }
                }
            }
        }
        return 1;
    }
}

int HttpParseHeader(struct HttpRequest *request) {
    struct Client *client = request->client;
    struct ClientBuffer *buffer = client->buffer;
    char c;
    int start, end;
    int i;
    short matchStart = 0;
    int colonPos;
    int kl, vl;
    short last = 0;
    short colonMatched = 0;
    char *ptr;
    size_t processed = 0;//processed how many bytes  so far
    if (!buffer->size) {
        return 1;
    }
    struct HttpHeader *header;
    for (i = 0; i < buffer->size; ++i) {
        c = BufferCharAtPos(buffer, i);
        last = (i == (buffer->size - 1));
        if (!matchStart) {
            if (CharIsEnter(c)) {
                if (!last) {
                    //header match finished
                    if (client->method == HTTP_METHOD_GET) {
                        client->status = STATUS_RECEIVED_FROM_CLIENT_FINISHED;//no body
                    } else {
                        client->status = STATUS_RECEIVING_BODY;//post request
                    }
                    BufferDiscard(buffer, processed + 2);
                } else {
                    // parse status has no change
                    // last '\r' is reserved for later parse
                    BufferDiscard(buffer, processed);
                }
                return 1;
            }
            matchStart = 1;
            start = i;
        } else if (CharIsColon(c)) {
            if (!colonMatched) {
                colonMatched = 1;
                colonPos = i;
            }
        } else if (c == CHAR_ENTER) {
            end = i;
        } else if (c == CHAR_NEW_LINE) {
            //match one line
            if (!(header = MemAlloc(sizeof(struct HttpHeader)))) {
                goto failed;
            }
            kl = colonPos - start + 1;//tail 0
            vl = end - colonPos;//tail 0

            ptr = trim(BufferSubstr(buffer, start), kl, CHARSPACE);
            if (!ptr) {
                goto failed;
            }
            header->name = ptr;
            ptr = trim(BufferSubstr(buffer, colonPos + 1), vl, CHARSPACE);
            if (!ptr) {
                goto failed;
            }
            header->value = ptr;
            if (!HttpParseSpecifedHeader(request, header->name, header->value)) {
                goto failed;
            }
            if (HashAdd(client->headers, header->name, header) != 0) {
                goto failed;
            }
            matchStart = 0;//reset flag
            colonMatched = 0;
            processed += (end - start + 2);//include tail \r\n
        }
    }
    //the case: last \r\n not match,so code run to here
    BufferDiscard(buffer, processed);
    return 1;

    failed:
    BufferDiscard(buffer, processed);//modify buffer offset
    if (header) {
        if (header->name) {
            MemFree(header->name);
        }
        if (header->value) {
            MemFree(header->value);
        }
        MemFree(header);
        return -1;
    }
}

int HttpParseSpecifedHeader(struct HttpRequest *request, char *name, char *value) {
    name = strlwr(name);
    size_t len = strlen(name);
    if (strncasecmp(name, "host", 4) == 0) {
        HttpParseHeaderHost(request, value);
    } else if (strncmp(name, "content-type", 12) == 0) {
        HttpParseContentType(request, value);
    }
}

/**
 * Host format like: "Host: www.baidu.com:8081"
 * **/
int HttpParseHeaderHost(struct HttpRequest *request, char *value) {
    struct Client *client = request->client;
    size_t len = strlen(value);
    int i;
    char c;
    int colonPos;
    short matchColon = 0;
    char *host;
    char *port;
    size_t kl;
    for (i = 0; i < len; ++i) {
        c = value[i];
        if (CharIsColon(c)) {
            matchColon = 1;
            colonPos = i;
        }
    }
    if (matchColon) {
        //port is specified
        kl = colonPos + 1;
    } else {
        kl = len + 1;
    }
    host = MemAlloc(kl);//tail 0
    if (!host) {
        return -1;
    } else {
        memcpy(host, value, kl - 1);
    }
    if (matchColon) {
        kl = len - (colonPos + 1) + 2;
    } else {
        kl = 3;//80\0
    }

    port = MemAlloc(kl);
    if (!port) {
        MemFree(host);
        return -1;
    } else {
        if (matchColon) {
            memcpy(port, value + colonPos + 1, kl - 1);
        } else {
            memcpy(port, "80", kl - 1);
        }
    }
    client->host = host;
    client->port = port;

    return 1;

}


/**
 * debug/index.php/order/deleted/1?a=b&b=2
 * order/delete/1
 * **/
int HttpParsePath(struct HttpRequest *request, char *path) {
    //path of client is not correct,so we parse it now
    //check extension name .php
    size_t len = strlen(path);
    int i;
    int dotPos;
    short dotMatched = 0;
    int lastSlashPos;//last match baclskash
    char c;
    char *entry;
    char *script_name;
    char *path_info;
    size_t el;
    for (i = 0; i < len; i++) {
        c = path[i];
        if (CharDot(c)) {
            dotMatched = 1;
            dotPos = i;
            if (strncmp(path + dotPos + 1, server.cgiExtName, strlen(server.cgiExtName)) != 0) {
                return -1;
            }
            //extract  entry filename
            el = i - (lastSlashPos + 1) + strlen(server.cgiExtName) + 1;//include '.',but not contain tail '\0'
            entry = MemAlloc(el + 1);//include tail 0
            if (!entry) {
                goto failed;
            } else {
                memcpy(entry, path + lastSlashPos + 1, el);
                request->client->entry_file = entry;
            }
            //extract script_name
            el = i + 4;
            script_name = MemAlloc(el + 1);
            if (!script_name) {
                goto failed;
            } else {
                memcpy(script_name, path, el);
                request->client->script_name = script_name;
            }
            //extract path_info
            el = len - (i + strlen(server.cgiExtName) + 1);
            path_info = MemAlloc(el + 1);
            if (!path_info) {
                goto failed;
            } else {
                memcpy(path_info, path + i + strlen(server.cgiExtName) + 1, el);
                request->client->path_info = path_info;
            }
            //parse finished and return
            return 1;

        } else if (CHAR_BACKSLASH == c) {
            lastSlashPos = i;
        }
    }

    //code run here,so explain that no dot matched,then entry file is not specified,we regard default file as entry file
    //but we do not the task here
    request->client->path_info = MemAlloc(len + 1);
    if (!request->client->path_info) {
        goto failed;
    } else {
        memcpy(request->client->path_info, path, len);
        return 1;
    }


    failed:
    if (entry) {
        MemFree(entry);
    }
    if (script_name) {
        MemFree(script_name);
    }
    if (path_info) {
        MemFree(path_info);
    }
    return -1;
}

int HttpParseContentType(struct HttpRequest *request, char *value) {
    struct Client *client = request->client;
    size_t len = strlen(value);
    int i;
    char c;
    size_t delta;//include tail 0
    char *buf;
    for (i = 0; i < len; i++) {
        c = value[i];
        if (CharIsSemicolon(c) || (i == (len - 1))) {
            if (CharIsSemicolon(c)) {
                delta = i + 1;
            } else {
                delta = i + 2;
            }
            if (!(buf = MemAlloc(delta))) {
                return -1;
            } else {
                memcpy(buf, value, delta - 1);
                client->content_type = buf;
                return 1;
            }
        }
    }
    //content-type format is invalid
    return -1;
}