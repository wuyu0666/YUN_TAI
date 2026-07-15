#include "key.h"
#include "bujin.h"
extern uint8_t DCC_v1_2[50];
void key_read()
{
        if(DL_GPIO_readPins(anjian6_PORT, anjian6_PIN_0_PIN))
        {
            delay_ms(20);
            if(DL_GPIO_readPins(anjian6_PORT, anjian6_PIN_0_PIN))
            {
                dianji_roll(30);
                while(DL_GPIO_readPins(anjian6_PORT, anjian6_PIN_0_PIN))
                {
                    delay_ms(20);
                }
            }
        }
}
uint8_t get_key_state(GPIO_Regs * const port, uint32_t key) {
    uint32_t high_bits = DL_GPIO_readPins(port, key); //0x00000040 0b01000000 PB6 0~31
    if((high_bits & key) != 0) return 1;
    else return 0;
}
void key6_init(void)
{
    DL_GPIO_clearPins(anjian6_PORT, anjian6_PIN_0_PIN);
    NVIC_EnableIRQ(DCC_PWM_INST_INT_IRQN);
     
    // 根据速度设置PWM频率
    uint32_t frequency = (uint32_t)(40 / 0.05625); // 计算所需的PWM频率
    frequency = frequency > 0 ? frequency : 1;

    // 计算定时器溢出值
    uint32_t period = DCC_PWM_INST_CLK_FREQ / frequency;
    period = period < 65536 ? period : 65535;
    period = period > 800 ? period : 800; 
    DL_Timer_setLoadValue(DCC_PWM_INST, period);
    DL_Timer_setCaptureCompareValue(DCC_PWM_INST, period / 2, GPIO_DCC_PWM_C0_IDX); // 设置占空比为50%

    DL_Timer_startCounter(DCC_PWM_INST);
}
