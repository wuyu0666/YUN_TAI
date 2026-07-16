#include "GC.h"
// extern volatile uint8_t dir_x=0;
// extern volatile uint16_t pulse_x=0;
// extern volatile uint8_t dir_y=0;
// extern volatile uint16_t pulse_y=0;
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
// void PRINT_INST_IRQHandler()
// {
//     switch (DL_UART_getPendingInterrupt(PRINT_INST))
//     {
//     case DL_UART_IIDX_RX:
//         {   
//             uint8_t rec = DL_UART_receiveData(PRINT_INST);  /* 接收一个字节 */
//             UART_send_char(PRINT_INST, rec);                /* 回显发送 */
//             break;
//         }
    
//     default:
//         break;
//     }
// }

#define FRAME_LEN 8
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

            // 帧头检测：非 0xAA 开头则重置
            if (rx_idx == 1 && rx_buf[0] != 0xAA) 
            {
                rx_idx = 0;
            }
        // 第二个字节必须是 0x55
            if (rx_idx == 2 && rx_buf[1] != 0x55) 
            {
                rx_idx = 0;
            }
        // 收满 8 字节 → 解析
            if (rx_idx == FRAME_LEN) 
            {
                dir_x    = rx_buf[2];
                pulse_x = rx_buf[3] | ((uint16_t)rx_buf[4] << 8);
                bujin_x=1;
                dir_y    = rx_buf[5];
                pulse_y = rx_buf[6] | ((uint16_t)rx_buf[7] << 8);
                bujin_y=1;

                
                rx_idx = 0;   // 复位，准备下一帧
            }
            break;
    }
    default:
        break;
    }
}