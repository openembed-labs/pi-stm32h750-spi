#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <syslog.h>
#include <signal.h>
#include "daemonize.h"

void daemonize()
{
    pid_t pid, sid;
    int fd;

    // 创建子进程
    pid = fork();
    if (pid < 0)
    {
        exit(EXIT_FAILURE);
    }
    // 终止父进程
    if (pid > 0)
    {
        exit(EXIT_SUCCESS);
    }

    // 在子进程中创建新的会话
    sid = setsid();
    if (sid < 0)
    {
        exit(EXIT_FAILURE);
    }

    // 改变工作目录
    if (chdir("/") < 0)
    {
        exit(EXIT_FAILURE);
    }

    // 重设文件权限掩码
    umask(0);

    // 关闭所有文件描述符
    for (fd = sysconf(_SC_OPEN_MAX); fd >= 0; fd--)
    {
        close(fd);
    }

    // 重定向标准输入、输出和错误输出
    fd = open("/dev/null", O_RDWR);
    if (fd != -1)
    {
        dup2(fd, STDIN_FILENO);
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);
        if (fd > STDERR_FILENO)
        {
            close(fd);
        }
    }

    // 打开系统日志
    openlog(LOG_IDENT, LOG_PID, LOG_DAEMON);
    syslog(LOG_INFO, "Daemon started");
}
