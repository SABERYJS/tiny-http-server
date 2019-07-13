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


#include "../src/core/net.h"
#include <sys/select.h>
#include "../src/algorithm/rb-tree.h"
#include "../src/core/string-tool.h"
#include "../src/core/config.h"
#include "../src/core/log.h"

extern char **environ;

#define __linux__

int SetProcessTitle(int argc, char **argv, char *title, int len) {
#ifdef  __linux__
    //linux system,argv and environ  stored one after another
    //so set process title is complicated
    char *pt = MemAlloc(len + 2);
    if (!pt) {
        return -1;
    } else {
        pt[0] = ' ';
        memcpy(pt + 1, title, len);
        title = pt;
    }
    char **p = argv;
    size_t al = strlen(p[0]);
    if (al >= len) {
        //argv[0] has enough memory to store title,then just set argv[0]
        memcpy(p[0], title, len);
        //left space  reset to 0
        memset(p[0] + len, 0, (al - len));
        return 1;
    } else {
        //firstly allocate memory to store environment variables
        size_t tl = 0;
        size_t til;
        char **pe = environ;
        char *envBuf;
        char *next;
        int i = 0;
        size_t nl;
        while (*pe) {
            tl += (strlen(*pe) + 1);//tail 0
            pe++;
        }
        envBuf = MemAlloc(tl);
        if (!envBuf) {
            return -1;
        }
        pe = environ;//reset again
        next = envBuf;
        while (*pe) {
            til = strlen(*pe);
            memcpy(next, *pe, til);
            environ[i++] = next;//reset global environ again
            next += (til + 1);//include tail 0
            pe++;
        }

        //args move backward,firstly calculate total memory length to store all  application args
        tl = 0;
        while (*p) {
            tl += (strlen(*p) + 1);//include tail 0
            p++;
        }
        p = argv;//reset again
        nl = tl - al + len;//new args length
        i = argc - 1;//copy from the last element
        next = p[0] + nl - 1;//last storage position
        while (i >= 0) {
            if (!i) {
                til = len;
            } else {
                til = strlen(p[i]);
            }
            next -= (til + 1);
            memcpy(next, !i ? title : p[i], til);
            argv[i] = next;//reset argv
            next[til] = 0;//Required,Very important
            i--;
        }
        return 1;
    }
#else
    //on windows,specified  Api defined,so ignored
#endif
}

int main(int argc, char **argv) {
    char *title = "clion Debug Set Process  Title";
    SetProcessTitle(argc, argv, title, strlen(title));
    char **p;
    p = argv;
    printf("first char:%c\n", p[0][0]);
    while (*p) {
        printf("argv:%s\n", *p);
        p++;
    }
    p = environ;
    while (*p) {
        printf("env:%s\n", *p);
        p++;
    }
    sleep(40);
}