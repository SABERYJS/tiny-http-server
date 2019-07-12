#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

#define    CGI_NAME "E:\\PHPTutorial\\php\\php-7.2.1-nts\\php-cgi.exe"
#define    REQUEST_METHOD "REQUEST_METHOD=POST"
#define REQUEST_PARAMETER "myname=huangzhihui"

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
        if (write(fd[1], REQUEST_PARAMETER, length) != length) {
            printf("write error to pipe\n");
        }
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
        sprintf(content_length, "CONTENT_LENGTH=%u", strlen(REQUEST_PARAMETER));
        char *exec_argv[3] = {REQUEST_METHOD, content_length};
        printf("argc:%d\n", argc);
        printf("argv[0] is %s\n", argv[0]);
        if (execve(CGI_NAME, argv + 1, exec_argv) < 0) {
            printf("execl error for %s", CGI_NAME);
        }
        exit(0);
    }
    exit(0);
}