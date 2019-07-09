//
// Created by Administrator on 2019/7/5 0005.
//

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