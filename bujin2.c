#include "GC.h"

/* 电机2正向持续标志：PA16按住=1，松开=0 */
volatile uint8_t motor2_forward_active = 0;

/* 电机2反向持续标志：PA15按住=1，松开=0 */
volatile uint8_t motor2_reverse_active = 0;

/**
 * @brief  驱动步进电机2滚动指定角度
 * @param  roll  滚动角度值，正值为正向旋转，负值为反向旋转
 *
 * 该函数将角度值转换为编码器脉冲数，通过UART1(PB6/PB7)发送指令给DCC驱动器。
 * 数据帧格式：包头(AA AA AA) + 命令帧(包含方向、角度、校验和)
 */
void dianji2_roll(int16_t roll)
{
    uint8_t fangxiang=0x01;                 /* 方向标志：0x00反转，0x01正转 */
    if(roll<=0)
    {
        roll=-roll;                         /* 取绝对值 */
        fangxiang=0x00;                     /* 负角度 -> 反向旋转 */
    }
    else
    {
        fangxiang=0x01;                     /* 正角度 -> 正向旋转 */
    }
    /* 将角度转换为编码器脉冲数并填入协议帧 */
    DCC_v1_3[8]=(jiaodu2*roll)&0xFF;        /* 脉冲数低字节 */
    DCC_v1_3[9]=((jiaodu2*roll)>>8)&0xFF;   /* 脉冲数高字节 */
    DCC_v1_3[5]=fangxiang;                  /* 方向字节 */
    /* 计算校验和：对协议帧[2]~[9]求和 */
    uint8_t check_sum=0;
    for(int i=2;i<10;i++)
    {
        check_sum+=DCC_v1_3[i];
    }
        
    DCC_v1_3[10]=check_sum;                 /* 校验和填入帧尾 */
    uint8_t wakeup[] = {0xAA, 0xAA, 0xAA};  /* 唤醒序列：3字节AA */
    UART_send_buffer(DCC101_v1_3_INST, wakeup, 3);   /* 发送唤醒脉冲 */
    delay_ms(20);                            /* 等待驱动器就绪 */
    UART_send_buffer(DCC101_v1_3_INST, DCC_v1_3,DCC_v1_3[4]+6);  /* 发送协议帧 */
}

void dianji2_pulse(int32_t pulse)
{
    if (pulse > 65535) pulse = 65535;
    if (pulse < -65535) pulse = -65535;
    uint8_t fangxiang=0x01;
    if(pulse<=0)
    {
        pulse=-pulse;
        fangxiang=0x00;
    }
    else
    {
        fangxiang=0x01;
    }
    DCC_v1_3[8]=pulse&0xFF;
    DCC_v1_3[9]=(pulse>>8)&0xFF;
    DCC_v1_3[5]=fangxiang;
    uint8_t check_sum=0;
    for(int i=2;i<10;i++)
    {
        check_sum+=DCC_v1_3[i];
    }
    DCC_v1_3[10]=check_sum;
    uint8_t wakeup[] = {0xAA, 0xAA, 0xAA};
    UART_send_buffer(DCC101_v1_3_INST, wakeup, 3);
    delay_ms(20);
    UART_send_buffer(DCC101_v1_3_INST, DCC_v1_3,DCC_v1_3[4]+6);
}
