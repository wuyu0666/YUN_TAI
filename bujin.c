#include "bujin.h"
#include "bujin2.h"

/* DCC驱动器通信协议缓冲区 */
extern uint8_t DCC_v1_2[50];

/* 主循环用标志位：按键触发后置1，通知主循环执行电机动作 */
volatile uint8_t motor_trigger = 0;

/* 电机2的标志位在bujin2.c中定义 */
extern volatile uint8_t motor2_trigger;

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
    DCC_v1_2[8]=(jiaodu*roll)&0xFF;         /* 脉冲数低字节 */
    DCC_v1_2[9]=((jiaodu*roll)>>8)&0xFF;    /* 脉冲数高字节 */
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
/**
 * @brief  定时器PWM中断处理（用于按键消抖扫描）
 *
 * 通过定时器中断周期性地采样按键6(PB9)和按键7(PA16)状态，实现软件消抖。
 * 当检测到按键稳定按下时，分别置位 motor_trigger / motor2_trigger 通知主循环。
 */
void DCC_PWM_INST_IRQHandler()
{

    /* ---------- 按键6 (PB9) 消抖 ---------- */
    static uint8_t  btn_stable = 0;       /* 消抖后的稳定按键值 */
    static uint8_t  btn_cnt    = 0;       /* 消抖计数 */
    static uint8_t  btn_last   = 0;       /* 上一次稳定状态，用于检测下降沿 */
    /* ---------- 按键7 (PA16) 消抖 ---------- */
    static uint8_t  btn2_stable = 0;      /* 按键7消抖稳定值 */
    static uint8_t  btn2_cnt    = 0;      /* 按键7消抖计数 */
    static uint8_t  btn2_last   = 0;      /* 按键7上一次稳定状态 */

    switch (DL_Timer_getPendingInterrupt(DCC_PWM_INST))
    {
        case DL_TIMER_IIDX_LOAD:
        {
            /* 读取按键6 (PB9) 当前电平 */
            uint8_t key_6 = get_key_state(anjian6_PORT, anjian6_PIN_0_PIN);

            if (key_6 == btn_stable)
            {
                btn_cnt = 0;                  /* 状态未变，清零消抖计数 */
            }
            else
            {
                btn_cnt++;
                if (btn_cnt >= 15)            /* 15 * 1.4ms ≈ 21ms 消抖时间 */
                {
                    btn_stable = key_6;       /* 状态稳定，更新稳定值 */
                    btn_cnt    = 0;
                }
                /* 检测按下沿：上次松开(0) -> 本次按下(1) */
                if (btn_stable == 1 && btn_last == 0)
                {
                    motor_trigger = 1;        /* 通知主循环处理按键事件 */
                }
                btn_last = btn_stable;
            }

            /* 读取按键7 (PA16) 当前电平 */
            uint8_t key_7 = get_key_state(anjian7_PORT, anjian7_PIN_7_PIN);

            if (key_7 == btn2_stable)
            {
                btn2_cnt = 0;
            }
            else
            {
                btn2_cnt++;
                if (btn2_cnt >= 15)           /* 15 * 1.4ms ≈ 21ms 消抖时间 */
                {
                    btn2_stable = key_7;
                    btn2_cnt    = 0;
                }
                if (btn2_stable == 1 && btn2_last == 0)
                {
                    motor2_trigger = 1;       /* 通知主循环处理按键7事件 */
                }
                btn2_last = btn2_stable;
            }

            break;
        }

        default:
            break;
    }
}