#include <stdio.h>
#include <string.h>
#include "spi.h"
#include "daemonize.h"
#include "logger.h"

void parse_arguments(int argc, char *argv[]);

void parse_arguments(int argc, char *argv[])
{
    int run_as_daemon = 0;
    for (int i = 1; i < argc; i++) // 从 1 开始，跳过程序名
    {
        if (strcmp(argv[i], "--daemon") == 0)
        {
            run_as_daemon = 1;
            // break; // 找到后就退出循环
        }
    }

    if (run_as_daemon)
    {
        daemonize();
        set_log_mode(LOG_MODE_SYSLOG);
    }
    else
    {
        set_log_mode(LOG_MODE_CONSOLE);
    }
}

int main(int argc, char *argv[])
{
    // parse_arguments(argc, argv);

    if (spi_main() != 0)
    {
        fprintf(stderr, "SPI initialization failed.\n");
        return 1;
    }

    return 0;
}
