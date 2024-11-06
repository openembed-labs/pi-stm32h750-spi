#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include "common.h"
#include "spi.h"
#include "logger.h"
#include "send_test_data.h"

// SPI initialization function
int spi_init(const char *spi_path, uint8_t mode, uint32_t speed)
{
    int spi_fd = open(spi_path, O_RDWR);
    if (spi_fd < 0)
    {
        log_error("Failed to open SPI device %s", spi_path);
        return -1;
    }

    if (ioctl(spi_fd, SPI_IOC_WR_MODE, &mode) < 0)
    {
        log_error("Failed to set SPI mode");
        close(spi_fd);
        return -1;
    }

    if (ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) < 0)
    {
        log_error("Failed to set SPI speed");
        close(spi_fd);
        return -1;
    }

    log_info("SPI initialized successfully, Device: %s, Mode: %d, Speed: %d Hz", spi_path, mode, speed);
    return spi_fd;
}

// SPI data transfer function
int spi_transfer_data(int spi_fd, uint8_t *send_data, uint8_t *recv_data, size_t len, uint32_t speed)
{
    struct spi_ioc_transfer spi_transfer = {
        .tx_buf = (unsigned long)send_data,
        .rx_buf = (unsigned long)recv_data,
        .len = len,
        .speed_hz = speed,
        .bits_per_word = 8,
    };

    if (ioctl(spi_fd, SPI_IOC_MESSAGE(1), &spi_transfer) < 0)
    {
        log_error("SPI data transfer failed");
        return -1;
    }

    return 0;
}

// Full duplex SPI data transfer function
int spi_transfer_full_duplex(int spi_fd, uint8_t *send_data, uint8_t *recv_data, size_t len)
{
    uint8_t first_recv_data[SEND_DATA_SIZE] = {0};
    uint8_t zero_data[SEND_DATA_SIZE] = {0}; // 全零数据，用于只读操作

    // 清空接收缓冲区
    memset(recv_data, 0, len);

    // 第一次发送并接收数据（冗余数据）
    if (spi_transfer_data(spi_fd, send_data, first_recv_data, len, SPI_SPEED) < 0)
    {
        return -1;
    }

    printf("\033[36mFirst_recv_data%d:", 1111);
    print_hex(first_recv_data, len);

    // 50ms
    usleep(500000);

    // 发送全零数据并接收数据（只读有效数据）
    if (spi_transfer_data(spi_fd, zero_data, recv_data, len, SPI_SPEED) < 0)
    {
        return -1;
    }

    // 50ms
    usleep(500000);

    return 0;
}

// SPI close function
void spi_close(int spi_fd)
{
    close(spi_fd);
    log_info("SPI device closed\n");
}

// Main function
int spi_main()
{
    int spi_fd;

    spi_fd = spi_init(SPI_PATH, SPI_MODE, SPI_SPEED);
    if (spi_fd < 0)
    {
        return -1;
    }

    test_main(spi_fd);

    // 关闭SPI
    spi_close(spi_fd);
    return 0;
}
