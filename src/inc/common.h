#ifndef COMMON_H
#define COMMON_H

#define SEND_DATA_SIZE 9

#define DEVICE_RS485_1 0x01
#define DEVICE_RS485_2 0x02
#define DEVICE_RS485_3 0x03
#define DEVICE_RS485_4 0x04

#define DEVICE_RS232_1 0x05
#define DEVICE_RS232_2 0x06

#define DEVICE_CAN_1 0x07
#define DEVICE_CAN_2 0x08

#define DEVICE_DI 0x09
#define DEVICE_DO 0x0A

#define DEVICE_DO_1 0x0B
#define DEVICE_DO_2 0x0C
#define DEVICE_DO_3 0x0D
#define DEVICE_DO_4 0x0E
#define DEVICE_DO_5 0x0F
#define DEVICE_DO_6 0x10
#define DEVICE_DO_7 0x11
#define DEVICE_DO_8 0x12
#define DEVICE_DO_9 0x13
#define DEVICE_DO_10 0x14

#define DEVICE_DI_1 0x15
#define DEVICE_DI_2 0x16
#define DEVICE_DI_3 0x17
#define DEVICE_DI_4 0x18
#define DEVICE_DI_5 0x19
#define DEVICE_DI_6 0x1A
#define DEVICE_DI_7 0x1B
#define DEVICE_DI_8 0x1C
#define DEVICE_DI_9 0x1D
#define DEVICE_DI_10 0x1E

extern const int device_do_ids[10];

extern const int device_di_ids[10];

#endif
