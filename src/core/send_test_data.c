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

// 设备名称映射
const char *device_names[] = {
    "Unknown", "RS485_1", "RS485_2", "RS485_3", "RS485_4", "RS232_1", "RS232_2", "CAN_1",
    "CAN_2", "DI", "DO", "DO_1", "DO_2", "DO_3", "DO_4", "DO_5", "DO_6", "DO_7", "DO_8",
    "DO_9", "DO_10", "DI_1", "DI_2", "DI_3", "DI_4", "DI_5", "DI_6", "DI_7", "DI_8", "DI_9", "DI_10"};

// 校验接收到的数据中的设备ID是否为期望的设备ID
int valid_device_id(const unsigned char *recv_data, int expected_sender_id)
{
    if (recv_data == NULL)
    {
        log_error("Received data is NULL");
        return -1;
    }

    if (recv_data[0] != expected_sender_id)
    {
        log_error("Received data from unexpected device. Expected: %02X, but got: %02X", expected_sender_id, recv_data[0]);
        return -1;
    }

    return 0; // 校验成功
}

// 校验数据是否一致（跳过设备ID）
int valid_data(const unsigned char *send_data, const unsigned char *recv_data, size_t len, size_t recv_data_len, int expected_sender_id)
{
    if (send_data == NULL || recv_data == NULL)
    {
        log_error("Invalid pointers: send_data or recv_data is NULL");
        return -1;
    }

    if (len == 0)
    {
        log_error("Data length is zero");
        return -1;
    }

    // 校验设备ID是否一致
    if (valid_device_id(recv_data, expected_sender_id) != 0)
    {
        return -1; // 设备ID校验失败
    }

    // 校验接收到的数据长度和发送的数据长度是否一致
    if (recv_data_len != len) // 比较接收到的数据的实际长度和预期的长度
    {
        log_error("Received data length: %zu bytes, expected length: %zu bytes\n", recv_data_len, len);
        return -1; // 长度不匹配，可能数据丢失
    }
    else
    {
        log_info("\033[32mReceived data length: %zu bytes, expected length: %zu bytes\n", recv_data_len, len);
    }

    // 比较发送和接收的数据内容（跳过设备ID）
    if (memcmp(send_data + 1, recv_data + 1, len - 1) != 0) // 跳过设备ID（首位）
    {
        log_error("Data mismatch between sent and received data");
        return -1;
    }

    return 0; // 数据一致，校验成功
}

// 发送和接收数据的核心函数
void send_and_receive(int spi_fd, int expected_sender_id, unsigned char *data_to_send, unsigned char *recv_data, size_t data_len, size_t recv_data_len, int iteration)
{

    const char *device_name = device_names[data_to_send[0]];

    printf("\n\033[33mSending....iteration %d, device [%s] to [%s]:\033[0m\n", iteration + 1, device_name, device_names[expected_sender_id]);

    data_to_send[1] += 1; // 增加数据

    printf("\033[36mSending  data, iteration %d:", iteration + 1);
    print_hex(data_to_send, data_len);

    // 清空接收数据
    memset(recv_data, 0, data_len);

    // 执行 SPI 数据传输
    if (spi_transfer_full_duplex(spi_fd, data_to_send, recv_data, data_len) < 0)
    {
        log_error("SPI transfer failed");
        return;
    }

    printf("\033[36mReceived data, iteration %d:", iteration + 1);
    print_hex(recv_data, data_len);

    // 校验数据
    // if (valid_data(data_to_send, recv_data, data_len, recv_data_len, expected_sender_id) != 0)
    // {
    //     log_error("Data validation failed");
    //     exit(0);
    //     return;
    // }

    // 延迟500毫秒
    usleep(500000);
}

// 主测试函数
int test_main(int spi_fd)
{
    unsigned char recv_data[SEND_DATA_SIZE] = {0};

    // 数据构造：从设备 RS485_1 发送到设备 RS485_2，设备 RS485_3 和 RS485_4 也需要进行相互测试
    unsigned char rs485_data_1_to_2[SEND_DATA_SIZE] = {DEVICE_RS485_1, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
    // unsigned char rs485_data_2_to_1[SEND_DATA_SIZE] = {DEVICE_RS485_2, 0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02};
    // unsigned char rs485_data_3_to_4[SEND_DATA_SIZE] = {DEVICE_RS485_3, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47};
    // unsigned char rs485_data_4_to_3[SEND_DATA_SIZE] = {DEVICE_RS485_4, 0x47, 0x46, 0x45, 0x44, 0x43, 0x42, 0x41, 0x40};

    // // 数据构造：从设备 RS232_1 发送到设备 RS232_2
    // unsigned char rs232_data_1_to_2[SEND_DATA_SIZE] = {DEVICE_RS232_1, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28};
    // unsigned char rs232_data_2_to_1[SEND_DATA_SIZE] = {DEVICE_RS232_2, 0x28, 0x27, 0x26, 0x25, 0x24, 0x23, 0x22, 0x21};

    // // 数据构造：从设备 CAN_1 发送到设备 CAN_2
    // unsigned char can_data_1_to_2[SEND_DATA_SIZE] = {DEVICE_CAN_1, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18};
    // unsigned char can_data_2_to_1[SEND_DATA_SIZE] = {DEVICE_CAN_2, 0x18, 0x17, 0x16, 0x15, 0x14, 0x13, 0x12, 0x11};

    for (int i = 0; i < 6; i++)
    {
        send_and_receive(spi_fd, DEVICE_RS485_2, rs485_data_1_to_2, recv_data, sizeof(rs485_data_1_to_2), sizeof(recv_data), i);
        // send_and_receive(spi_fd, DEVICE_RS485_1, rs485_data_2_to_1, recv_data, sizeof(rs485_data_2_to_1), sizeof(recv_data), i);
        // send_and_receive(spi_fd, DEVICE_RS485_4, rs485_data_3_to_4, recv_data, sizeof(rs485_data_2_to_1), sizeof(recv_data), i);
        // send_and_receive(spi_fd, DEVICE_RS485_3, rs485_data_4_to_3, recv_data, sizeof(rs485_data_2_to_1), sizeof(recv_data), i);

        // send_and_receive(spi_fd, DEVICE_RS232_1, rs232_data_1_to_2, recv_data, sizeof(rs232_data_1_to_2), sizeof(recv_data), i);
        // send_and_receive(spi_fd, DEVICE_RS232_2, rs232_data_2_to_1, recv_data, sizeof(rs232_data_2_to_1), sizeof(recv_data), i);

        // send_and_receive(spi_fd, DEVICE_CAN_2, can_data_1_to_2, recv_data, sizeof(can_data_1_to_2), sizeof(recv_data), i);
        // send_and_receive(spi_fd, DEVICE_CAN_1, can_data_2_to_1, recv_data, sizeof(can_data_2_to_1), sizeof(recv_data), i);
    }

    return 0;
}
