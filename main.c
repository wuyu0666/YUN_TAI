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

#include "GC.h"

uint8_t DCC_v1_2[50]={0};

volatile uint8_t  bujin_x;
volatile uint8_t  dir_x;
volatile uint16_t pulse_x;
volatile int32_t  motor_angle = 0;    /* 电机返回的实际角度 */
volatile int32_t  motor_pulse = 0;    /* 累积脉冲数 */
volatile int16_t  motor_target = 0;   /* 目标角度（钳位前） */

static PID_Inc_t pid_x;
static char disp_buf[32];

int main(void){
    SYSCFG_DL_init();
    OLED_Init();
    OLED_ColorTurn(0); OLED_DisplayTurn(0);
    OLED_Clear();
    key6_init();
    NVIC_EnableIRQ(K230_INST_INT_IRQN);

    PID_Inc_Init(&pid_x, 0.5f, 0.0f, 0.0f, 3.0f, 3000.0f, -3000.0f);
    /* ---- 步进电机开机较零 ---- */
    dianji_set_origin();
    delay_ms(200);

    /* ====== 步进电机开机自检（含超限钳位测试） ====== */
    motor_target = 5;
    OLED_ShowString(0, 0, (u8 *)"Tst:+005    ", 12);
    dianji_rotate_to(motor_target);
    sprintf(disp_buf, "Tgt:%+05d", motor_target);  OLED_ShowString(0, 16, (u8 *)disp_buf, 12);
    sprintf(disp_buf, "Act:%+05d", (int)motor_angle); OLED_ShowString(0, 32, (u8 *)disp_buf, 12);
    OLED_Refresh();  delay_ms(1200);

    motor_target = -5;
    OLED_ShowString(0, 0, (u8 *)"Tst:-005    ", 12);
    dianji_rotate_to(motor_target);
    sprintf(disp_buf, "Tgt:%+05d", motor_target);  OLED_ShowString(0, 16, (u8 *)disp_buf, 12);
    sprintf(disp_buf, "Act:%+05d", (int)motor_angle); OLED_ShowString(0, 32, (u8 *)disp_buf, 12);
    OLED_Refresh();  delay_ms(1200);

    /* 超限：+10° -> 钳到+5° */
    motor_target = 10;
    OLED_ShowString(0, 0, (u8 *)"Tst:+010>+5", 12);
    dianji_rotate_to(motor_target);
    sprintf(disp_buf, "Tgt:%+05d", motor_target);  OLED_ShowString(0, 16, (u8 *)disp_buf, 12);
    sprintf(disp_buf, "Act:%+05d", (int)motor_angle); OLED_ShowString(0, 32, (u8 *)disp_buf, 12);
    OLED_Refresh();  delay_ms(1500);

    /* 超限：-8° -> 钳到-5° */
    motor_target = -8;
    OLED_ShowString(0, 0, (u8 *)"Tst:-008>-5", 12);
    dianji_rotate_to(motor_target);
    sprintf(disp_buf, "Tgt:%+05d", motor_target);  OLED_ShowString(0, 16, (u8 *)disp_buf, 12);
    sprintf(disp_buf, "Act:%+05d", (int)motor_angle); OLED_ShowString(0, 32, (u8 *)disp_buf, 12);
    OLED_Refresh();  delay_ms(1500);

    motor_target = 0;
    OLED_ShowString(0, 0, (u8 *)"Tst:Finish  ", 12);
    dianji_rotate_to(motor_target);
    sprintf(disp_buf, "Tgt:%+05d", motor_target);  OLED_ShowString(0, 16, (u8 *)disp_buf, 12);
    sprintf(disp_buf, "Act:%+05d", (int)motor_angle); OLED_ShowString(0, 32, (u8 *)disp_buf, 12);
    OLED_Refresh();  delay_ms(800);


    while (1) {
        /* ---- 协议帧固定字段 ---- */
        DCC_v1_2[0]=0xAA; DCC_v1_2[1]=0x55; DCC_v1_2[2]=0x01;
        DCC_v1_2[3]=0x11; DCC_v1_2[4]=0x05; DCC_v1_2[5]=0x01;
        DCC_v1_2[6]=0x00; DCC_v1_2[7]=0x00;


        /* ========== OLED 显示 ========== */
        /* 第1行：目标角度 */
        sprintf(disp_buf, "Tgt:%+05d", motor_target);
        OLED_ShowString(0, 0, (u8 *)disp_buf, 12);

        /* 第2行：实际角度 */
        sprintf(disp_buf, "Act:%+05d", (int)motor_angle);
        OLED_ShowString(0, 16, (u8 *)disp_buf, 12);

        /* 第3行：按键状态 */
        if (btn1_active)                OLED_ShowString(0, 32, (u8 *)"Btn1:SET0 ", 12);
        else if (btn2_active)           OLED_ShowString(0, 32, (u8 *)"Btn2:---- ", 12);
        else if (btn3_active)           OLED_ShowString(0, 32, (u8 *)"Btn3:---- ", 12);
        else if (btn4_active)           OLED_ShowString(0, 32, (u8 *)"Btn4:---- ", 12);
        else                            OLED_ShowString(0, 32, (u8 *)"Btn :---- ", 12);

        /* 第4行：K230 反馈 */
        if (bujin_x == 1) {
            sprintf(disp_buf, "FB:%c%-5u", (dir_x==0x01)?'+':'-', pulse_x);
            OLED_ShowString(0, 48, (u8 *)disp_buf, 12);
        } else {
            OLED_ShowString(0, 48, (u8 *)"FB:-----", 12);
        }
        OLED_Refresh();

        /* ---- 按键1：较零（上升沿触发） ---- */
        {
            static uint8_t btn1_prev = 0;
            if (btn1_active && !btn1_prev) {
                motor_target = 0;
                dianji_set_origin();
            }
            btn1_prev = btn1_active;
        }

        /* ---- K230 视觉伺服 PID ---- */
        if (bujin_x == 1)
        {
            float feedback_x = (dir_x == 0x01) ? (float)pulse_x : -(float)pulse_x;
            float out_x = PID_Inc_Calc(&pid_x, 0.0f, feedback_x);
            dianji1_pulse((int32_t)out_x);
            motor_pulse += (int32_t)out_x;
            bujin_x = 0;
        }
    }
}
