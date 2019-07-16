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

#ifndef STL_CLONE_CGI_H
#define STL_CLONE_CGI_H

//about CGI,please refer to site:https://tools.ietf.org/html/rfc3875 and https://en.wikipedia.org/wiki/Common_Gateway_Interface

#include "global_header.h"
#include "server.h"
#include "client.h"


#define CGI_VERSION  1.1

#define CGI_SERVER_SOFTWARE 1
#define CGI_SERVER_NAME 2
#define CGI_SERVER_PROTOCOL 3
#define CGI_SERVER_PORT 4
#define CGI_REQUEST_METHOD 5
#define CGI_PATH_INFO 6
#define CGI_SCRIPT_NAME 7
#define CGI_QUERY_STRING 8
#define CGI_REMOTE_HOST 9
#define CGI_REMOTE_ADDR 10
#define CGI_CONTENT_TYPE 11
#define CGI_CONTENT_LENGTH 12


#define CGI_SERVER_SOFTWARE_TEXT ("SERVER_SOFTWARE=")
#define CGI_SERVER_NAME_TEXT ("SERVER_NAME=")
#define CGI_SERVER_PROTOCOL_TEXT  ("SERVER_PROTOCOL=")
#define CGI_SERVER_PORT_TEXT   ("SERVER_PORT=")
#define CGI_REQUEST_METHOD_TEXT ("REQUEST_METHOD=")
#define CGI_PATH_INFO_TEXT  ("PATH_INFO=")
#define CGI_SCRIPT_NAME_TEXT ("SCRIPT_NAME=")
#define CGI_QUERY_STRING_TEXT ("QUERY_STRING=")
#define CGI_REMOTE_HOST_TEXT  ("REMOTE_HOST=")
#define CGI_REMOTE_ADDR_TEXT  ("REMOTE_ADDR=")
#define CGI_CONTENT_TYPE_TEXT ("CONTENT_TYPE=")
#define CGI_CONTENT_LENGTH_TEXT  ("CONTENT_LENGTH=")


struct Backend {
    struct Client *client;
    int pipes[2];//pipes to communicate with  child process
    char *cgiPath;//absolute cgi path
    struct Log *log;//debug
    pid_t childPid;//child process id
    char **environments;//current request environment variables
    char *entry;//entry file
};

typedef int (*CgiEnvironmentVariableInitCallback)(struct Backend *backend);

struct CgiEnvironmentVariableDefinition {
    short flag;
    CgiEnvironmentVariableInitCallback callback;
};

struct CgiEnvironmentVariableDefinition cgiEnvDefinitions[] = {
};

struct Backend *BackendCreate(struct Client *client, char *cgi, struct Log *log);

int BackendExecuteCgiScript(struct Backend *backend);

char **BackendCreateEnvironmentVariables(struct Backend *backend);

int CgiServerSoftwareInitCallback(struct Backend *backend, int next);

int CgiServerNameInitCallback(struct Backend *backend, int next);

int CgiServerProtocolInitCallback(struct Backend *backend, int next);

int CgiServerPortInitCallback(struct Backend *backend, int next);

int CgiRequestMethodInitCallback(struct Backend *backend, int next);

int CgiPathInfoInitCallback(struct Backend *backend, int next);

int CgiScriptNameInitCallback(struct Backend *backend, int next);

int CgiQueryStringInitCallback(struct Backend *backend, int next);

int CgiRemoteHostInitCallback(struct Backend *backend, int next);

int CgiRemoteAddrInitCallback(struct Backend *backend, int next);

int CgiContentTypeInitCallback(struct Backend *backend, int next);

int CgiContentLengthInitCallback(struct Backend *backend, int next);

#endif //STL_CLONE_CGI_H
