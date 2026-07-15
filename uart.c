#include "uart.h"

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
 * @brief  串口打印中断处理（UART RX回显）
 *
 * 接收一个字节后立即回发，实现简单的串口回显功能。
 */
void PRINT_INST_IRQHandler()
{
    switch (DL_UART_getPendingInterrupt(PRINT_INST))
    {
    case DL_UART_IIDX_RX:
        {   
            uint8_t rec = DL_UART_receiveData(PRINT_INST);  /* 接收一个字节 */
            UART_send_char(PRINT_INST, rec);                /* 回显发送 */
            break;
        }
    
    default:
        break;
    }
}

