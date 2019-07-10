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
        }
    }
    return 1;
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
    printf("%s\n", client->buffer->buf);
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
    LogInfo(log, "client request url:%s\n", url);
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
                delta = i - start;
            } else {
                //last char included
                delta = i - start + 1;
            }
            //url path maybe empty,so check it
            // for example: /  or  /?a=1
            if ((delta - 1) > 0) {
                //delta  contain tail 0
                tb = MemAlloc(delta);
                if (!tb) {
                    return -1;
                } else {
                    memcpy(tb, p + start + 1, delta - 1);
                    client->path = tb;
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
                    LogInfo(log, "query name:%s  and value%s\n", param->name, param->value);
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
    char *vb = MemAlloc(4);
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
                //match finished
                memcpy(vb, BufferSubstr(buffer, start), (i - start));
                version = atof(vb);
                MemFree(vb);
                LogInfo(request->log, "http version is:%f\n", version);
                client->http_version = version;
                BufferDiscard(buffer, end + 2);//include tail \r\n
                LogInfo(request->log, "rest content；%s\n", BufferSubstr(buffer, 0));
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
                    LogInfo(request->log, "header match finished\n");
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
            if (HashAdd(client->headers, header->name, header) != 0) {
                goto failed;
            }
            matchStart = 0;//reset flag
            colonMatched = 0;
            processed += (end - start + 2);//include tail \r\n
            LogInfo(request->log, "handle one header[%s]:%s\n", header->name, header->value);
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
