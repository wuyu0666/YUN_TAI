/*
 * Copyright (c) 2021, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* ====== 鐢垫満杩炴帴鐘舵€侊細0=鏈繛鎺ワ紝1=宸茶繛鎺?====== */
#define MOTOR_CONNECTED 1

#include "GC.h"

uint8_t DCC_v1_2[50]={0};

volatile uint8_t  bujin_x;
volatile uint8_t  dir_x;
volatile uint16_t pulse_x;
volatile int32_t  motor_angle = 0;
volatile int32_t  motor_pulse = 0;
volatile int16_t  motor_target = 0;

static PID_Inc_t pid_x;
static float pid_prev_out = 0;
static char disp_buf[32];

int main(void){
    SYSCFG_DL_init();
    OLED_Init();
    OLED_ColorTurn(0); OLED_DisplayTurn(0);
    OLED_Clear();

    /* 鍒濆鍖栧璁惧畬鎴愶紝OLED 鏄剧ず鍚姩淇℃伅 */
    OLED_ShowString(0, 0,  (u8 *)"OLED Init OK", 12);
#if MOTOR_CONNECTED
    OLED_ShowString(0, 16, (u8 *)"Motor: ON", 12);
#else
    OLED_ShowString(0, 16, (u8 *)"Motor: OFF", 12);
#endif
    OLED_ShowString(0, 48, (u8 *)"Starting...", 12);
    OLED_Refresh();
    delay_ms(1000);

    key6_init();
    NVIC_EnableIRQ(K230_INST_INT_IRQN);

    PID_Inc_Init(&pid_x, 0.8f, 0.05f, 0.15f, 3.0f, 3000.0f, -3000.0f);

#if MOTOR_CONNECTED
    /* ---- 鐢垫満杩炴帴妫€娴嬮€氳繃锛岃缃綋鍓嶄綅缃负鍘熺偣 ---- */
    dianji_set_origin();
    delay_ms(200);
#endif

    while (1) {
#if MOTOR_CONNECTED
        /* ---- 涓诲惊鐜細濉厖鐢垫満鎺у埗鍗忚甯э紙榛樿姝ｅ悜锛?--- */
        DCC_v1_2[0]=0xAA; DCC_v1_2[1]=0x55; DCC_v1_2[2]=0x01;
        DCC_v1_2[3]=0x11; DCC_v1_2[4]=0x05; DCC_v1_2[5]=0x01;
        DCC_v1_2[6]=0x00; DCC_v1_2[7]=0x00;
#endif

        /* ========== OLED 鏄剧ず鍒锋柊 ========== */
        /* 绗?琛岋細鏄剧ず鐩爣瑙掑害 */
#if MOTOR_CONNECTED
        sprintf(disp_buf, "Tgt:%+05d", motor_target);
#else
        sprintf(disp_buf, "Motor: OFF");
#endif
        OLED_ShowString(0, 0, (u8 *)disp_buf, 12);

        /* 绗?琛岋細鏄剧ず瀹為檯瑙掑害 */
#if MOTOR_CONNECTED
        sprintf(disp_buf, "Act:%+05d", (int)motor_angle);
#else
        sprintf(disp_buf, "No Motor Conn");
#endif
        OLED_ShowString(0, 16, (u8 *)disp_buf, 12);

        /* 绗?琛岋細鏄剧ず鎸夐敭鐘舵€?*/
        if (btn1_active)                OLED_ShowString(0, 32, (u8 *)"Btn1:SET0 ", 12);
        else if (btn2_active)           OLED_ShowString(0, 32, (u8 *)"Btn2:---- ", 12);
        else if (btn3_active)           OLED_ShowString(0, 32, (u8 *)"Btn3:---- ", 12);
        else if (btn4_active)           OLED_ShowString(0, 32, (u8 *)"Btn4:---- ", 12);
        else                            OLED_ShowString(0, 32, (u8 *)"Btn :---- ", 12);

        /* 绗?琛岋細鏄剧ず K230 瑙嗚鍙嶉鏁版嵁 */
        if (bujin_x == 1) {
            sprintf(disp_buf, "FB:%c%-5u", (dir_x==0x01)?'+':'-', pulse_x);
            OLED_ShowString(0, 48, (u8 *)disp_buf, 12);
        } else {
            OLED_ShowString(0, 48, (u8 *)"FB:-----", 12);
        }
        OLED_Refresh();

#if MOTOR_CONNECTED
        /* ---- 鎸夐敭1(PB9)涓婂崌娌匡細鐢垫満褰掗浂鍘熺偣 ---- */
        {
            static uint8_t btn1_prev = 0;
            if (btn1_active && !btn1_prev) {
                motor_target = 0;
                dianji_set_origin();
                pid_prev_out = 0;
            }
            btn1_prev = btn1_active;
        }

        /* ---- K230 瑙嗚璇樊鍙嶉 鈫?PID 闂幆鎺у埗 ---- */
        if (bujin_x == 1)
        {
            float feedback_x = (dir_x == 0x01) ? (float)pulse_x : -(float)pulse_x;
            float out_x = PID_Inc_Calc(&pid_x, 0.0f, feedback_x);
            int32_t delta = (int32_t)(out_x - pid_prev_out);
            pid_prev_out = out_x;
            if (delta != 0) {
                dianji1_pulse(delta);
            }
            motor_pulse += delta;
            bujin_x = 0;
        }
#endif
    }
}
