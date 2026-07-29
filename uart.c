#include "GC.h"

/**
 * @brief  阻塞方式发送一个字节
 * @param  uart  UART外设寄存器基地址
 * @param  chr   待发送的字节数据
 */
void UART_send_char(UART_Regs *uart, const uint8_t chr)
{
    DL_UART_transmitDataBlocking(uart, chr);
}

/**
 * @brief  发送一个以'\0'结尾的字符串
 * @param  uart  UART外设寄存器基地址
 * @param  str   待发送的字符串指针
 */
void UART_send_string(UART_Regs *uart, const char *str)
{
    while (*str) {
        UART_send_char(uart, (uint8_t) *str);
        str++;
    }
}

/**
 * @brief  发送指定长度的数据缓冲区
 * @param  uart    UART外设寄存器基地址
 * @param  buffer  数据缓冲区指针
 * @param  len     待发送的字节数
 */
void UART_send_buffer(UART_Regs *uart, const uint8_t *buffer, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        UART_send_char(uart, buffer[i]);
    }
}

/**
 * @brief  K230 视觉数据接收中断处理
 *
 * 帧格式：AA 55 [dir_x] [pulse_x_L] [pulse_x_H]（共5字节）
 * 帧头校验后解析方向与脉冲，置位 bujin_x 通知主循环消费。
 */
#define FRAME_LEN 5
static uint8_t rx_buf[FRAME_LEN];
static uint8_t rx_idx = 0;

void K230_INST_IRQHandler(void)
{
    switch (DL_UART_getPendingInterrupt(K230_INST))
    {
        case DL_UART_IIDX_RX:
        {
            uint8_t byte = DL_UART_receiveData(K230_INST);
            rx_buf[rx_idx++] = byte;

            if (rx_idx == 1 && rx_buf[0] != 0xAA) 
            {
                rx_idx = 0;
            }
            if (rx_idx == 2 && rx_buf[1] != 0x55) 
            {
                rx_idx = 0;
            }
            if (rx_idx == FRAME_LEN) 
            {
                dir_x   = rx_buf[2];
                pulse_x = rx_buf[3] | ((uint16_t)rx_buf[4] << 8);
                bujin_x = 1;

                rx_idx = 0;
            }
            break;
        }
        default:
            break;
    }
}
