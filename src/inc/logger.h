#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <syslog.h>

typedef enum
{
    LOG_MODE_CONSOLE,
    LOG_MODE_SYSLOG
} log_mode_t;

void set_log_mode(log_mode_t mode);
void log_info(const char *fmt, ...);
void log_error(const char *fmt, ...);
void print_hex(const unsigned char *buf, size_t length);
void print_byte_binary(uint8_t byte);
void print_binary_arr(uint8_t *array, size_t size);

#endif // LOGGER_H
