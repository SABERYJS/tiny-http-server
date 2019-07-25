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


#include "config.h"


struct Config *ConfigCreate(char *filename) {
    struct Config *config = MemAlloc(sizeof(struct Config));
    if (!config) {
        goto error;
    } else {
        size_t flen = strlen(filename);
        if ((config->fd = FileOpen(filename, FILE_OPEN_READ_ONLY, 0)) < 0) {
            goto error;
        }
        config->buffer = BufferCreate();
        if (!config->buffer) {
            goto error;
        }
        config->filename = MemAlloc(flen + 1);
        if (!config->filename) {
            goto error;
        }
        memcpy(config->filename, filename, flen);
        config->filename[flen] = 0;
        if (!(config->config_items = HashTableCreate(100, NULL))) {
            goto error;
        }
        if (!(config->log = LogCreate(0, STDOUT_FILENO, NULL, LOG_LEVEL_INFO))) {
            goto error;
        }
        if (!(config->hash_sys_config = HashTableCreate(100, NULL))) {
            goto error;
        } else {
            //add system config definitions
            int i = 0;
            //sentinel  config name is NULL
            while (definitions[i].config_name) {
                if (HashAdd(config->hash_sys_config, definitions[i].config_name, &definitions[i]) < 0) {
                    goto error;
                }
                i++;
            }
        }

        if (!(config->list_sys_config = createList(SystemConfigListComparator, NULL))) {
            goto error;
        } else {
            int j = 0;
            while (definitions[j].config_name) {
                if (appendNode(config->list_sys_config, &definitions[j]) < 0) {
                    goto error;
                }
                j++;
            }
        }
        return config;
    }
    error:
    if (config) {
        if (config->buffer) {
            MemFree(config->buffer);
        }
        if (config->filename) {
            MemFree(config->filename);
        }
        if (config->config_items) {
            MemFree(config->config_items);
        }
        if (config->log) {
            MemFree(config->log);
        }
        if (config->hash_sys_config) {
            MemFree(config->hash_sys_config);
        }
        if (config->list_sys_config) {
            MemFree(config->list_sys_config);
        }
        MemFree(config);
    }
    return NULL;
}

int ConfigFileParse(struct Config *config) {
    short matchStart = 0;
    short equalMatch = 0;
    short matchEnter = 0;
    short keyMatchSpace = 0;
    short valueNonEmpty = 0;
    int start, end, i, equalPos, kend;
    int processed = 0;//calculate how many bytes that has been processed
    size_t rc;
    char c;
    int lineStart;//line start position
    short lineStartMatched = 0;
    short last = 0;
    //start read file
    while ((rc = ReadFromSource(config->fd, config->buffer)) > 0) {
        for (i = 0; i < config->buffer->size; ++i) {
            last = (i == (config->buffer->size - 1));
            c = BufferCharAtPos(config->buffer, i);
            if (CharIsEnter(c)) {
                if (!matchStart || !equalMatch || (i - equalPos) == 1 || matchEnter || !valueNonEmpty) {
                    goto failed;
                }
                matchEnter = 1;
                end = i;
            } else if (CharIsNewLine(c)) {
                if (!matchEnter) {
                    goto failed;
                }
                //match finished,start process text line
                if (!ConfigProcessOneLine(config, start, (kend - 1), equalPos + 1, end - 1)) {
                    goto failed;
                }
                processed += (end - lineStart + 2);//include \r\n
                //clear all parse flag
                matchStart = equalMatch = matchEnter = keyMatchSpace = valueNonEmpty = lineStartMatched = 0;
            } else if (CharIsEqual(c)) {
                if (equalMatch) {
                    goto failed;
                }
                if (!keyMatchSpace) {
                    //key match finished
                    kend = i;
                }
                equalMatch = 1;
                equalPos = i;
            } else {
                if (!matchStart || !equalMatch) {
                    if (!matchStart) {
                        if (!CharIsAlpha(c)) {
                            //space is permitted before any alpha
                            if (!CharIsSpace(c)) {
                                //config name must be alpha
                                goto failed;
                            }
                        } else {
                            matchStart = 1;
                            start = i;
                        }
                    }
                    if (matchStart && !equalMatch) {
                        if (!keyMatchSpace) {
                            //before matching space, c  must be alpha
                            if (!CharIsAlpha(c)) {
                                goto failed;
                            }
                        }
                        if (keyMatchSpace && !CharIsSpace(c)) {
                            //if space matched before, then other character(non space) can  not be matched again
                            goto failed;
                        }
                        if (CharIsSpace(c) && !keyMatchSpace) {
                            keyMatchSpace = 1;
                            //because first charcater  must be alpha ,so key length must be greater than 1
                            //so there is no need to check key validation
                            kend = i;//case;key finished with space
                        }
                    }
                }
                if (equalMatch) {
                    //beacuse user can  type what they want,so checking validity of character is unnecessary
                    //each config  take responsibility for validity
                    //but except one rule: config value can not be empty
                    if (!valueNonEmpty && !CharIsSpace(c)) {
                        valueNonEmpty = 1;
                    }
                }

                if (last && equalMatch && valueNonEmpty && rc < CLIENT_RECEIVE_BUFFER_SIZE) {
                    //file read finished
                    //the last character matched and '=' matched before and value non empty
                    if (!ConfigProcessOneLine(config, start, (kend - 1), equalPos + 1, (config->buffer->size - 1))) {
                        goto failed;
                    } else {
                        processed += (config->buffer->size - lineStart);//from line start to end of line
                    }
                }
            }
            if (!CharIsNewLine(c) && !lineStartMatched) {
                lineStartMatched = 1;
                lineStart = i;
            }
        }
        //no error happened,so discard some bytes
        BufferDiscard(config->buffer, processed);
    }
    if (rc < 0) {
        goto failed;
    } else {
        //config file parse finished
        if (!ConfigCheckSystemConfig(config)) {
            return -1;
        }
        return 1;
    }
    failed:
    return -1;
}

/**
 * @notice both kend and vend contain current character
 * **/
int ConfigProcessOneLine(struct Config *config, int kstart, int kend, int vstart, int vend) {
    struct ClientBuffer *buffer = config->buffer;
    struct ConfigItem *item = MemAlloc(sizeof(struct ConfigItem));
    if (!item) {
        return -1;
    } else {
        item->key = MemAlloc(kend - kstart + 2);
        item->value = MemAlloc(vend - vstart + 2);
        if (!item->key || !item->value) {
            goto failed;
        } else {
            memcpy(item->key, BufferSubstr(buffer, kstart), (kend - kstart + 1));
            memcpy(item->value, BufferSubstr(buffer, vstart), (vend - vstart + 1));
            if (HashAdd(config->config_items, item->key, item) < 0) {
                goto failed;
            } else {
                struct SystemConfigDefinition *definition = HashSearch(config->hash_sys_config, item->key);
                if (definition && !(definition->flag & CONFIG_PARSE_FINISHED)) {
                    //config item parse finished
                    definition->flag |= CONFIG_PARSE_FINISHED;
                    if (definition->configCallback) {
                        if (!definition->configCallback(item)) {
                            return -1;
                        }
                    }
                }
                return 1;
            }
        }
    }
    failed:
    if (item->key) {
        MemFree(item->key);
    }
    if (item->value) {
        MemFree(item->value);
    }
    MemFree(item);
    return -1;
}


static int SystemConfigListComparator(struct ListNode *node, void *data) {
    struct SystemConfigDefinition *d1 = (struct SystemConfigDefinition *) node->data;
    struct SystemConfigDefinition *d2 = (struct SystemConfigDefinition *) data;
    return strcmp(d1->config_name, d2->config_name);
}

static int ConfigCheckSystemConfig(struct Config *config) {
    struct ListNode *node = config->list_sys_config->header;
    struct SystemConfigDefinition *definition;
    while (node) {
        definition = (struct SystemConfigDefinition *) node->data;
        if (definition->flag & CONFIG_REQUIRED && !(definition->flag & CONFIG_PARSE_FINISHED)) {
            return -1;
        }
        node = node->next;
    }
}

int ConfigInitServerCgi(struct ConfigItem *configItem) {
    server.cgiPath = trim(configItem->value, strlen(configItem->value), CHARSPACE);
    return server.cgiPath ? 1 : -1;
}

int ConfigInitServerDocDirectory(struct ConfigItem *configItem) {
    server.docRoot = trim(configItem->value, strlen(configItem->value), CHARSPACE);
    return server.docRoot ? 1 : -1;
}

int ConfigInitServerCgiExtDirectory(struct ConfigItem *configItem) {
    server.cgiExtName = trim(configItem->value, strlen(configItem->value), CHARSPACE);
    return server.cgiExtName ? 1 : -1;
}


int ConfigInitServerCgiDefaultFile(struct ConfigItem *configItem) {
    server.default_file = trim(configItem->value, strlen(configItem->value), CHARSPACE);
    return server.default_file ? 1 : -1;
}


int ConfigInitErrorPage(struct ConfigItem *configItem) {
    server.error_page = trim(configItem->value, strlen(configItem->value), CHARSPACE);
    return server.error_page ? 1 : -1;
}