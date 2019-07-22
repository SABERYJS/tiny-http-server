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

#include "http_url_encode.h"

char *HttpUrlDecode(char *str) {
    int d = 0; /* whether or not the string is decoded */

    char *dStr = malloc(strlen(str) + 1);
    if (!dStr) {
        return NULL;
    }
    char eStr[] = "00"; /* for a hex code */

    strcpy(dStr, str);

    while (!d) {
        d = 1;
        int i; /* the counter for the string */

        for (i = 0; i < strlen(dStr); ++i) {

            if (dStr[i] == '%') {
                if (dStr[i + 1] == 0)
                    return dStr;

                if (isxdigit(dStr[i + 1]) && isxdigit(dStr[i + 2])) {

                    d = 0;

                    /* combine the next to numbers into one */
                    eStr[0] = dStr[i + 1];
                    eStr[1] = dStr[i + 2];

                    /* convert it to decimal */
                    long int x = strtol(eStr, NULL, 16);

                    /* remove the hex */
                    memmove(&dStr[i + 1], &dStr[i + 3], strlen(&dStr[i + 3]) + 1);

                    dStr[i] = x;
                }
            }
        }
    }

    return dStr;
}