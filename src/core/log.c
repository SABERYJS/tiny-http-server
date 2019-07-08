//
// Created by Administrator on 2019/7/8 0008.
//

#include "log.h"

/**
 * @param system system whether write to system log
 * @param fd write file to file related with fd
 * @param filename write log to filename
 * **/
struct Log *LogCreate(unsigned short system, unsigned int fd, const char *filename, unsigned short level) {
    struct Log *log = MemAlloc(sizeof(struct Log));
    if (!log) {
        return NULL;
    } else {
        if (system) {
            log->system = 1;
            openlog("http server", LOG_PID, LOG_USER);
        } else {
            log->system = 0;
            if (fd) {
                log->fd = fd;
            } else {
                if (!filename) {
                    goto failed;
                } else {
                    log->filename = filename;
                    log->fd = FileOpen(filename, FILE_OPEN_WRITE_ONLY | FILE_OPEN_CREATE,
                                       FILE_AUTH_OWNER_WRITE | FILE_AUTH_OTHER_READ | FILE_AUTH_GROUP_READ);
                    if (!log->fd) {
                        MemFree(log);
                        goto failed;
                    }
                }
            }
        }
        log->level = level;
        return log;
    }

    failed:
    MemFree(log);
    return NULL;
}


int LogWrite(struct Log *log, int level, char *format, ...) {
    //check  log level
    if (log->level > level) {
        return 1;
    }
    char *buf = MemAlloc(LOG_OUTPUT_BUFFER_SIZE);
    if (!buf) {
        return -1;
    }
    va_list vl;
    int ret;
    va_start(vl, format);
    ret = vsnprintf(buf, LOG_OUTPUT_BUFFER_SIZE - 1, format, vl);//tail 0
    va_end(vl);
    if (!ret) {
        MemFree(buf);
        return -1;
    }
    int flag = 0;
    if (log->system) {
        if (level & LOG_LEVEL_FATAL) {
            flag |= LOG_EMERG;
        }
        if (level & LOG_LEVEL_ERROR) {
            flag |= LOG_ERR;
        }
        if (level & LOG_LEVEL_DEBUG) {
            flag |= LOG_DEBUG;
        }
        if (level & LOG_LEVEL_INFO) {
            flag |= LOG_INFO;
        }
        if (level & LOG_LEVEL_WARN) {
            flag |= LOG_WARNING;
        }
        flag |= LOG_USER;
        syslog(flag, buf);
    } else {
        ret = write(log->fd, buf, strlen(buf));
    }
    MemFree(buf);
    return ret;
}

int LogInfo(struct Log *log, char *format, ...) {
    va_list vl;
    int ret;
    va_start(vl, format);
    ret = LogWrite(log, LOG_LEVEL_INFO, format, vl);
    va_end(vl);
    return ret;
}

int LogError(struct Log *log, char *format, ...) {
    va_list vl;
    int ret;
    va_start(vl, format);
    ret = LogWrite(log, LOG_LEVEL_ERROR, format, vl);
    va_end(vl);
    return ret;
}

int LogDebug(struct Log *log, char *format, ...) {
    va_list vl;
    int ret;
    va_start(vl, format);
    ret = LogWrite(log, LOG_LEVEL_DEBUG, format, vl);
    va_end(vl);
    return ret;
}

int LogFatal(struct Log *log, char *format, ...) {
    va_list vl;
    int ret;
    va_start(vl, format);
    ret = LogWrite(log, LOG_LEVEL_FATAL, format, vl);
    va_end(vl);
    return ret;
}

int LogWarn(struct Log *log, char *format, ...) {
    va_list vl;
    int ret;
    va_start(vl, format);
    ret = LogWrite(log, LOG_LEVEL_WARN, format, vl);
    va_end(vl);
    return ret;
}