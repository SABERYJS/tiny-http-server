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


#ifndef STL_CLONE_CONFIG_H
#define STL_CLONE_CONFIG_H

#include "global_header.h"
#include "buffer.h"
#include "file.h"
#include "log.h"
#include "http.h"

#define CONFIG_REQUIRED 0x00000001
#define CONFIG_NUMERIC_INTEGER 0x00000002
#define CONFIG_IP_ADDRESS 0x00000004
#define CONFIG_FILE_EXECUTABLE  0x00000008
#define CONFIG_NUMERIC_FLOAT 0x00000010
#define CONFIG_CHARACTER_SEQUENCE 0x00000020
#define CONFIG_FILE 0x00000040
#define CONFIG_PARSE_FINISHED 0x00000080//config parse finished

struct ConfigItem {
    char *key;
    char *value;
};

typedef int (*ConfigCallback)(struct ConfigItem *configItem);

int ConfigInitServerCgi(struct ConfigItem *configItem);

int ConfigInitServerDocDirectory(struct ConfigItem *configItem);

int ConfigInitServerCgiExtDirectory(struct ConfigItem *configItem);

int ConfigInitServerCgiDefaultFile(struct ConfigItem *configItem);

struct SystemConfigDefinition {
    char *config_name;
    int flag;
    ConfigCallback configCallback;
};
#define SentinelConfig  {NULL, 0, NULL}

static struct SystemConfigDefinition definitions[] = {
        {"cgiPath", CONFIG_REQUIRED | CONFIG_FILE_EXECUTABLE | CONFIG_FILE, ConfigInitServerCgi},
        {"docRoot", CONFIG_REQUIRED, ConfigInitServerDocDirectory},
        {"cgiExtName", CONFIG_REQUIRED, ConfigInitServerCgiExtDirectory},
        {"defaultFile", CONFIG_REQUIRED, ConfigInitServerCgiDefaultFile},
        SentinelConfig
};

struct Config {
    char *filename;
    int fd;
    struct HashTable *config_items;
    struct ClientBuffer *buffer;
    struct HashTable *hash_sys_config;//include  system config definitions
    struct Log *log;
    struct List *list_sys_config;//list of system config
};


struct Config *ConfigCreate(char *filename);

int ConfigFileParse(struct Config *config);

int ConfigProcessOneLine(struct Config *config, int kstart, int kend, int vstart, int vend);


static int SystemConfigListComparator(struct ListNode *node, void *data);

static int ConfigCheckSystemConfig(struct Config *config);

#endif //STL_CLONE_CONFIG_H
