#ifndef SPI_H
#define SPI_H

#include <stdint.h>
#include <stddef.h>

#define SEND_DATA_SIZE 256        // 发送数据大小
#define SPI_PATH "/dev/spidev1.2" // SPI 设备文件
#define SPI_MODE SPI_MODE_0
#define SPI_SPEED 500000

int spi_main();

int spi_init(const char *spi_path, uint8_t mode, uint32_t speed);

int spi_transfer_data(int spi_fd, uint8_t *send_data, uint8_t *recv_data, size_t len, uint32_t speed);

int spi_transfer_full_duplex(int spi_fd, unsigned char *send_data, unsigned char *recv_data, size_t len);

void spi_close(int spi_fd);

#endif // SPI_H
