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
                    log->fd = FileOpen(filename, FILE_OPEN_WRITE_ONLY | FILE_OPEN_CREATE | FILE_OPEN_TRUNC,
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


int LogWrite(struct Log *log, int level, char *format, va_list vl) {
    //check  log level
    if (log->level > level) {
        return 1;
    }
    char *buf = MemAlloc(LOG_OUTPUT_BUFFER_SIZE);
    if (!buf) {
        return -1;
    }
    int ret;
    ret = vsnprintf(buf, LOG_OUTPUT_BUFFER_SIZE - 1, format, vl);//tail 0
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