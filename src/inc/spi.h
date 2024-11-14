#ifndef SPI_H
#define SPI_H

#include <stdint.h>
#include <stddef.h>

#define SEND_DATA_SIZE 255        // 发送数据大小，根据实际数据大小调整
#define SPI_PATH "/dev/spidev1.2" // SPI 设备文件
#define SPI_MODE SPI_MODE_0       // SPI模式，根据实际情况调整
#define SPI_SPEED 500000          // SPI速度，根据实际情况调整

int spi_main();

// 初始化 SPI 设备
int spi_init(const char *spi_path, uint8_t mode, uint32_t speed);

// 发送和接收 SPI 数据
int spi_transfer_data(int spi_fd, uint8_t *send_data, uint8_t *recv_data, size_t len, uint32_t speed);

int spi_transfer_full_duplex(int spi_fd, unsigned char *send_data, unsigned char *recv_data, size_t len);

// 关闭 SPI 设备
void spi_close(int spi_fd);

#endif // SPI_H
