#ifndef GPIO_CONTROL_H
#define GPIO_CONTROL_H

// 导出 GPIO 引脚
void export_gpio(const char *gpio);

// 设置 GPIO 引脚的方向 ("in" 或 "out")
void set_gpio_direction(const char *gpio, const char *direction);

// 设置 GPIO 引脚的值 ("0" 或 "1")
void set_gpio_value(const char *gpio, const char *value);

// 初始化 GPIO18：导出、设置为输出并拉低
void initialize_gpio18();

#endif // GPIO_CONTROL_H
