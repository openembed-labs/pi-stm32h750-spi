#include <stdio.h>
#include <string.h>
#include "spi.h"
#include "daemonize.h"
#include "logger.h"
#include "gpio.h"

void parse_arguments(int argc, char *argv[]);

void parse_arguments(int argc, char *argv[])
{
    for (size_t i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "--daemon") == 0)
        {
            daemonize();
            set_log_mode(LOG_MODE_SYSLOG);
        }
    }
}

int main(int argc, char *argv[])
{

    initialize_gpio18();

    spi_main();

    return 0;
}
