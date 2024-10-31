#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include "spi.h"
#include "logger.h"

/**
 * @brief SPI initialization function
 *
 * @param spi_path
 * @param mode
 * @param speed
 * @return int
 */
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

/**
 * @brief SPI data transfer function
 *
 * @param spi_fd
 * @param send_data
 * @param recv_data
 * @param len
 * @param speed
 * @return int
 */
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

    log_info("SPI data transfer successful");
    return 0;
}

// SPI close function
void spi_close(int spi_fd)
{
    close(spi_fd);
    log_info("SPI device closed");
}

// Main function
int spi_main()
{
    int spi_fd;
    uint8_t send_data[4] = {0x01, 0x02, 0x03, 0x04};
    uint8_t recv_data[4] = {0};

    spi_fd = spi_init(SPI_PATH, SPI_MODE, SPI_SPEED);
    if (spi_fd < 0)
    {
        return -1;
    }

    for (int i = 0; i < 5; i++)
    {
        // Update send data
        for (int j = 0; j < 4; j++)
        {
            send_data[j] += i;
        }

        log_info("Sending data, iteration %d:", i + 1);
        print_hex(send_data, sizeof(send_data));

        // Perform SPI transfer
        if (spi_transfer_data(spi_fd, send_data, recv_data, sizeof(send_data), SPI_SPEED) < 0)
        {
            spi_close(spi_fd);
            return -1;
        }

        // Print received data
        log_info("Received data, iteration %d:", i + 1);
        print_hex(recv_data, sizeof(recv_data));

        // Delay 500 ms
        usleep(500000);
    }

    // Close SPI
    spi_close(spi_fd);
    return 0;
}
