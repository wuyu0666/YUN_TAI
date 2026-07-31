#define MOTOR_CONNECTED 1

/* ----- 反馈丢失超时参数 ----- */
#define FB_LOST_THRESHOLD  50      /* 无反馈超时阈值（主循环迭代次数，约500ms） */

#include "GC.h"

uint8_t DCC_v1_2[50] = {0};
volatile uint8_t  bujin_x, dir_x;
volatile uint16_t pulse_x;
volatile int32_t  motor_angle = 0, motor_pulse = 0;
volatile int16_t  motor_target = 0;
static char dbuf[32];

int main(void)
{
    SYSCFG_DL_init();
    OLED_Init();
    OLED_ColorTurn(0); OLED_DisplayTurn(0);
    OLED_Clear();
    OLED_ShowString(0, 0,  (u8 *)"PID Simple", 12);
    OLED_ShowString(0, 48, (u8 *)"Starting...", 12);
    OLED_Refresh();
    delay_ms(500);

    key6_init();
    NVIC_EnableIRQ(K230_INST_INT_IRQN);
    pid_control_init();

#if MOTOR_CONNECTED
    dianji_set_origin();
    delay_ms(200);
#endif

    float   pprev  = 0.0f;
    int32_t pout   = 0;
    int16_t perr   = 0;
    int16_t  last_perr_saved = 0;   /* 保存最后一次有效偏差值 */
    uint8_t  last_perr_valid = 0;   /* 是否有有效的上次偏差 */
    uint16_t no_fb_cnt       = 0;   /* 无反馈计数 */

    while (1)
    {
#if MOTOR_CONNECTED
        /* 接收新帧：只更新误差输入，驱动交给下方固定节拍 */
        if (bujin_x == 1)
        {
            perr  = (int16_t)((dir_x == 0x01) ? (int16_t)pulse_x : -(int16_t)pulse_x);
            last_perr_saved = perr;   /* 保存最后一次有效偏差 */
            last_perr_valid = 1;
            no_fb_cnt = 0;            /* 重置丢失计数 */
            bujin_x = 0;
        }
        else
        {
            /* 无新反馈，累加丢失计数（仅用于 OLED 显示 LOST） */
            if (no_fb_cnt < 0xFFFF) no_fb_cnt++;
        }

        /* 固定节拍执行 PID：有新帧用新误差，没新帧沿用上次误差
         * 持续修正，球静止不发帧时也能继续推，到位后死区内停机 */
        if (pid_timer_tick != 0U)
        {
            pid_timer_tick = 0U;
            if (last_perr_valid)
            {
                int32_t d = pid_compute((float)last_perr_saved, &pprev);
                pout = (int32_t)pprev;
                if (d != 0) { dianji1_pulse_fast(d); motor_pulse += d; }
            }
        }
#endif
        /* 按键 */
        { static uint8_t b1=0; if(btn1_active&&!b1){motor_target=0;dianji_set_origin();pid_reset_state();pprev=0;pout=0;perr=0;} b1=btn1_active; }
        { static uint8_t b2=0,b3=0,b4=0; if(btn2_active&&!b2)pid_tune_kp(0.02f); if(btn3_active&&!b3)pid_tune_kd(0.05f); if(btn4_active&&!b4)pid_tune_ki(0.001f); b2=btn2_active;b3=btn3_active;b4=btn4_active; }

        /* OLED */
        sprintf(dbuf,"E:%+05d O:%+05ld",(int)perr,(long)pout); OLED_ShowString(0,0,(u8*)dbuf,12);
        { float kp,ki,kd; pid_get_params(&kp,&ki,&kd); sprintf(dbuf,"P%.2f I%.3f D%.2f",kp,ki,kd); }
        OLED_ShowString(0,16,(u8*)dbuf,12);
        if(btn1_active)       OLED_ShowString(0,32,(u8*)"Btn1:SET0 ",12);
        else if(btn2_active)  OLED_ShowString(0,32,(u8*)"Btn2:Kp+  ",12);
        else if(btn3_active)  OLED_ShowString(0,32,(u8*)"Btn3:Kd+  ",12);
        else if(btn4_active)  OLED_ShowString(0,32,(u8*)"Btn4:Ki+  ",12);
        else                  OLED_ShowString(0,32,(u8*)"Btn :---- ",12);
        sprintf(dbuf,"FB:%+05d FAST",(int)perr); OLED_ShowString(0,48,(u8*)dbuf,12);
        if (no_fb_cnt >= FB_LOST_THRESHOLD && last_perr_valid) {
            OLED_ShowString(56,48,(u8*)"LOST",12);
        }
        OLED_Refresh();
    }
}
