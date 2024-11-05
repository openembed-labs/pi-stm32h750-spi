#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include "common.h"
#include "spi.h"
#include "logger.h"

// 校验接收到的数据中的设备ID是否为期望的设备ID
int valid_device_id(const unsigned char *recv_data, int expected_sender_id)
{
    if (recv_data == NULL)
    {
        log_error("Received data is NULL");
        return -1; // 错误：接收到的数据为空
    }

    // 校验接收到的设备ID是否匹配
    if (recv_data[0] != expected_sender_id)
    {
        log_error("Received data from unexpected device. Expected: %02X, but got: %02X", expected_sender_id, recv_data[0]);
        return -1; // 校验失败
    }

    return 0; // 校验成功
}

// 校验数据是否一致（跳过设备ID），统一的校验逻辑
int valid_data(const unsigned char *send_data, const unsigned char *recv_data, size_t len, int expected_sender_id)
{
    if (send_data == NULL || recv_data == NULL)
    {
        log_error("Invalid pointers: send_data or recv_data is NULL");
        return -1; // 错误：无效指针
    }

    if (len == 0)
    {
        log_error("Data length is zero");
        return -1; // 错误：数据长度为零
    }

    // 校验设备ID是否匹配
    if (valid_device_id(recv_data, expected_sender_id) != 0)
    {
        exit(0);
        return -1; // 如果设备ID校验失败，直接返回错误
    }

    // 从第二个字节开始比较，跳过设备号（首位）
    if (memcmp(send_data + 1, recv_data + 1, len - 1) != 0)
    {
        log_error("Data mismatch between sent and received data");
        exit(0);
        return -1; // 数据不一致，返回失败
    }

    return 0; // 数据一致，校验成功
}

int test_main(int spi_fd)
{
    unsigned char recv_data[SEND_DATA_SIZE] = {0};

    unsigned char data_to_send[SEND_DATA_SIZE] = {DEVICE_RS485_1, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};

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

        // 校验数据（包括设备ID和内容）
        if (valid_data(data_to_send, recv_data, sizeof(data_to_send), DEVICE_RS485_2) != 0)
        {
            // spi_close(spi_fd);
            // return -1; // 如果校验失败，返回错误
        }

        // 延迟500毫秒
        usleep(500000);
    }

    return 0;
}