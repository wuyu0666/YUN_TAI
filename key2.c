#include "GC.h"

/**
 * @brief  轮询方式读取按键7(PA16)状态（含软件消抖）
 *
 * 先延时20ms消抖，确认按下后触发电机2滚动30度，
 * 然后等待按键释放后再返回。
 */
void key2_read()
{
        if(DL_GPIO_readPins(anjian7_PORT, anjian7_PIN_7_PIN))
        {
            delay_ms(20);                   /* 延时20ms消抖 */
            if(DL_GPIO_readPins(anjian7_PORT, anjian7_PIN_7_PIN))
            {
                dianji2_roll(30);           /* 确认按下，电机2滚动30度 */
                while(DL_GPIO_readPins(anjian7_PORT, anjian7_PIN_7_PIN))
                {
                    delay_ms(20);           /* 等待按键释放 */
                }
            }
        }
}

/**
 * @brief  读取按键7(PA16)的电平
 * @param  port  GPIO端口寄存器基地址
 * @param  key   引脚掩码
 * @return 1 = 按键按下(高电平)，0 = 按键松开(低电平)
 */
uint8_t get_key7_state(GPIO_Regs * const port, uint32_t key) {
    uint32_t high_bits = DL_GPIO_readPins(port, key); /* 读取端口电平 e.g. 0x00010000 PA16 */
    if((high_bits & key) != 0) return 1;
    else return 0;
}

/**
 * @brief  初始化按键7(PA16)的GPIO
 *
 * 配置PA16为输入模式并下拉，清除初始状态。
 * 消抖扫描由TIMA0定时器中断统一处理（在bujin.c的ISR中）。
 */
void key7_init(void)
{
    DL_GPIO_clearPins(anjian7_PORT, anjian7_PIN_7_PIN);
}
