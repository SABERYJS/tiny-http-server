//
// Created by Administrator on 2019/7/8 0008.
//

#include "log.h"

/**
 * @param system system whether write to system log
 * @param fd write file to file related with fd
 * @param filename write log to filename
 * **/
struct Log *LogCreate(unsigned short system, unsigned int fd, char *filename, unsigned short level) {
    struct Log *log = MemAlloc(sizeof(struct Log));
    if (!log) {
        return NULL;
    } else {
    }
}