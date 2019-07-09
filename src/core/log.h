//
// Created by Administrator on 2019/7/8 0008.
//

#ifndef STL_CLONE_LOG_H
#define STL_CLONE_LOG_H

#include <syslog.h>
#include "global_header.h"
#include "file.h"

//system log  level
#define LOG_LEVEL_ALL 0x01
#define LOG_LEVEL_TRACE 0x02
#define LOG_LEVEL_DEBUG 0x04
#define LOG_LEVEL_INFO 0x08
#define LOG_LEVEL_WARN 0x10
#define LOG_LEVEL_ERROR 0x20
#define LOG_LEVEL_FATAL 0x40

#define LOG_OUTPUT_BUFFER_SIZE 256


struct Log {
    unsigned int fd;//stdout,stderr,stdin ,or any file descriptor
    const char *filename;//file to output
    unsigned short level;//combination of  upper log level
    unsigned short system;// write log to system log
};

struct Log *LogCreate(unsigned short system, unsigned int fd, const char *filename, unsigned short level);

int LogWrite(struct Log *log, int level, char *format, va_list vl);

int LogInfo(struct Log *log, char *format, ...);

int LogError(struct Log *, char *format, ...);

int LogDebug(struct Log *log, char *format, ...);

int LogFatal(struct Log *log, char *format, ...);

int LogWarn(struct Log *log, char *format, ...);

#endif //STL_CLONE_LOG_H
