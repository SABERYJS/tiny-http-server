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
        for (i = 0; i < buffer->size; ++i) {
            if (CharIsSpace(BufferCharAtPos(buffer, i))) {
                if (!matchStart) {
                    continue;
                } else {
                    end = i;
                    //match a non empty string,but url format  is not checked,so validity is uncertain
                    if (!HttpParseUrl(BufferSubstr(buffer, start), (end - start), client)) {
                        //url is invalid
                        return -1;
                    } else {

                    }
                }
            } else {
                if (!matchStart) {
                    matchStart = 1;
                    start = i;
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
short HttpParseUrl(const char *url, size_t len, struct Client *client) {
    if (len <= 8) {
        return -1;
    }
    //http url  must start with https:// or http://
    int matchHttp = strncmp(url, "http://", 7) == 0;
    if (!matchHttp) {
        if (strncmp(url, "https://", 8) != 0) {
            return -1;
        }
    }

    client->ssl = !matchHttp ? 1 : 0;
    int off = matchHttp ? 7 : 8;
    const char *p = url + off;
    //check char '/' or '#' or '?' or '&' or ':'
    int i = 0;
    int matchDot = 0;
    int matchColon = 0;
    char prev = 0;
    size_t pl = 0;
    int colonPos = 0;
    int port;
    int last = 0;
    int first = 0;
    char localhost[] = "localhost";
    int left = len - off; //left character except prefix
    char *buf = MemAlloc(sizeof(char) * 6);//store port,extra 0
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
    success:
    p += (i + 1);
    left -= (i + 1);
    if (left == 0) {
        //full url parse finished
        return 1;
    }
    i = 0;
    off_t start = 0, delta = 0;
    short parsePathFinished = 0;
    short parseQueryFinished = 0;
    short status = 0; // status value 1,2,3
    char *tb;
    while (i < left) {
        if (i == (left - 1)) {
            last = 1;
        } else {
            last = 0;
        }
        if (i == 0 && CHAR_BACKSLASH == p[i]) {
            //if url contain path ,so  the first char must be '/'
            status = 1;
            start = i;
        } else {
            //url path parse finished
            parsePathFinished = 1;
            status = 1;
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

        if (status == 1 && (CHAR_QUESTION_MARK == p[i] || CHAR_HASH_TAG == p[i] || last)) {
            if (CHAR_QUESTION_MARK == p[i] || CHAR_HASH_TAG == p[i]) {
                delta = i - start;
            } else {
                //last char included
                delta = i - start + 1;
            }
            //url path maybe empty,so check it
            if ((delta - 1) > 0) {
                //delta  contain tail 0
                tb = MemAlloc(sizeof(char) * delta);
                if (!tb) {
                    return -1;
                } else {
                    memcpy(tb, p + start + 1, delta - 1);
                    client->path = tb;
                }
            }
            parsePathFinished = 1;
            if (last) {
                return 1;
            }
        }
        if (status == 2 && (CHAR_HASH_TAG == p[i] || last)) {
            if (CHAR_HASH_TAG == p[i]) {
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

            char *pc = p + start;
            int j = 0;
            int ks = 0;
            char c;
            short keyStartMatch = 0;
            short reachLast = 0;
            short matchEqual = 0;
            int equalPos = 0;
            char *tb = NULL;
            int kl = 0, vl = 0;
            struct HttpQueryParam *param;
            for (; j < (delta - 1); j++) {
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
                    if (!HashAdd(client->headers, tb, param)) {
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
                        param = NULL; //reset param
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

        i++;
        prev = p[i];
    }
    return 1;
}
