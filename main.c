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
uint8_t DCC_v1_3[50]={0};

volatile uint8_t  bujin_x;
volatile uint8_t  bujin_y;
volatile uint8_t  dir_x;
volatile uint16_t pulse_x;
volatile uint8_t  dir_y;
volatile uint16_t pulse_y;

static PID_Inc_t pid_x;
static PID_Inc_t pid_y;

int main(void){
    SYSCFG_DL_init();
    OLED_Init();
    OLED_ColorTurn(0); OLED_DisplayTurn(0);
    OLED_Clear();
    key6_init(); key7_init();
    NVIC_EnableIRQ(K230_INST_INT_IRQN);

    PID_Inc_Init(&pid_x, 0.5f, 0.0f, 0.0f, 3.0f, 3000.0f, -3000.0f);
    PID_Inc_Init(&pid_y, 0.2f, 0.0f, 0.0f, 3.0f, 3000.0f, -3000.0f);

    OLED_ShowString(0, 0, (u8 *)"DCC-101 v1", 12);
    OLED_ShowString(0, 24, (u8 *)"Waiting K230...", 12);
    OLED_Refresh();

    while (1) {
        DCC_v1_2[0]=0xAA; DCC_v1_2[1]=0x55; DCC_v1_2[2]=0x01;
        DCC_v1_2[3]=0x11; DCC_v1_2[4]=0x05; DCC_v1_2[5]=0x01;
        DCC_v1_2[6]=0x00; DCC_v1_2[7]=0x00;

        DCC_v1_3[0]=0xAA; DCC_v1_3[1]=0x55; DCC_v1_3[2]=0x01;
        DCC_v1_3[3]=0x11; DCC_v1_3[4]=0x05; DCC_v1_3[5]=0x01;
        DCC_v1_3[6]=0x00; DCC_v1_3[7]=0x00;

        if (motor1_forward_active)  { dianji_roll(30);  delay_ms(80); }
        if (motor1_reverse_active)  { dianji_roll(-30); delay_ms(80); }
        if (motor2_forward_active)  { dianji2_roll(30);  delay_ms(80); }
        if (motor2_reverse_active)  { dianji2_roll(-30); delay_ms(80); }

        if (bujin_x == 1)
        {
            float feedback_x = (dir_x == 0x01) ? (float)pulse_x : -(float)pulse_x;
            float out_x = PID_Inc_Calc(&pid_x, 0.0f, feedback_x);
            dianji1_pulse((int32_t)out_x);
            bujin_x = 0;
        }
        if (bujin_y == 1)
        {
            float feedback_y = (dir_y == 0x01) ? (float)pulse_y : -(float)pulse_y;
            float out_y = PID_Inc_Calc(&pid_y, 0.0f, feedback_y);
            dianji2_pulse((int32_t)(-out_y));
            bujin_y = 0;
        }
    }
}
