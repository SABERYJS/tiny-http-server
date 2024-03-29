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


#include "client.h"
#include "buffer.h"

struct Client *ClientCreate(int sock, struct sockaddr_in *clientAddr) {
    struct Client *client = MemAlloc(sizeof(struct Client));
    if (!client) {
        return NULL;
    } else {
        client->sock = sock;
        client->addr = clientAddr;
        client->status = STATUS_RECEIVING_HEADER;
        client->headers = HashTableCreate(100, NULL);
        client->query = HashTableCreate(100, NULL);
        client->request_line_parse_status = HTTP_REQUEST_LINE_METHOD;
        if (!client->headers || !client->query) {
            if (client->headers) {
                MemFree(client->headers);
            }
            if (client->query) {
                MemFree(client->query);
            }
            MemFree(client);
            return NULL;
        } else {
            client->buffer = MemAlloc(sizeof(struct ClientBuffer));
            if (!client->buffer) {
                MemFree(client->headers);
                MemFree(client);
                return NULL;
            } else {
                client->buffer->write_pos = 0;
                client->buffer->read_pos = 0;
                memset(client->buffer->buf, 0, CLIENT_RECEIVE_BUFFER_SIZE);
                client->buffer->size = 0;
                client->buffer->left = 0;
                return client;
            }
        }
    }
}