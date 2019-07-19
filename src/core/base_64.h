//
// Created by Administrator on 2019/7/19 0019.
//

#ifndef STL_CLONE_BASE_64_H
#define STL_CLONE_BASE_64_H

#include "global_header.h"

#define MASK_FOURTH_CHAR 0x00FC0000
#define MASK_THIRD_CHAR  0x0003F000
#define MASK_SECOND_CHAR 0x00000FC0
#define MASK_FIRST_CHAR  0x0000003F

#define  BASE64_DECODE_MASK_THIRD  0x00ff0000
#define  BASE64_DECODE_MASK_SECOND 0x0000ff00
#define  BASE64_DECODE_MASK_FIRST  0x000000ff

static char Base64CharMap(short c);

static short Base64CharDecode(char c);

char *Base64_encrypt(char *src, size_t len);

char *Base64_decrypt(char *src, size_t len);

#endif //STL_CLONE_BASE_64_H
