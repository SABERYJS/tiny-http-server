//
// Created by Administrator on 2019/7/8.
//

#ifndef STL_CLONE_FILE_H
#define STL_CLONE_FILE_H

#include "global_header.h"

#define FILE_OPEN_READ_ONLY O_RDONLY
#define FILE_OPEN_WRITE_ONLY O_WRONLY
#define FILE_OPEN_READ_AND_WRITE O_RDWR
#define FILE_OPEN_APPEND O_APPEND
#define FILE_OPEN_CREATE O_CREAT
#define FILE_OPEN_TRUNC O_TRUNC  //use with O_CREAT
#define FILE_OPEN_NON_BLOCK O_NONBLOCK
#define FILE_OPEN_SYNC O_SYNC


#define FILE_AUTH_OWNER_READ S_IRUSR
#define FILE_AUTH_OWNER_WRITE S_IWUSR
#define FILE_AUTH_OWNER_EXEC S_IXUSR

#define FILE_AUTH_GROUP_READ S_IRGRP
#define FILE_AUTH_GROUP_WRITE S_IWGRP
#define FILE_AUTH_GROUP_EXEC S_IXGRP

#define FILE_AUTH_OTHER_READ S_IROTH
#define FILE_AUTH_OTHER_WRITE S_IWOTH
#define FILE_AUTH_OTHER_EXEC S_IXOTH


int FileOpen(const char *filename, int flag, int mode);

short FileExist(const char *filename);


short FileCanRead(const char *filename);

short FileCanWrite(const char *filename);

short FileCanExec(const char *filename);

#endif //STL_CLONE_FILE_H
