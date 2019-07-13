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


#include "command_line.h"


static int CommandOptionCompare(struct ListNode *node, void *data) {
    struct CommandOption *a1 = (struct CommandOption *) node->data;
    struct CommandOption *a2 = (struct CommandOption *) data;
    return strcmp(a1->name, a2->name);
}

static void CommandOptionClear(struct ListNode *node) {
    struct CommandOption *option = (struct CommandOption *) node->data;
    free(option->name);
    if (!option->value) {
        free(option->value);
    }
}

/**
 *support command line option format as follow:
 * --c  display boolean value
 * -c   with  value
 * **/
struct CommandLineArgs *ParseCommandLine(int argc, char *argv[]) {
    struct CommandLineArgs *options = (struct CommandLineArgs *) malloc(sizeof(struct CommandLineArgs));
    if (!options) {
        return NULL;
    } else {
        options->options = createList(CommandOptionCompare, CommandOptionClear);
        if (!options->options) {
            free(options);
            return NULL;
        } else {
            struct CommandOption *upper = NULL;
            int require;
            int i;
            for (i = 1; i < argc; i++) {
                char *option = argv[i];
                if (option[0] == '-') {
                    if (option[1] == '-') {
                        require = 0;
                    } else {
                        require = 1;
                    }
                    if (upper) {
#ifdef COMMAND_LOG_OPEN
                        printf("option %s need value,but missing\n", upper->name);
#endif
                        goto clearOptions;
                    }
                    option += require ? 1 : 2;
                    struct CommandOption *op = (struct CommandOption *) malloc(sizeof(struct CommandOption));
                    if (!op) {
                        goto clearOptions;
                    } else {
                        size_t nl = strlen(option);
                        char *buf = (char *) malloc(sizeof(char) * (nl + 1));
                        if (!buf) {
                            free(op);
                            goto clearOptions;
                        } else {
                            memcpy(buf, option, nl);
                            buf[nl] = 0;
                            op->name = buf;
                            if (require) {
                                upper = op;
                            } else {
                                appendNode(options->options, op);
                            }
                        }
                    }
                } else {
                    if (!upper || upper->value) {
                        goto clearOptions;
                    }
                    size_t vl = strlen(option);
                    char *val = (char *) malloc(sizeof(char) * (vl + 1));
                    if (!val) {
                        goto clearOptions;
                    } else {
                        memcpy(val, option, vl);
                        val[vl] = 0;
                        upper->value = val;
                        appendNode(options->options, upper);
                        upper = NULL;
                    }
                }
            }
            if (upper) {
                goto clearOptions;
            } else {
                return options;
            }
            clearOptions:
            clearList(options->options);
            if (upper) {
                free(upper->name);
                free(upper);
            }
            return NULL;
        }
    }


}
