#ifndef UART_H
#define UART_H

/**
 * @file    uart.h
 * @brief   UART串口通信模块头文件
 *
 * 提供基本的串口发送函数：单字节、字符串、缓冲区
 */

#include "ti_msp_dl_config.h"

/* 发送一个以'\0'结尾的字符串 */
void UART_send_string(UART_Regs *uart, const char *str);

/* 发送一个字节数据（阻塞方式） */
void UART_send_char(UART_Regs *uart, const uint8_t chr);

/* 发送指定长度的数据缓冲区 */
void UART_send_buffer(UART_Regs *uart, const uint8_t *buffer, size_t len);

#endif /* UART_H */
