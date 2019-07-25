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

#include "http_media_type.h"


struct MediaTypeConfig *MediaTypeCreate(size_t bucket_size, struct Log *log, unsigned int (*hashMap)(char *)) {
    struct MediaTypeConfig *config = MemAlloc(sizeof(struct MediaTypeConfig));
    if (!config) {
        return NULL;
    } else {
        if (!(config->hash = HashTableCreate(bucket_size, hashMap))) {
            goto failed;
        } else {
            if (!log) {
                if (!(config->log = LogCreate(0, STDOUT_FILENO, NULL, LOG_LEVEL_INFO))) {
                    goto failed;
                }
            } else {
                config->log = log;
            }
            return config;
        }
    }

    failed:
    MemFree(config);
    if (config->hash) {
        MemFree(config->hash);
    }
    return NULL;
}

int MediaTypeConfigParse(struct MediaTypeConfig *config, const char *filename) {
    struct Log *log = config->log;
    int fd = FileOpen(filename, FILE_OPEN_READ_ONLY, 0);
    if (!fd) {
        LogError(log, "open media config file:%s failed\n", filename);
        return -1;
    } else {
        size_t rb;
        char buffer[MEDIA_TYPE_CONFIG_FILE_PARSE_CHUNK] = {TAIL};
        short matchStart = 0;
        int i, j;
        char c, m;
        char *ptr;
        size_t leftLen;
        short blockStartMatched = 0;
        short blockEndMatched = 0;
        short lineStartMatched = 0;
        short mediaTypeMatchEnded = 0;//match media type
        short extMatchStart = 0, extTrueMatched = 0;
        int start, end, mediaTypeEndPos, extStartPos;
        struct HashTable *hash = config->hash;
        char *mediaType, *mediaExt, *ext;
        size_t delta;
        int extItemStart;
        short extItemMatchStart = 0;
        struct MediaTypeEntry *entry;
        short extLastChar = 0;
        if ((rb = read(fd, buffer, MEDIA_TYPE_CONFIG_FILE_PARSE_CHUNK))) {
            ptr = buffer;
            i = 0;//reset count
            while (i < rb) {
                c = ptr[i];
                leftLen = strlen(ptr + i);//left character count
                if (!matchStart) {
                    if (!CharIsAlpha(c)) {
                        if (!CharIsSpace(c) && !CharIsNewLine(c) && !CharIsEnter(c)) {
                            LogError(log, "Only space is permitted before block name\n");
                            return -1;
                        } else {
                            i++;
                            continue;
                        }
                    } else {
                        if (leftLen < 5) {
                            //byte num is not enough
                            LogError(log, "check media type config file format\n");
                            return -1;
                        }
                        if (strncmp(ptr + i, "types", 5) != 0) {
                            LogError(log, "please check file format,block name 'types' required\n");
                            return -1;
                        } else {
                            matchStart = 1;
                            i += 5;
                            continue;
                        }
                    }
                } else if (!blockStartMatched) {
                    if (!CharIsBracketLeft(c)) {
                        if (!CharIsSpace(c)) {
                            LogError(log, "any char can not happen between block name[types] and bracket[}]\n");
                            return -1;
                        } else {
                            i++;
                            continue;
                        }
                    } else {
                        blockStartMatched = 1;//block start matched
                        i++;
                    }
                } else if (!blockEndMatched) {
                    if (CharIsBracketRight(c)) {
                        blockEndMatched = 1;
                        i++;
                    } else {
                        if (!lineStartMatched) {
                            //not match first alpha
                            if (!CharIsAlpha(c)) {
                                if (!CharIsSpace(c) && !CharIsNewLine(c) && !CharIsEnter(c)) {
                                    return -1;
                                } else {
                                    i++;
                                }
                            } else {
                                lineStartMatched = 1;
                                start = i;
                                i++;
                            }
                        } else {
                            //';' not detected
                            if (!CharIsSemicolon(c)) {
                                if (!extMatchStart) {
                                    if (!mediaTypeMatchEnded) {
                                        //space detected
                                        if (CharIsSpace(c)) {
                                            if ((i - start) <= 0) {
                                                LogError(log, "length of media type can not be 0\n");
                                                return -1;
                                            } else {
                                                mediaTypeMatchEnded = 1;//full media type matched
                                                mediaTypeEndPos = i;
                                                i++;
                                                extMatchStart = 1;//start match ext names
                                                continue;
                                            }
                                        }
                                        if (!CharIsAlpha(c) && !CharIsNumber(c) && !CharIsBlackSlash(c) &&
                                            !CharIsPlus(c) &&
                                            !CharIsBar(c) && !CharDot(c)) {
                                            LogError(log,
                                                     "mime type can only contain alpha,number,backslash,dot and plus\n");
                                            return -1;
                                        } else {
                                            i++;
                                            continue;
                                        }
                                    }
                                } else {
                                    if (!extTrueMatched) {
                                        if (CharIsAlpha(c) || CharIsNumber(c)) {
                                            extStartPos = i;
                                            extTrueMatched = 1;
                                            i++;
                                            continue;
                                        } else {
                                            if (!CharIsSpace(c)) {
                                                LogError(log, "Invalid Character detected before starting match ext\n");
                                                return -1;
                                            } else {
                                                i++;
                                                continue;
                                            }
                                        }
                                    } else {
                                        if (!CharIsSpace(c) && !CharIsAlpha(c) && !CharIsNumber(c)) {
                                            LogError(log, "ext name can only contain  alpha,number\n");
                                            return -1;
                                        }
                                        i++;
                                        continue;
                                    }
                                }
                            } else {
                                if (!mediaTypeMatchEnded || !extTrueMatched || (i - extStartPos) <= 0) {
                                    LogError(log,
                                             "line parse finished,but error happened,please check config format\n");
                                    return -1;
                                } else {
                                    delta = (mediaTypeEndPos - start);
                                    if (!(mediaType = MemAlloc(delta + 1))) {
                                        LogError(log, "MemAlloc called[Media type] failed\n");
                                        return -1;
                                    } else {
                                        memcpy(mediaType, ptr + start, delta);//one full line
                                        mediaExt = ptr + extStartPos;//ext start from here
                                        delta = (i - extStartPos);
                                        for (j = 0; j < delta; j++) {
                                            extLastChar = (j == (delta - 1));
                                            m = mediaExt[j];
                                            if (!extItemMatchStart) {
                                                if (CharIsSpace(m)) {
                                                    continue;
                                                } else {
                                                    extItemMatchStart = 1;
                                                    extItemStart = j;
                                                }
                                            } else {
                                                if (CharIsSpace(m) || extLastChar) {
                                                    //if c is the last char and is not space char,we also can run here
                                                    //one ext name matched
                                                    if (!(ext = MemAlloc(
                                                            j - extItemStart + 1 + (CharIsSpace(m) ? 0 : 1)))) {
                                                        LogError(log, "MemAlloc called[ext name] failed\n");
                                                        return -1;
                                                    } else {
                                                        memcpy(ext, mediaExt + extItemStart,
                                                               (j - extItemStart + (CharIsSpace(m) ? 0 : 1)));
                                                        extItemMatchStart = 0;//reset flag
                                                        //add ext to hash
                                                        if (!(entry = MemAlloc(sizeof(struct MediaTypeEntry)))) {
                                                            LogError(log, "MemAlloc called[media type entry] failed\n");
                                                            return -1;
                                                        } else {
                                                            entry->ext_name = ext;
                                                            entry->header_value = mediaType;
                                                            if (HashAdd(hash, ext, entry) < 0) {
                                                                LogError(log, "add ext[%s] to hash failed\n", ext);
                                                                return -1;
                                                            } else {
                                                            }
                                                        }
                                                    }
                                                } else {
                                                    //do nothing here
                                                    continue;
                                                }
                                            }
                                        }
                                    }
                                    //one line matched finished,so we reset some flag
                                    lineStartMatched = 0;
                                    mediaTypeMatchEnded = 0;
                                    extMatchStart = 0;
                                    extTrueMatched = 0;
                                    extItemMatchStart = 0;
                                    i++;
                                }
                            }
                        }
                    }
                } else {
                    if (!CharIsSpace(c) && !CharIsEnter(c) && !CharIsNewLine(c)) {
                        LogError(log, "block ended,but extra character is detected\n");
                        return -1;
                    } else {
                        i++;
                    }
                }
            }
        } else {
            LogError(log, "read  media config file :%s failed\n", filename);
            return -1;
        }

        if (lineStartMatched || mediaTypeMatchEnded || extMatchStart || extTrueMatched || extItemMatchStart) {
            LogError(log, "config file is  not complete\n");
            return -1;
        }
        //config file parse finished
        return 1;
    }
}


char *MediaTypeQuery(struct MediaTypeConfig *config, const char *key) {
    struct MediaTypeEntry *entry = (struct MediaTypeEntry *) HashSearch(config->hash, key);
    return entry->header_value;
}