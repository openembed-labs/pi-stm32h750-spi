#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <time.h>
#include <linux/spi/spidev.h>
#include "common.h"
#include "spi.h"
#include "logger.h"

// 设备名称映射
const char *device_names[] = {
    "Unknown", // 0x00
    "RS485_1", // 0x01
    "RS485_2", // 0x02
    "RS485_3", // 0x03
    "RS485_4", // 0x04
    "RS232_1", // 0x05
    "RS232_2", // 0x06
    "CAN_1",   // 0x07
    "CAN_2",   // 0x08
    "DI",      // 0x09
    "DO",      // 0x0A
    "DO_1",    // 0x0B
    "DO_2",    // 0x0C
    "DO_3",    // 0x0D
    "DO_4",    // 0x0E
    "DO_5",    // 0x0F
    "DO_6",    // 0x10
    "DO_7",    // 0x11
    "DO_8",    // 0x12
    "DO_9",    // 0x13
    "DO_10",   // 0x14
    "DI_1",    // 0x15
    "DI_2",    // 0x16
    "DI_3",    // 0x17
    "DI_4",    // 0x18
    "DI_5",    // 0x19
    "DI_6",    // 0x1A
    "DI_7",    // 0x1B
    "DI_8",    // 0x1C
    "DI_9",    // 0x1D
    "DI_10",   // 0x1E
};

// 校验接收到的数据中的设备ID是否为期望的设备ID
int valid_device_id(const uint8_t *recv_data, int expected_sender_id)
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

    return 0;
}

// 校验数据是否一致（跳过设备ID）
int valid_data(const uint8_t *send_data, const uint8_t *recv_data, size_t len, size_t recv_data_len, int expected_sender_id, int is_di_do)
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
    if (recv_data_len != len)
    {
        log_error("Received data length: %zu bytes, expected length: %zu bytes\n", recv_data_len, len);
        return -1; // 长度不匹配，可能数据丢失
    }
    else
    {
        log_info("\033[32mSuccessful, Received data length: %zu bytes, expected length: %zu bytes\n", recv_data_len, len);
    }

    if (send_data[0] == DEVICE_CAN_1 || send_data[0] == DEVICE_CAN_2)
    {
        uint8_t send_data_temp[8] = {0};
        uint8_t recv_data_temp[8] = {0};

        memcpy(send_data_temp, send_data + 1, 8);
        memcpy(recv_data_temp, recv_data + 1, 8);

        if (memcmp(send_data_temp, recv_data_temp, 8) != 0)
        {
            log_error("Data mismatch between sent and received data");
            return -1;
        }
    }
    else
    {
        if (memcmp(send_data + 1, recv_data + 1, len - 1) != 0) // 跳过设备ID（首位）
        {
            log_error("Data mismatch between sent and received data");
            return -1;
        }
    }

    return 0;
}

void send_and_receive(int spi_fd, int expected_sender_id, uint8_t *data_to_send, uint8_t *recv_data, size_t data_len, size_t recv_data_len, int iteration, int is_di_do)
{

    const char *device_name = device_names[data_to_send[0]];

    printf("\n\033[33mSending....iteration %d, device [%s] to [%s]:\033[0m\n", iteration, device_name, device_names[expected_sender_id]);

    // 防止递增到0x00
    if (is_di_do != 1)
    {
        if (data_to_send[1] < 0xFF)
        {
            data_to_send[1] += 1;
        }
        else
        {
            data_to_send[1] = 0x01;
        }
    }

    printf("\033[36mSending  data, iteration %d:", iteration);
    print_hex(data_to_send, data_len);

    if (spi_transfer_full_duplex(spi_fd, data_to_send, recv_data, data_len) < 0)
    {
        log_error("SPI transfer failed");
        return;
    }

    printf("\033[36mReceived data, iteration %d:", iteration);
    print_hex(recv_data, data_len);

    if (valid_data(data_to_send, recv_data, data_len, recv_data_len, expected_sender_id, is_di_do) != 0)
    {
        log_error("Data validation failed");
        exit(0);
        return;
    }
}

void fill_data(uint8_t *data, uint8_t device_id)
{
    data[0] = device_id;

    srand(time(NULL));

    for (int i = 1; i < SEND_DATA_SIZE; i++)
    {
        // 生成1到255之间的随机数
        uint8_t random_value = (uint8_t)(rand() % 255 + 1);
        data[i] = random_value;
    }
}

int test_main(int spi_fd)
{
    uint8_t recv_data[SEND_DATA_SIZE] = {0};

    uint8_t rs485_data_1_to_2[SEND_DATA_SIZE];
    uint8_t rs485_data_2_to_1[SEND_DATA_SIZE];
    uint8_t rs485_data_3_to_4[SEND_DATA_SIZE];
    uint8_t rs485_data_4_to_3[SEND_DATA_SIZE];

    uint8_t rs232_data_1[SEND_DATA_SIZE];
    uint8_t rs232_data_2[SEND_DATA_SIZE];

    uint8_t can_data_1_to_2[SEND_DATA_SIZE];
    uint8_t can_data_2_to_1[SEND_DATA_SIZE];

    fill_data(rs485_data_1_to_2, DEVICE_RS485_1);
    fill_data(rs485_data_2_to_1, DEVICE_RS485_2);
    fill_data(rs485_data_3_to_4, DEVICE_RS485_3);
    fill_data(rs485_data_4_to_3, DEVICE_RS485_4);

    fill_data(rs232_data_1, DEVICE_RS232_1);
    fill_data(rs232_data_2, DEVICE_RS232_2);

    fill_data(can_data_1_to_2, DEVICE_CAN_1);
    fill_data(can_data_2_to_1, DEVICE_CAN_2);

    uint8_t di_do_data[SEND_DATA_SIZE] = {DEVICE_DO_1, 0x01};

    int iteration = 0;

    while (1)
    {
        iteration++;

        send_and_receive(spi_fd, DEVICE_RS485_2, rs485_data_1_to_2, recv_data, sizeof(rs485_data_1_to_2), sizeof(recv_data), iteration, 0);
        send_and_receive(spi_fd, DEVICE_RS485_1, rs485_data_2_to_1, recv_data, sizeof(rs485_data_2_to_1), sizeof(recv_data), iteration, 0);
        send_and_receive(spi_fd, DEVICE_RS485_4, rs485_data_3_to_4, recv_data, sizeof(rs485_data_2_to_1), sizeof(recv_data), iteration, 0);
        send_and_receive(spi_fd, DEVICE_RS485_3, rs485_data_4_to_3, recv_data, sizeof(rs485_data_2_to_1), sizeof(recv_data), iteration, 0);

        send_and_receive(spi_fd, DEVICE_RS232_1, rs232_data_1, recv_data, sizeof(rs232_data_1), sizeof(recv_data), iteration, 0);
        send_and_receive(spi_fd, DEVICE_RS232_2, rs232_data_2, recv_data, sizeof(rs232_data_2), sizeof(recv_data), iteration, 0);

        send_and_receive(spi_fd, DEVICE_CAN_2, can_data_1_to_2, recv_data, sizeof(can_data_1_to_2), sizeof(recv_data), iteration, 0);
        send_and_receive(spi_fd, DEVICE_CAN_1, can_data_2_to_1, recv_data, sizeof(can_data_2_to_1), sizeof(recv_data), iteration, 0);

        //  DO  To DI Cross Test
        for (int i = 0; i < 10; i++)
        {
            di_do_data[0] = DEVICE_DO_1 + i;
            di_do_data[1] = 0x01;
            send_and_receive(spi_fd, DEVICE_DO_1 + i, di_do_data, recv_data, sizeof(di_do_data), sizeof(recv_data), iteration, 1);

            di_do_data[0] = DEVICE_DI_1 + i;
            di_do_data[1] = 0x00;
            send_and_receive(spi_fd, DEVICE_DI_1 + i, di_do_data, recv_data, sizeof(di_do_data), sizeof(recv_data), iteration, 1);
        }

        for (int i = 0; i < 10; i++)
        {
            di_do_data[0] = DEVICE_DO_1 + i;
            di_do_data[1] = 0x00;
            send_and_receive(spi_fd, DEVICE_DO_1 + i, di_do_data, recv_data, sizeof(di_do_data), sizeof(recv_data), iteration, 1);

            di_do_data[0] = DEVICE_DI_1 + i;
            di_do_data[1] = 0x01;
            send_and_receive(spi_fd, DEVICE_DI_1 + i, di_do_data, recv_data, sizeof(di_do_data), sizeof(recv_data), iteration, 1);
        }
    }

    return 0;
}
