#include "GC.h"

/* 电机正向持续标志：PB9或PA16按住=1，松开=0 */
volatile uint8_t motor1_forward_active = 0;

/* 电机反向持续标志：PB8或PA15按住=1，松开=0 */
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
 * @brief  设置当前位置为原点（较零），CMD 0x12
 *
 * 将电机当前位置清零，设为绝对坐标系原点(0度)，
 * 消除多圈累加偏移，自动保存至Flash。
 */
void dianji_set_origin(void)
{
    DCC_v1_2[0]=0xAA; DCC_v1_2[1]=0x55; DCC_v1_2[2]=0x01;
    DCC_v1_2[3]=0x12; DCC_v1_2[4]=0x00;
    uint8_t check_sum=0;
    for(int i=2;i<5;i++) check_sum+=DCC_v1_2[i];
    DCC_v1_2[5]=check_sum;
    uint8_t wakeup[]={0xAA,0xAA,0xAA};
    UART_send_buffer(DCC101_v1_2_INST,wakeup,3); delay_ms(20);
    UART_send_buffer(DCC101_v1_2_INST,DCC_v1_2,6);
    motor_angle = 0;
    motor_pulse = 0;
}

/**
 * @brief  绝对旋转到指定角度（相对于原点），CMD 0x13
 * @param  angle  目标角度，正值为正向，负值为反向
 *
 * 控制电机旋转到相对于原点的绝对坐标位置。
 * 角度转换为编码器脉冲数(16384/圈)，通过UART发送0x13指令。
 */
void dianji_rotate_to(int16_t angle)
{
    /* 目标角度归一化到 [0, 360) */
    int16_t target = angle % 360;
    if(target < 0) target += 360;

    /* 当前角度归一化到 [0, 360) */
    int16_t current = motor_angle % 360;
    if(current < 0) current += 360;

    /* 计算最短路径差值 */
    int16_t diff = target - current;
    uint8_t dir;
    if(diff > 180)
        dir = 0x00;       /* 反向走更短 */
    else if(diff < -180)
        dir = 0x01;       /* 正向走更短 */
    else if(diff >= 0)
        dir = 0x01;       /* 正向 */
    else
        dir = 0x00;       /* 反向 */

    /* 目标脉冲数（绝对坐标，始终为正） */
    uint16_t pulse = (uint16_t)(jiaodu * (float)target);

    DCC_v1_2[0]=0xAA; DCC_v1_2[1]=0x55; DCC_v1_2[2]=0x01;
    DCC_v1_2[3]=0x13; DCC_v1_2[4]=0x03;
    DCC_v1_2[5]=dir;
    DCC_v1_2[6]=pulse & 0xFF;
    DCC_v1_2[7]=(pulse >> 8) & 0xFF;
    uint8_t check_sum=0;
    for(int i=2;i<8;i++) check_sum+=DCC_v1_2[i];
    DCC_v1_2[8]=check_sum;
    uint8_t wakeup[]={0xAA,0xAA,0xAA};
    UART_send_buffer(DCC101_v1_2_INST,wakeup,3); delay_ms(20);
    UART_send_buffer(DCC101_v1_2_INST,DCC_v1_2,9);
    motor_angle = angle;
    /* pulse: 目标位置在[0,360)的脉冲; dir: 最短路径方向 */
    motor_pulse = (int32_t)pulse * (dir==0x01 ? 1 : -1);
}

/**
 * @brief  返回当前累计角度
 * @return 当前累计角度值（度）
 */
int32_t dianji_get_angle(void)
{
    return motor_angle;
}


/**
 * @brief  定时器PWM中断处理（4路按键电平消抖扫描）
 *
 * PB9  → motor1_forward_active  (电机正向)
 * PB8  → motor1_reverse_active  (电机反向)
 * PA16 → motor1_forward_active  (电机正向)
 * PA15 → motor1_reverse_active  (电机反向)
 */
void DCC_PWM_INST_IRQHandler()
{
    /* ---- PB9 正向 ---- */
    static uint8_t  btn_stable = 0;
    static uint8_t  btn_cnt    = 0;
    /* ---- PB8 反向 ---- */
    static uint8_t  btn2_stable = 0;
    static uint8_t  btn2_cnt    = 0;
    /* ---- PA16 正向 ---- */
    static uint8_t  btn3_stable = 0;
    static uint8_t  btn3_cnt    = 0;
    /* ---- PA15 反向 ---- */
    static uint8_t  btn4_stable = 0;
    static uint8_t  btn4_cnt    = 0;

    switch (DL_Timer_getPendingInterrupt(DCC_PWM_INST))
    {
        case DL_TIMER_IIDX_LOAD:
        {
            /* ====== PB9 电机正向 ====== */
            uint8_t key_pb9 = get_key_state(anjian6_PORT, anjian6_PIN_0_PIN);
            if (key_pb9 == btn_stable) {
                btn_cnt = 0;
            } else {
                btn_cnt++;
                if (btn_cnt >= 15) {
                    btn_stable = key_pb9;
                    btn_cnt    = 0;
                }
            }

            /* ====== PB8 电机反向 ====== */
            uint8_t key_pb8 = get_key_state(fan1_PORT, fan1_PIN_1_PIN);
            if (key_pb8 == btn2_stable) {
                btn2_cnt = 0;
            } else {
                btn2_cnt++;
                if (btn2_cnt >= 15) {
                    btn2_stable = key_pb8;
                    btn2_cnt    = 0;
                }
            }

            /* ====== PA16 电机正向 ====== */
            uint8_t key_pa16 = get_key_state(anjian7_PORT, anjian7_PIN_7_PIN);
            if (key_pa16 == btn3_stable) {
                btn3_cnt = 0;
            } else {
                btn3_cnt++;
                if (btn3_cnt >= 15) {
                    btn3_stable = key_pa16;
                    btn3_cnt    = 0;
                }
            }

            /* ====== PA15 电机反向 ====== */
            uint8_t key_pa15 = get_key_state(fan2_PORT, fan2_PIN_2_PIN);
            if (key_pa15 == btn4_stable) {
                btn4_cnt = 0;
            } else {
                btn4_cnt++;
                if (btn4_cnt >= 15) {
                    btn4_stable = key_pa15;
                    btn4_cnt    = 0;
                }
            }

            /* 任意正向按键 → 电机正转，任意反向按键 → 电机反转 */
            motor1_forward_active = btn_stable || btn3_stable;
            motor1_reverse_active = btn2_stable || btn4_stable;

            break;
        }

        default:
            break;
    }
}
