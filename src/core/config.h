//
// Created by Administrator on 2019/7/10 0010.
//

#ifndef STL_CLONE_CONFIG_H
#define STL_CLONE_CONFIG_H

#include "global_header.h"

#define CONFIG_FILE_BUFFER  1024

struct Config {
    char *filename;
    struct HashTable *config_items;
};

#endif //STL_CLONE_CONFIG_H
