#include "logger.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <syslog.h>
#include "daemonize.h"

// ANSI 转义码
#define RESET_COLOR "\033[0m"
#define INFO_COLOR "\033[36m"  // 青色
#define ERROR_COLOR "\033[31m" // 红色

static log_mode_t current_log_mode = LOG_MODE_CONSOLE;

void set_log_mode(log_mode_t mode)
{
    if (current_log_mode != mode)
    {
        if (mode == LOG_MODE_SYSLOG)
        {
            openlog(LOG_IDENT, LOG_PID, LOG_DAEMON);
        }
        else
        {
            closelog();
        }
        current_log_mode = mode;
    }
}

void log_info(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    char buffer[256];
    vsnprintf(buffer, sizeof(buffer), fmt, args);

    if (current_log_mode == LOG_MODE_CONSOLE)
    {
        // 使用绿色打印 info 日志
        printf(INFO_COLOR "%s" RESET_COLOR, buffer);
    }
    else
    {
        syslog(LOG_INFO, "%s", buffer);
    }

    va_end(args);
}

void log_error(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    char buffer[256];
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer) - 1, ": %s", strerror(errno));

    if (current_log_mode == LOG_MODE_CONSOLE)
    {
        // 使用红色打印 error 日志
        fprintf(stderr, ERROR_COLOR "ERROR: %s" RESET_COLOR "\n", buffer);
    }
    else
    {
        syslog(LOG_ERR, "%s", buffer);
    }

    va_end(args);
}

void print_hex(const unsigned char *buf, size_t length)
{
    for (size_t i = 0; i < length; i++)
    {
        printf("%02X ", buf[i]);
    }
    printf("\n");
}

void print_binary(unsigned char byte)
{
    for (int i = 7; i >= 0; i--)
    {
        printf("%d", (byte >> i) & 1);
    }
    printf("\n");
}
