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

#include "cgi.h"


struct Backend *BackendCreate(struct Client *client, char *cgi, struct Log *log) {
    struct Backend *backend = MemAlloc(sizeof(struct Backend));
    int flag = 0;
    if (!backend) {
        return NULL;
    } else {
        if (!cgi) {
            cgi = server.cgiPath;//get global config
        }
        if (!log) {
            flag = 1;
            log = LogCreate(0, STDOUT_FILENO, NULL, LOG_LEVEL_INFO);
            if (!log) {
                goto failed;
            }
        }
        if (pipe(backend->pipes) < 0) {
            LogError(log, "pipe failed\n");
            goto failed;
        } else {
            backend->log = log;
            backend->cgiPath = cgi;
            backend->client = client;

            //after create backend finished,we need create HttpResponse object
            client->response = HttpResponseCreate(client, backend->pipes[0], log);
            if (!client->response) {
                goto failed;
            }
            return backend;
        }
    }

    failed:
    MemFree(backend);
    if (flag) {
        MemFree(log);
    }
    return NULL;
}

int BackendExecuteCgiScript(struct Backend *backend) {
    pid_t pid;
    struct HttpResponse *response = backend->client->response;
    if ((pid = fork()) < 0) {
        LogError(backend->log, "fork failed\n");
        return -1;
    } else {
        if (pid > 0) {
            //parent process
            backend->childPid = pid;
            close(backend->pipes[1]);
            //add read event to event system
            HttpResponseRegisterReadEvent(response, server.depositary);
            return 1;
        } else {
            //child process,execute cgi script
            if (backend->pipes[0] != STDIN_FILENO) {
                if (dup2(backend->pipes[0], STDIN_FILENO) != STDIN_FILENO) {
                    LogError(response->log, "dup2 STDIN_FILENO failed\n");
                }
                close(backend->pipes[0]);
            }
            if (backend->pipes[1] != STDOUT_FILENO) {
                if (dup2(backend->pipes[1], STDOUT_FILENO) != STDOUT_FILENO) {
                    LogError(response->log, "dup2  STDOUT_FILENO failed\n");
                }
                close(backend->pipes[1]);
            }
        }
    }
}

/**
 * environment variables  contain:
 * @brief SERVER_SOFTWARE: name/version of HTTP server.
 * @brief SERVER_NAME: host name of the server, may be dot-decimal IP address.
 * @brief SERVER_PROTOCOL: HTTP/version.
 * @brief SERVER_PORT: TCP port (decimal).
 * @brief REQUEST_METHOD: name of HTTP method (see above).
 * @brief PATH_INFO: path suffix, if appended to URL after program name and a slash.
 * @brief SCRIPT_NAME: relative path to the program, like /cgi-bin/script.cgi.
 * @brief QUERY_STRING: the part of URL after ? character.The query string may be composed of *name=value pairs separated with ampersands
 *                     (such as var1=val1&var2=val2...)when used to submit form data transferred via GET method as defined by HTML application/x-www-form-urlencoded.
 * @brief REMOTE_HOST: host name of the client, unset if server did not perform such lookup.
 * @brief REMOTE_ADDR: IP address of the client (dot-decimal).
 * @brief CONTENT_TYPE: Internet media type of input data if PUT or POST method are used, as provided via HTTP header.
 * @brief CONTENT_LENGTH: similarly, size of input data (decimal, in octets) if provided via HTTP header.
 * **/
char **BackendCreateEnvironmentVariables(struct Backend *backend) {
    struct Client *client = backend->client;
    struct HttpRequest *request = client->request;
    char **environables[13];

}

/**
 * server software environment variable init
 * **/
int CgiServerSoftwareInitCallback(struct Backend *backend, int next) {
    size_t prefixLen = strlen(CGI_SERVER_SOFTWARE_TEXT);
    size_t serverNameLen = strlen(SERVER_NAME);
    size_t len = prefixLen + serverNameLen + 1;
    char *software = MemAlloc(len);
    if (!software) {
        return -1;
    } else {
        memcpy(software, CGI_SERVER_SOFTWARE_TEXT, prefixLen);
        memcpy(software + prefixLen, SERVER_NAME, serverNameLen);
        backend->environments[next] = software;
        return 1;
    }
}


/**
 * server name environment variable init
 * **/
int CgiServerNameInitCallback(struct Backend *backend, int next) {
    size_t prefixLen = strlen(CGI_SERVER_NAME_TEXT);
    size_t hostLen = strlen(backend->client->host);
    size_t len = prefixLen + hostLen + 1;
    char *host = MemAlloc(len);
    if (!host) {
        return -1;
    } else {
        memcpy(host, CGI_SERVER_NAME_TEXT, prefixLen);
        memcpy(host + prefixLen, backend->client->host, hostLen);
        backend->environments[next] = host;
        return 1;
    }
}


/**
 * server protocol environment variable init
 * **/
int CgiServerProtocolInitCallback(struct Backend *backend, int next) {
    struct Client *client = backend->client;
    size_t len = strlen("SERVER_PROTOCOL=") + strlen(client->protocol_version) + 1;
    char *protocol = MemAlloc(len);
    if (!protocol) {
        return -1;
    } else {
        memcpy(protocol, client->protocol_version, len - 1);
        backend->environments[next] = protocol;
        return 1;
    }
}

/**
 * server port environment variable init
 * **/
int CgiServerPortInitCallback(struct Backend *backend, int next) {
    struct Client *client = backend->client;
    size_t len = strlen("SERVER_PORT=") + strlen(client->port) + 1;
    char *port = MemAlloc(len);
    if (!port) {
        return -1;
    } else {
        memcpy(port, client->port, len - 1);
        backend->environments[next] = port;
        return 1;
    }
}


/**
 * request method environment variable init
 * **/
int CgiRequestMethodInitCallback(struct Backend *backend, int next) {
    struct Client *client = backend->client;
    size_t len = strlen("REQUEST_METHOD=") + strlen(client->tMethod) + 1;
    char *method = MemAlloc(len);
    if (!method) {
        return -1;
    } else {
        memcpy(method, client->tMethod, len - 1);
        backend->environments[next] = method;
        return 1;
    }
}


/**
 * path info environment variable init
 * **/
int CgiPathInfoInitCallback(struct Backend *backend, int next) {
    struct Client *client = backend->client;
    size_t len = strlen("PATH_INFO=") + strlen(client->path_info) + 1;
    char *path_info = MemAlloc(len);
    if (!path_info) {
        return -1;
    } else {
        memcpy(path_info, client->path_info, len - 1);
        backend->environments[next] = path_info;
        return 1;
    }
}

int CgiScriptNameInitCallback(struct Backend *backend, int next) {
    struct Client *client = backend->client;
    size_t len = strlen(client->path_info) + 1;
    char *path_info = MemAlloc(len);
    if (!path_info) {
        return -1;
    } else {
        memcpy(path_info, client->path_info, len - 1);
        backend->environments[next] = path_info;
        return 1;
    }
}

int CgiQueryStringInitCallback(struct Backend *backend, int next);

int CgiRemoteHostInitCallback(struct Backend *backend, int next);

int CgiRemoteAddrInitCallback(struct Backend *backend, int next);

int CgiContentTypeInitCallback(struct Backend *backend, int next);

int CgiContentLengthInitCallback(struct Backend *backend, int next);