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

const char *FileExtension(const char *filename);

#endif //STL_CLONE_FILE_H
