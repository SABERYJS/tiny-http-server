//
// Created by Administrator on 2019/7/10 0010.
//

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

struct SystemConfigDefinition {
    char *config_name;
    int flag;
    ConfigCallback configCallback;
};
#define SentinelConfig  {NULL, 0, NULL}

struct SystemConfigDefinition definitions[] = {
        {"cgiPath", CONFIG_REQUIRED | CONFIG_FILE_EXECUTABLE | CONFIG_FILE, NULL},
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
