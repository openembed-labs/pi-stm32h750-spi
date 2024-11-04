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

int test_main(int spi_fd)
{
    unsigned char recv_data[SEND_DATA_SIZE] = {0};

    unsigned char data_to_send[SEND_DATA_SIZE] = {DEVICE_RS485_1, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};

    // 正式数据传输步骤
    for (int i = 0; i < 6; i++)
    {
        data_to_send[1] += 1;

        log_info("Sending  data, iteration %d:", i + 1);
        print_hex(data_to_send, sizeof(data_to_send));

        // 清空接收数据
        memset(recv_data, 0, sizeof(recv_data));

        if (spi_transfer_full_duplex(spi_fd, data_to_send, recv_data, sizeof(data_to_send)) < 0)
        {
            spi_close(spi_fd);
            return -1;
        }

        log_info("Received data, iteration %d:", i + 1);
        print_hex(recv_data, sizeof(recv_data));

        // 延迟500毫秒
        usleep(500000);
    }

    return 0;
}