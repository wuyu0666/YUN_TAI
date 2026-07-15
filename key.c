#include "key.h"
#include "bujin.h"

/* DCC驱动器通信协议缓冲区 */
extern uint8_t DCC_v1_2[50];

/**
 * @brief  轮询方式读取按键6状态（含软件消抖）
 *
 * 先延时20ms消抖，确认按下后触发电机滚动30度，
 * 然后等待按键释放后再返回。
 */
void key_read()
{
        if(DL_GPIO_readPins(anjian6_PORT, anjian6_PIN_0_PIN))
        {
            delay_ms(20);                   /* 延时20ms消抖 */
            if(DL_GPIO_readPins(anjian6_PORT, anjian6_PIN_0_PIN))
            {
                dianji_roll(30);            /* 确认按下，电机滚动30度 */
                while(DL_GPIO_readPins(anjian6_PORT, anjian6_PIN_0_PIN))
                {
                    delay_ms(20);           /* 等待按键释放 */
                }
            }
        }
}

/**
 * @brief  读取指定GPIO端口的按键电平
 * @param  port  GPIO端口寄存器基地址
 * @param  key   引脚掩码
 * @return 1 = 按键按下(高电平)，0 = 按键松开(低电平)
 */
uint8_t get_key_state(GPIO_Regs * const port, uint32_t key) {
    uint32_t high_bits = DL_GPIO_readPins(port, key); /* 读取端口电平 e.g. 0x00000040 PB6 */
    if((high_bits & key) != 0) return 1;
    else return 0;
}

/**
 * @brief  初始化按键6的消抖定时器
 *
 * 配置PWM定时器频率约710Hz（周期约1.4ms），占空比50%，
 * 用于在中断中周期性地采样按键状态实现消抖。
 */
void key6_init(void)
{
    DL_GPIO_clearPins(anjian6_PORT, anjian6_PIN_0_PIN);
    NVIC_EnableIRQ(DCC_PWM_INST_INT_IRQN);
     
    /* 根据速度计算PWM频率：40 / 0.05625 ≈ 711Hz → 周期约1.4ms */
    uint32_t frequency = (uint32_t)(40 / 0.05625);
    frequency = frequency > 0 ? frequency : 1;

    /* 计算定时器溢出周期值 */
    uint32_t period = DCC_PWM_INST_CLK_FREQ / frequency;
    period = period < 65536 ? period : 65535;   /* 限制最大65535(16位) */
    period = period > 800 ? period : 800;       /* 限制最小800，防止频率过高 */
    DL_Timer_setLoadValue(DCC_PWM_INST, period);
    DL_Timer_setCaptureCompareValue(DCC_PWM_INST, period / 2, GPIO_DCC_PWM_C0_IDX); /* 占空比50% */

    DL_Timer_startCounter(DCC_PWM_INST);        /* 启动定时器 */
}
