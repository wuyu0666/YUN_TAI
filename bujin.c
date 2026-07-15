#include "bujin.h"
extern uint8_t DCC_v1_2[50];
volatile uint8_t motor_trigger = 0;   // 主循环用标志位
void key6_init();
void dianji_roll(int16_t roll)
{
    uint8_t fangxiang=0x01;
    if(roll<=0)
    {
        roll=-roll;
        fangxiang=0x00;
    }
    else
    {
        fangxiang=0x01;
    }
    DCC_v1_2[8]=(jiaodu*roll)&0xFF;
    DCC_v1_2[9]=((jiaodu*roll)>>8)&0xFF;
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
void DCC_PWM_INST_IRQHandler()
{

    static uint8_t  btn_stable = 0;       // 消抖稳定值
    static uint8_t  btn_cnt    = 0;       // 消抖计数
    static uint8_t  btn_last   = 0;       // 上次稳定状态（用于检测按下沿）

    switch (DL_Timer_getPendingInterrupt(DCC_PWM_INST))
    {
        case DL_TIMER_IIDX_LOAD:
        {
            uint8_t key_6 = get_key_state(anjian6_PORT, anjian6_PIN_0_PIN);

            if (key_6 == btn_stable)
            {
                btn_cnt = 0;                  // 状态未变，清零计数
            }
            else
            {
                btn_cnt++;
                if (btn_cnt >= 15)            // 15 * 1.4ms ≈ 21ms 消抖时间
                {
                    btn_stable = key_6;
                    btn_cnt    = 0;
                }
                if (btn_stable == 1 && btn_last == 0)
                {
                    motor_trigger = 1;    // 按下沿，通知主循环
                }
                btn_last = btn_stable;
            }
            break;
        }

        default:
            break;
    }
}