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
            backend->entry = client->entry_file;

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
            BackendCreateEnvironmentVariables(backend);
            LogInfo(backend->log, "BackendCreateEnvironmentVariables\n");
            BackendCreateExecArgv(backend);
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
            chdir(server.docRoot);//switch server root directory to server.docRoot
            if (execve(server.cgiPath, backend->exec_argv, backend->environments) < 0) {
                LogError(backend->log, "execve failed:%s\n", strerror(errno));
                exit(100);
            }
        }
    }
}

int BackendCreateExecArgv(struct Backend *backend) {
    int i;
    size_t lenCgiPath = strlen(server.cgiPath);
    struct Log *log = backend->log;
    char *cgiPath = MemAlloc(lenCgiPath + 1);
    if (!cgiPath) {
        goto failed;
    } else {
        memcpy(cgiPath, server.cgiPath, lenCgiPath);
        backend->exec_argv[0] = cgiPath;
        LogInfo(log, "cig path:%s\n", cgiPath);
    }

    char *option = MemAlloc(3);
    if (!option) {
        goto failed;
    } else {
        memcpy(option, "-f", 2);
        backend->exec_argv[1] = option;
        LogInfo(log, "option:%s\n", option);
    }

    size_t len = strlen(backend->client->entry_file ?: server.default_file);
    char *entry_file = MemAlloc(len + 1);
    if (!entry_file) {
        goto failed;
    } else {
        memcpy(entry_file, backend->client->entry_file ?: server.default_file, len);
        backend->exec_argv[2] = entry_file;
        LogInfo(log, "entry file:%s\n", entry_file);
        backend->exec_argv[3] = NULL;
        return 1;
    }

    failed:
    if (cgiPath) {
        MemFree(cgiPath);
    }
    if (option) {
        MemFree(option);
    }

    return -1;
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
int BackendCreateEnvironmentVariables(struct Backend *backend) {
    struct Client *client = backend->client;
    int i;
    int count = 0;
    int ret;
    for (i = 0; i < NUM_ENVIRONMENT_VARIABLES; i++) {
        if (cgiEnvDefinitions[i].flag == 0) {
            //process finished
            backend->environments[i] = NULL;
            return 1;
        }
        if ((ret = cgiEnvDefinitions[i].callback(backend, count)) < 0) {
            return -1;
        } else {
            if (ret > 1) {
                ++count;
            }
        }
    }
    return 1;
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
        LogInfo(backend->log, "cgi-env:%s\n", software);
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
        LogInfo(backend->log, "cgi-env:%s\n", host);
        return 1;
    }
}


/**
 * server protocol environment variable init
 * **/
int CgiServerProtocolInitCallback(struct Backend *backend, int next) {
    struct Client *client = backend->client;
    size_t prefixLen = strlen(CGI_SERVER_PROTOCOL_TEXT);
    size_t protocolLen = strlen(client->protocol_version);
    size_t len = prefixLen + protocolLen + 1;
    char *protocol = MemAlloc(len);
    if (!protocol) {
        return -1;
    } else {
        memcpy(protocol, CGI_SERVER_PROTOCOL_TEXT, prefixLen);
        memcpy(protocol + prefixLen, client->protocol_version, protocolLen);
        backend->environments[next] = protocol;
        LogInfo(backend->log, "cgi-env:%s\n", protocol);
        return 1;
    }
}

/**
 * server port environment variable init
 * **/
int CgiServerPortInitCallback(struct Backend *backend, int next) {
    struct Client *client = backend->client;
    size_t prefixLen = strlen(CGI_SERVER_PORT_TEXT);
    size_t portLen = strlen(client->port);
    size_t len = prefixLen + portLen + 1;
    char *port = MemAlloc(len);
    if (!port) {
        return -1;
    } else {
        memcpy(port, CGI_SERVER_PORT_TEXT, prefixLen);
        memcpy(port + prefixLen, client->port, portLen);
        backend->environments[next] = port;
        LogInfo(backend->log, "cgi-env:%s\n", port);
        return 1;
    }
}


/**
 * request method environment variable init
 * **/
int CgiRequestMethodInitCallback(struct Backend *backend, int next) {
    struct Client *client = backend->client;
    size_t prefixLen = strlen(CGI_REQUEST_METHOD_TEXT);
    size_t methodLen = strlen(client->tMethod);
    size_t len = prefixLen + methodLen + 1;
    char *method = MemAlloc(len);
    if (!method) {
        return -1;
    } else {
        memcpy(method, CGI_REQUEST_METHOD_TEXT, prefixLen);
        memcpy(method + prefixLen, client->tMethod, methodLen);
        backend->environments[next] = method;
        LogInfo(backend->log, "cgi-env:%s\n", method);
        return 1;
    }
}


/**
 * path info environment variable init
 * **/
int CgiPathInfoInitCallback(struct Backend *backend, int next) {
    struct Client *client = backend->client;
    size_t prefixLen = strlen(CGI_PATH_INFO_TEXT);
    size_t piLen = strlen(client->path_info);
    size_t len = prefixLen + piLen + 1;
    char *pathInfo = MemAlloc(len);
    if (!pathInfo) {
        return -1;
    } else {
        memcpy(pathInfo, CGI_PATH_INFO_TEXT, prefixLen);
        memcpy(pathInfo + prefixLen, client->path_info, piLen);
        backend->environments[next] = pathInfo;
        LogInfo(backend->log, "cgi-env:%s\n", pathInfo);
        return 1;
    }
}

int CgiScriptNameInitCallback(struct Backend *backend, int next) {
    struct Client *client = backend->client;
    if (!client->script_name) {
        return 0;
    }
    size_t prefixLen = strlen(CGI_SCRIPT_NAME_TEXT);
    size_t scriptNameLen = strlen(client->script_name);
    size_t len = prefixLen + scriptNameLen + 1;
    char *scriptName = MemAlloc(len);
    if (!scriptName) {
        return -1;
    } else {
        memcpy(scriptName, CGI_SCRIPT_NAME_TEXT, prefixLen);
        memcpy(scriptName + prefixLen, client->script_name, scriptNameLen);
        backend->environments[next] = scriptName;
        LogInfo(backend->log, "cgi-env:%s\n", scriptName);
        return 1;
    }
}


int CgiQueryStringInitCallback(struct Backend *backend, int next) {
    struct Client *client = backend->client;
    //firstly,we check if client->query_string is NULL,if client->query_string is NULL,we return 0
    if (!client->query_string) {
        return 0;
    }
    size_t prefixLen = strlen(CGI_QUERY_STRING_TEXT);
    size_t queryStringLen = strlen(client->query_string);
    size_t len = prefixLen + queryStringLen + 1;
    char *queryString = MemAlloc(len);
    if (!queryString) {
        return -1;
    } else {
        memcpy(queryString, CGI_QUERY_STRING_TEXT, prefixLen);
        memcpy(queryString + prefixLen, client->query_string, queryStringLen);
        backend->environments[next] = queryString;
        LogInfo(backend->log, "cgi-env:%s\n", queryString);
        return 1;
    }
}

int CgiRemoteHostInitCallback(struct Backend *backend, int next) {
    return 0;
}

int CgiRemoteAddrInitCallback(struct Backend *backend, int next) {
    struct Client *client = backend->client;
    const char *ip = Ipv4TranslateToVisualText(client->addr);
    if (!ip) {
        return -1;
    } else {
        size_t prefixLen = strlen(CGI_REMOTE_ADDR_TEXT);
        size_t ipLen = IPV4_STR_LENGTH;//tail 0 included
        size_t len = prefixLen + ipLen;
        char *addr = MemAlloc(len);
        if (!addr) {
            return -1;
        } else {
            memcpy(addr, CGI_REMOTE_ADDR_TEXT, prefixLen);
            memcpy(addr + prefixLen, ip, ipLen - 1);
            backend->environments[next] = addr;
            LogInfo(backend->log, "cgi-env:%s\n", addr);
            return 1;
        }
    }
}

int CgiContentTypeInitCallback(struct Backend *backend, int next) {
    struct Client *client = backend->client;
    if (!client->content_type) {
        return 0;
    } else {
        size_t prefixLen = strlen(CGI_CONTENT_TYPE_TEXT);
        size_t ctLen = strlen(client->content_type);//tail 0 included
        size_t len = prefixLen + ctLen + 1;
        char *addr = MemAlloc(len);
        if (!addr) {
            return -1;
        } else {
            memcpy(addr, CGI_CONTENT_TYPE_TEXT, prefixLen);
            memcpy(addr + prefixLen, client->content_type, ctLen);
            backend->environments[next] = addr;
            LogInfo(backend->log, "cgi-env:%s\n", addr);
            return 1;
        }
    }
}

int CgiContentLengthInitCallback(struct Backend *backend, int next) {
    return 0;
}

int CgiPhpRedirectStatusInitCallback(struct Backend *backend, int next) {
    size_t len = strlen(CGI_REDIRECT_STATUS_TEXT);
    char *buf = MemAlloc(len + 1);
    if (!buf) {
        return -1;
    } else {
        memcpy(buf, CGI_REDIRECT_STATUS_TEXT, len);
        backend->environments[next] = buf;
        LogInfo(backend->log, "cgi-env:%s\n", buf);
        return 1;
    }
}

int CgiDocumentRootInitCallback(struct Backend *backend, int next) {
    size_t prefixLen = strlen(CGI_DOCUMENT_ROOT_TEXT);
    size_t docLen = strlen(server.docRoot);
    size_t len = prefixLen + docLen + 1;
    char *docRoot = MemAlloc(len);
    if (!docRoot) {
        return -1;
    } else {
        memcpy(docRoot, CGI_DOCUMENT_ROOT_TEXT, prefixLen);
        memcpy(docRoot + prefixLen, server.docRoot, docLen);
        backend->environments[next] = strReplace(docRoot, '\\', '/');
        LogInfo(backend->log, "cgi-env:%s\n", docRoot);
        return 1;
    }
}

int CgiScriptFilenameInitCallback(struct Backend *backend, int next) {
    struct Client *client = backend->client;
    char *rootDir = server.docRoot;
    char *scriptName;
    if (!client->script_name) {
        size_t prefixLen = strlen(server.default_file);
        scriptName = MemAlloc(prefixLen + 2);
        if (!scriptName) {
            return -1;
        } else {
            scriptName[0] = '/';
            memcpy(scriptName + 1, server.default_file, prefixLen);
        }
    } else {
        scriptName = client->script_name;
    }
    size_t prefixLen = strlen(CGI_SCRIPT_FILENAME_TEXT);
    size_t rootLen = strlen(rootDir);
    size_t scriptNameLen = strlen(scriptName);
    size_t len = rootLen + scriptNameLen + prefixLen + 1;
    char *buf = MemAlloc(len);
    if (!buf) {
        return -1;
    } else {
        memcpy(buf, CGI_SCRIPT_FILENAME_TEXT, prefixLen);
        memcpy(buf + prefixLen, rootDir, rootLen);
        memcpy(buf + rootLen + prefixLen, scriptName, scriptNameLen);
        backend->environments[next] = strReplace(buf, '\\', '/');
        LogInfo(backend->log, "cgi-env:%s\n", buf);
        return 1;
    }
}