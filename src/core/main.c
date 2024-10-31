#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

#define SPI_PATH "/dev/spidev1.2" // SPI 设备文件
#define SPI_SPEED 50000           // SPI 通信速率
#define SPI_MODE SPI_MODE_0       // SPI 模式

int main()
{
    int spi_fd;
    uint8_t send_data[4] = {0x01, 0x02, 0x03, 0x04}; // 初始发送的四个字节
    uint8_t recv_data[4] = {0};                      // 用于接收的数据
    struct spi_ioc_transfer spi_transfer = {0};

    // 打开 SPI 设备
    spi_fd = open(SPI_PATH, O_RDWR);
    if (spi_fd < 0)
    {
        perror("无法打开 SPI 设备");
        return -1;
    }

    // 定义 SPI 模式和速率变量
    uint8_t mode = SPI_MODE;
    uint32_t speed = SPI_SPEED;

    // 设置 SPI 模式
    if (ioctl(spi_fd, SPI_IOC_WR_MODE, &mode) < 0)
    {
        perror("无法设置 SPI 模式");
        close(spi_fd);
        return -1;
    }

    // 设置 SPI 通信速率
    if (ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) < 0)
    {
        perror("无法设置 SPI 速率");
        close(spi_fd);
        return -1;
    }

    // 配置传输结构体
    spi_transfer.tx_buf = (unsigned long)send_data;
    spi_transfer.rx_buf = (unsigned long)recv_data;
    spi_transfer.len = sizeof(send_data); // 传输的字节长度
    spi_transfer.speed_hz = speed;
    spi_transfer.bits_per_word = 8;

    // 发送和接收数据 5 次
    for (int i = 0; i < 5; i++)
    {
        // 更新发送数据：每个字节增加 i
        for (int j = 0; j < 4; j++)
        {
            send_data[j] += i;
        }

        printf("第 %d 次发送数据: ", i + 1);
        for (int j = 0; j < 4; j++)
        {
            printf("0x%02X ", send_data[j]);
        }
        printf("\n");

        // 发送和接收数据
        if (ioctl(spi_fd, SPI_IOC_MESSAGE(1), &spi_transfer) < 0)
        {
            perror("SPI 数据传输失败");
            close(spi_fd);
            return -1;
        }

        // 打印接收到的数据
        printf("第 %d 次接收到的数据: ", i + 1);
        for (int j = 0; j < 4; j++)
        {
            printf("0x%02X ", recv_data[j]);
        }
        printf("\n");

        // 延迟一段时间，模拟不同的发送间隔
        usleep(500000); // 延迟 500 毫秒
    }

    // 关闭 SPI 设备
    close(spi_fd);

    return 0;
}
