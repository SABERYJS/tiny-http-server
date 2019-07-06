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
                        if (!HttpUrlCheckValidity(BufferSubstr(buffer, start), (end - start), client)) {
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
short HttpUrlCheckValidity(const char *url, size_t len, struct Client *client) {
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
    int left = len - off; //left character except prefix
    char *buf = MemAlloc(sizeof(char) * 6);//store port,extra 0
    if (!buf) {
        return -1;
    }
    if (0 == left) {
        //no extra character to parse,just return
        return -1;
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
                //url length is 0
                return -1;
            }
            //prev non empty
            if (!CharIsNumber(prev) && !CharIsAlpha(prev)) {
                return -1;
            }
            if (!matchDot) {
                if (!IsLocalhost(url)) {
                    //only localhost do not contain dot
                    return -1;
                }
            }
            if (matchColon) {
                if (!pl) {
                    //port is missing
                    return -1;
                } else {
                    if (!CheckPortValidaty((port = atoi(buf)))) {
                        return -1;
                    } else {
                        client->port = port;
                    }
                }
            }
            client->host = MemAlloc(sizeof(char) * (i + 1));//tail 0
            if (!client->host) {
                return -1;
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
            return -1;
        } else {
            if (CharDot(p[i])) {
                if (last || first || (!CharIsNumber(prev) && !CharIsAlpha(prev))) {
                    return -1;
                }
                matchDot = 1;
            } else if (CharIsColon(p[i])) {
                if (last || first || (!CharIsNumber(prev) && !CharIsAlpha(prev))) {
                    return -1;
                }
                if (matchColon) {
                    //colon can only be matched once
                    return -1;
                }
                matchColon = 1;
                colonPos = i;
            } else if (CharBar(p[i])) {
                if (first || last || (!CharIsNumber(prev) && !CharIsAlpha(prev))) {
                    return -1;
                }
            } else {
                if (matchColon) {
                    if (!CharIsNumber(p[i])) {
                        //char after ':' must be numeric
                        return -1;
                    } else {
                        buf[pl++] = p[i];
                    }
                }
                if (last) {
                    //last checked
                    if (!matchDot) {
                        if (!IsLocalhost(url)) {
                            //only localhost do not contain dot
                            return -1;
                        }
                    }
                    if (matchColon) {
                        if (!CheckPortValidaty((port = atoi(buf)))) {
                            return -1;
                        } else {
                            client->port = port;
                        }
                    }

                    client->host = MemAlloc(sizeof(char) * (i + 2));//include current character
                    if (!client->host) {
                        return -1;
                    }
                    //url passed the validation
                    if (!matchColon) {
                        client->port = HTTP_DEFAULT_PORT;
                        memcpy(client->host, p, i + 1);
                    } else {
                        memcpy(client->host, p, colonPos);
                    }
                } else {
                    i++;
                    prev = p[i];
                }
            }
        }
    }
    success:
    p += (i + 1);
    left -= (i + 1);
    if (left == 0) {
        //full url parse finished
        return 1;
    }
    i = 0;
    off_t start = 0, end = 0, delta = 0;
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
        if (parsePathFinished && status == 1) {
            if (p[i] == CHAR_QUESTION_MARK) {
                status = 2;
                start = i;
            }
        }
        if (parseQueryFinished && status == 2) {
            if (p[i] == CHAR_HASH_TAG) {
                status = 3;
                start = i;
            }
        }
        if (status == 1 && (CHAR_QUESTION_MARK == p[i] || CHAR_HASH_TAG == p[i] || last)) {
            if (CHAR_QUESTION_MARK == p[i] || CHAR_HASH_TAG == p[i]) {
                delta = i - start;
            } else {
                //last char
                delta = i - start + 1;
            }
            //delta  contain tail 0
            tb = MemAlloc(sizeof(char) * delta);
            if (!tb) {
                return -1;
            } else {
                memcpy(tb, p + start + 1, delta - 1);
                client->path = tb;
                parsePathFinished = 1;
                if (last) {
                    return 1;
                }
            }
        }
        if (status == 2 && (CHAR_HASH_TAG == p[i] || last)) {
            if (CHAR_HASH_TAG == p[i]) {
                delta = i - start;
            } else {
                delta = i - start + 1;
            }
            int j;
            const char *pc = p + start + 1;
            int ks = 0, ke;
            int equalPos = 0;
            size_t kl, vl, ml;
            short matchEqual = 0;
            char *key, *value;
            short keyMatchStart = 0;
            struct HttpQueryParam *param;
            short queryMatchLast = 0;
            short isAddr = 0;
            for (j = 0; j < (delta - 1); ++j) {
                queryMatchLast = j == (delta - 1);
                isAddr = pc[j] == CHAR_ADDRESS;
                if (isAddr || queryMatchLast) {
                    if (!keyMatchStart) {
                        keyMatchStart = 1;
                        ks = i;
                    } else {
                        if (!matchEqual) {
                            kl = isAddr ? (j - ks) : (j - ks + 1);
                        } else {
                            kl = (equalPos - ks);
                        }
                        key = MemAlloc(sizeof(char) * kl);
                        if (!key) {
                            return -1;
                        } else {
                            memcpy(key, pc + ks + 1, kl - 1);
                        }
                        param = MemAlloc(sizeof(struct HttpQueryParam));
                        if (!param) {
                            free(key);
                            return -1;
                        }
                        param->name = key;
                        HashAdd(client->headers, key, param);
                        if (matchEqual) {
                            vl = isAddr ? (j - (equalPos + 1)) : (j - (equalPos + 1) + 1);
                        } else {
                            vl = 0;
                        }
                        if (vl) {
                            value = MemAlloc(sizeof(char) * vl);
                            if (!value) {
                                return -1;
                            } else {
                                memcpy(value, pc + equalPos + 1, vl - 1);
                                param->value = value;
                            }
                        } else {
                            param->value = NULL;
                        }
                        keyMatchStart = 0;
                        matchEqual = 0;
                    }
                } else if (pc[j] == CharEqual) {
                    equalPos = j;
                    matchEqual = 1;
                } else if (j == 0) {
                    keyMatchStart = 1;
                    ks = i;
                }
            }
            parseQueryFinished = 1;
        }

        if (status == 3 && last) {
            delta = i - (start + 1) + 2;
            tb = MemAlloc(sizeof(char) * (delta));
            if (!tb) {
                return -1;
            } else {
                memcpy(tb, p + start + 1, delta - 1);
                client->path = tb;
            }
        }

        i++;
    }
    return 1;

}

#include "http.h"
