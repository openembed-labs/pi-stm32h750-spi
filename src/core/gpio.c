#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "logger.h"

// 定义 GPIO18 的编号
#define GPIO_18 "18"

// 导出 GPIO
void export_gpio(const char *gpio)
{
    int fd = open("/sys/class/gpio/export", O_WRONLY);
    if (fd < 0)
    {
        log_error("Failed to open export for writing");
        return;
    }

    // 写入 GPIO 编号，注意这里要写入 2 个字节
    if (write(fd, gpio, 2) != 2)
    {
        log_error("Error writing to export");
    }
    close(fd);
}

// 设置 GPIO 方向
void set_gpio_direction(const char *gpio, const char *direction)
{
    char path[35];
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%s/direction", gpio);
    int fd = open(path, O_WRONLY);
    if (fd < 0)
    {
        log_error("Failed to open gpio direction");
        return;
    }

    // 写入方向，确保写入的字节数正确
    if (write(fd, direction, 3) != 3)
    {
        log_error("Error writing direction");
    }
    close(fd);
}

// 设置 GPIO 电平
void set_gpio_value(const char *gpio, const char *value)
{
    char path[30];
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%s/value", gpio);
    int fd = open(path, O_WRONLY);
    if (fd < 0)
    {
        log_error("Failed to open gpio value");
        return;
    }

    // 写入值，确保写入的字节数正确
    if (write(fd, value, 1) != 1)
    {
        log_error("Error writing value");
    }
    close(fd);
}

// 初始化 GPIO18
void initialize_gpio18()
{
    // 导出 GPIO18
    export_gpio(GPIO_18);

    // 设置 GPIO18 为输出模式
    set_gpio_direction(GPIO_18, "out");

    // 设置 GPIO18 为低电平
    set_gpio_value(GPIO_18, "0");

    log_info("GPIO18 has been set to LOW.\n");
}
