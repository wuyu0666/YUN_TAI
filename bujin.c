#include "GC.h"

/* 电机正向持续标志：PB9按住=1，松开=0 */
volatile uint8_t motor1_forward_active = 0;

/* 电机反向持续标志：PB8按住=1，松开=0 */
volatile uint8_t motor1_reverse_active = 0;

void key6_init();

/**
 * @brief  驱动步进电机滚动指定角度
 * @param  roll  滚动角度值，正值为正向旋转，负值为反向旋转
 *
 * 该函数将角度值转换为编码器脉冲数，通过UART发送指令给DCC驱动器。
 * 数据帧格式：包头(AA AA AA) + 命令帧(包含方向、角度、校验和)
 */
void dianji_roll(int16_t roll)
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
    DCC_v1_2[8]=((uint16_t)(jiaodu*roll))&0xFF;         /* 脉冲数低字节 */
    DCC_v1_2[9]=(((uint16_t)(jiaodu*roll))>>8)&0xFF;    /* 脉冲数高字节 */
    DCC_v1_2[5]=fangxiang;                  /* 方向字节 */
    /* 计算校验和：对协议帧[2]~[9]求和 */
    uint8_t check_sum=0;
    for(int i=2;i<10;i++)
    {
        check_sum+=DCC_v1_2[i];
    }
        
    DCC_v1_2[10]=check_sum;                 /* 校验和填入帧尾 */
    uint8_t wakeup[] = {0xAA, 0xAA, 0xAA};  /* 唤醒序列：3字节AA */
    UART_send_buffer(DCC101_v1_2_INST, wakeup, 3);   /* 发送唤醒脉冲 */
    delay_ms(20);                            /* 等待驱动器就绪 */
    UART_send_buffer(DCC101_v1_2_INST, DCC_v1_2,DCC_v1_2[4]+6);  /* 发送协议帧 */

}

void dianji1_pulse(int32_t pulse)
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
    DCC_v1_2[8]=pulse&0xFF;
    DCC_v1_2[9]=(pulse>>8)&0xFF;
    DCC_v1_2[5]=fangxiang;
    uint8_t check_sum=0;
    for(int i=2;i<10;i++)
    {
        check_sum+=DCC_v1_2[i];
    }
    DCC_v1_2[10]=check_sum;
    uint8_t wakeup[] = {0xAA, 0xAA, 0xAA};
    UART_send_buffer(DCC101_v1_2_INST, wakeup, 3);
    delay_ms(20);
    UART_send_buffer(DCC101_v1_2_INST, DCC_v1_2,DCC_v1_2[4]+6);
}

void dianji_roll_pulse(uint16_t pulse, uint8_t dir)
{
    DCC_v1_2[0]=0xAA; DCC_v1_2[1]=0x55; DCC_v1_2[2]=0x01;
    DCC_v1_2[3]=0x11; DCC_v1_2[4]=0x05; DCC_v1_2[5]=dir;
    DCC_v1_2[6]=0x00; DCC_v1_2[7]=0x00;
    DCC_v1_2[8]=pulse&0xFF; DCC_v1_2[9]=(pulse>>8)&0xFF;
    uint8_t cs=0; for(int i=2;i<10;i++) cs+=DCC_v1_2[i];
    DCC_v1_2[10]=cs;
    uint8_t wu[]={0xAA,0xAA,0xAA};
    UART_send_buffer(DCC101_v1_2_INST,wu,3); delay_ms(20);
    UART_send_buffer(DCC101_v1_2_INST,DCC_v1_2,DCC_v1_2[4]+6);
}

/**
 * @brief  定时器PWM中断处理（2路按键电平消抖扫描）
 *
 * PB9(anjian6) → 电平触发 → motor1_forward_active  (电机正向，按住持续)
 * PB8(fan1)    → 电平触发 → motor1_reverse_active  (电机反向，按住持续)
 */
void DCC_PWM_INST_IRQHandler()
{
    /* ---- 按键6 (PB9) 电机正向 电平消抖 ---- */
    static uint8_t  btn_stable = 0;
    static uint8_t  btn_cnt    = 0;
    /* ---- 按键8 (PB8) 电机反向 电平消抖 ---- */
    static uint8_t  btn3_stable = 0;
    static uint8_t  btn3_cnt    = 0;

    switch (DL_Timer_getPendingInterrupt(DCC_PWM_INST))
    {
        case DL_TIMER_IIDX_LOAD:
        {
            /* ====== 按键6 (PB9) 电机正向 ====== */
            uint8_t key_6 = get_key_state(anjian6_PORT, anjian6_PIN_0_PIN);
            if (key_6 == btn_stable) {
                btn_cnt = 0;
            } else {
                btn_cnt++;
                if (btn_cnt >= 15) {
                    btn_stable = key_6;
                    btn_cnt    = 0;
                }
            }
            motor1_forward_active = btn_stable;

            /* ====== 按键8 (PB8) 电机反向 ====== */
            uint8_t key_8 = get_key_state(fan1_PORT, fan1_PIN_1_PIN);
            if (key_8 == btn3_stable) {
                btn3_cnt = 0;
            } else {
                btn3_cnt++;
                if (btn3_cnt >= 15) {
                    btn3_stable = key_8;
                    btn3_cnt    = 0;
                }
            }
            motor1_reverse_active = btn3_stable;

            break;
        }

        default:
            break;
    }
}
