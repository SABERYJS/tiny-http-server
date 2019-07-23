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

/**
 *@notice  about http header,please refer to site [https://www.w3.org/Protocols/rfc2616/rfc2616-sec14.html]
 * **/

#ifndef STL_CLONE_HTTP_HEADER_H
#define STL_CLONE_HTTP_HEADER_H

#define HTTP_HEADER_ACCEPT(flag)  (flag==0?1:"Accept")
#define HTTP_HEADER_ACCEPT_CHARSET(flag) (flag==0?2:"Accept-Charset")
#define HTTP_HEADER_ACCEPT_ENCODING(flag) (flag==0?3:"Accept-Encoding")
#define HTTP_HEADER_ACCEPT_LANGUAGE(flag) (flag==0?4:"Accept-Language")
#define HTTP_HEADER_ACCEPT_RANGES(flag) (flag==0?5:"Accept-Ranges")
#define HTTP_HEADER_AGE(flag) (flag==0?6:"Age")
#define HTTP_HEADER_ALLOW(flag) (flag==0?7:"Allow")
#define HTTP_HEADER_AUTHORIZATION(flag) (flag==0?8:"Allow")
#define HTTP_HEADER_CACHE_CONTROL(flag) (flag==0?9:"Cache-Control")
#define HTTP_HEADER_CONNECTION(flag) (flag==0?10:"Connection")
#define HTTP_HEADER_CONTENT_ENCODING(flag) (flag==0?11:"Content-Encoding")
#define HTTP_HEADER_CONTENT_LANGUAGE(flag) (flag==0?12:"Content-Language")
#define HTTP_HEADER_CONTENT_LENGTH(flag) (flag==0?13:"Content-Length")
#define HTTP_HEADER_CONTENT_LOCATION(flag) (flag==0?14:"Content-Location")
#define HTTP_HEADER_CONTENT_MD5(flag) (flag==0?15:"Content-MD5")
#define HTTP_HEADER_CONTENT_RANGE(flag) (flag==0?16:"Content-Range")
#define HTTP_HEADER_CONTENT_TYPE(flag) (flag==0?17:"Content-Type")
#define HTTP_HEADER_DATE(flag) (flag==0?18:"Date")
#define HTTP_HEADER_ETAG(flag) (flag==0?19:"ETag")
#define HTTP_HEADER_EXPECT(flag) (flag==0?20:"Expect")
#define HTTP_HEADER_EXPIRES(flag) (flag==0?21:"Expires")
#define HTTP_HEADER_FROM(flag) (flag==0?22:"From")
#define HTTP_HEADER_HOST(flag) (flag==0?23:"Host")
#define HTTP_HEADER_IF_MATCH(flag) (flag==0?24:"If-Match")
#define HTTP_HEADER_IF_MODIFIED_SINCE(flag) (flag==0?25:"If-Modified-Since")
#define HTTP_HEADER_IF_NONE_MATCH(flag) (flag==0?26:"If-Modified-Since")
#define HTTP_HEADER_IF_RANGE(flag)  (flag==0?27:"If-Range")
#define HTTP_HEADER_IF_UNMODIFIED_SINCE(flag) (flag==0?28:"If-Unmodified-Since")
#define HTTP_HEADER_LAST_MODIFIED(flag)  (flag==0?29:"Last-Modified")
#define HTTP_HEADER_LOCATION(flag)  (flag==0?30:"Location")
#define HTTP_HEADER_MAX_FORWARDS(flag)  (flag==0?31:"Max-Forwards")
#define HTTP_HEADER_PRAGMA(flag)  (flag==0?32:"Pragma")
#define HTTP_HEADER_PROXY_AUTHENTICATE(flag)  (flag==0?33:"Proxy-Authenticate")
#define HTTP_HEADER_PROXY_AUTHORIZATION(flag)  (flag==0?34:"Proxy-Authorization")
#define HTTP_HEADER_RANGE(flag)  (flag==0?35:"Range")
#define HTTP_HEADER_REFERRER(flag)  (flag==0?36:"Referer")
#define HTTP_HEADER_RETRY_AFTER(flag)  (flag==0?37:"Retry-After")
#define HTTP_HEADER_SERVER(flag)  (flag==0?38:"Server")
#define HTTP_HEADER_TE(flag)  (flag==0?39:"TE")
#define HTTP_HEADER_TRAILER(flag)  (flag==0?40:"Trailer")
#define HTTP_HEADER_TRANSFER_ENCODING(flag)  (flag==0?41:"Transfer-Encoding")
#define HTTP_HEADER_UPGRADE(flag)  (flag==0?42:"Upgrade")
#define HTTP_HEADER_USER_AGENT(flag)  (flag==0?43:"User-Agent")
#define HTTP_HEADER_VARY(flag)  (flag==0?44:"Vary")
#define HTTP_HEADER_VIA(flag)  (flag==0?45:"Via")
#define HTTP_HEADER_WARNING(flag) (flag==0?46:"Warning")
#define HTTP_HEADER_WWW_AUTHENTICATE(flag) (flag==0?47:"WWW-Authenticate")

#endif //STL_CLONE_HTTP_HEADER_H
