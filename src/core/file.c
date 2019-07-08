//
// Created by Administrator on 2019/7/8.
//

#include "file.h"


int FileOpen(const char *filename, int flag, int mode) {
    return open(filename, flag, mode);
}

short FileExist(const char *filename) {
    return access(filename, F_OK) == 0;
}


short FileCanRead(const char *filename) {
    return access(filename, R_OK) == 0;
}

short FileCanWrite(const char *filename) {
    return access(filename, W_OK) == 0;
}

short FileCanExec(const char *filename) {
    return access(filename, X_OK) == 0;
}
