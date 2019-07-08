//
// Created by Administrator on 2019/7/8 0008.
//

#ifndef STL_CLONE_LOG_H
#define STL_CLONE_LOG_H

#include "global_header.h"
#include <syslog.h>

//system log  level
#define LOG_LEVEL_ALL 0x01
#define LOG_LEVEL_TRACE 0x02
#define LOG_LEVEL_DEBUG 0x04
#define LOG_LEVEL_INFO 0x08
#define LOG_LEVEL_WARN 0x10
#define LOG_LEVEL_ERROR 0x20
#define LOG_LEVEL_FATAL 0x40
#define LOG_LEVEL_OFF 0x80

struct Log {
    unsigned int fd;//stdout,stderr,stdin ,or any file descriptor
    char *filename;//file to output
    unsigned short level;//combination of  upper log level
    unsigned short system;// write log to system log
};

struct Log *LogCreate(unsigned short system, unsigned int fd, char *filename, unsigned short level);

#endif //STL_CLONE_LOG_H
