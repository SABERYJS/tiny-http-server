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
