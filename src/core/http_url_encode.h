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

#ifndef STL_CLONE_HTTP_URL_ENCODE_H
#define STL_CLONE_HTTP_URL_ENCODE_H

#include "global_header.h"

/**
 * about Percent-encoding,please refer to https://zh.wikipedia.org/wiki/%E7%99%BE%E5%88%86%E5%8F%B7%E7%BC%96%E7%A0%81
 * about utf-8 ,please refer to https://en.wikipedia.org/wiki/UTF-8
 * about rfc3986,please refer to https://tools.ietf.org/html/rfc3986
 * **/

char *HttpUrlDecode(char *str);

/**
 * check if char c is reserved char
 * **/
static inline int CharIsReserved(char c) {
    return c == '!' || c == '*' || c == ','
           || c == '\'' || c == '(' || c == ')'
           || c == ';' || c == ':' || c == '@' ||
           c == '&' || c == '=' || c == '+'
           || c == '$' || c == '/' || c == '?' || c == '#' || c == '[' || c == ']';
}


char *HttpUrlEncode(char *src, size_t len);

#endif //STL_CLONE_HTTP_URL_ENCODE_H
