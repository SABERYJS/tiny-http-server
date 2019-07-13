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



#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <error.h>

#define    CGI_NAME "E:\\phpstudy\\PHPTutorial\\php\\php-7.2.1-nts\\php-cgi.exe"
#define    REQUEST_METHOD "REQUEST_METHOD=GET"
#define REQUEST_PARAMETER "mode=debug"

int main(int argc, char *argv[]) {
    int fd[2];
    if (pipe(fd) < 0) {
        printf("create pipe fail.\n");
    }

    pid_t pid;
    if ((pid = fork()) < 0) {
        printf("fork fail.\n");
    } else if (pid > 0) {
        /* parent */
        //模拟向 CGI 传送数据
        ssize_t length = strlen(REQUEST_PARAMETER);
        /*if (write(fd[1], REQUEST_PARAMETER, length) != length) {
            printf("write error to pipe\n");
        }*/
        close(fd[1]);

        //等待CGI子进程全然把数据读取后写入,
        //实际情况应该是使用select 或者 epoll 监听
        sleep(1);

        //模拟接收 CGI 应答的数据
        char buff[256] = {0};
        length = read(fd[0], buff, sizeof(buff));
        if (length <= 0) {
            printf("read error from pipe\n");
        } else {
            printf("pid %d read data=%u,%s\n", getpid(), length, buff);
        }
        close(fd[0]);

        if (waitpid(pid, NULL, 0) < 0) {
            printf("waitpid error\n");
        }
        exit(0);
    } else {

        chdir("E:\\phpstudy\\PHPTutorial\\WWW\\public\\");
        int logf = open("./log.txt", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
        /* child */
        //重定向管道的输入端到标准输入
        if (fd[0] != STDIN_FILENO) {
            if (dup2(fd[0], STDIN_FILENO) != STDIN_FILENO) {
                printf("dup2 error to stdin");
            }
            close(fd[0]);
        }

        //重定向管道的输出端到标准输出
        if (fd[1] != STDOUT_FILENO) {
            if (dup2(fd[1], STDOUT_FILENO) != STDOUT_FILENO) {
                printf("dup2 error to stdout");
            }
            close(fd[1]);
        }

        //覆盖进程空间,设置CGI环境变量
        char content_length[128] = {0};
        char *exec_envp[7] = {"DOCUMENT_ROOT=E:\\phpstudy\\PHPTutorial\\WWW\\public\\", "SCRIPT_NAME=debug.php",
                              "SCRIPT_FILENAME=E:\\phpstudy\\PHPTutorial\\WWW\\public\\debug.php"
                              REQUEST_METHOD, "REDIRECT_STATUS=200", "SERVER_PROTOCOL=HTTP/1.1", NULL};
        char *exec_argv[5] = {"php-cgi.exe", "-f", "debug.php", NULL};

        if (execve(CGI_NAME, exec_argv, exec_envp) < 0) {
            printf("execl error for %s", strerror(errno));
        }
        exit(0);
    }
    exit(0);
}