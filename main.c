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

volatile uint8_t bujin_x, bujin_y;
volatile uint8_t dir_x, dir_y;
volatile uint16_t pulse_x, pulse_y;

/* OLED 显示缓存 */
static int16_t  disp_m1_in  = 0;
static int16_t  disp_m1_out = 0;
static int16_t  disp_m2_in  = 0;
static int16_t  disp_m2_out = 0;
static uint8_t  oled_dirty  = 0;

/* 滑动平均滤波：滤除K230视觉PID高频噪声 */
#define FILTER_WINDOW   5
static int16_t filter_buf_m1[FILTER_WINDOW] = {0};
static uint8_t filter_idx_m1 = 0;
static int16_t filter_buf_m2[FILTER_WINDOW] = {0};
static uint8_t filter_idx_m2 = 0;

static int16_t moving_avg(int16_t *buf, uint8_t size, int16_t new_val, uint8_t *idx)
{
    int32_t sum = 0;
    buf[*idx] = new_val;
    *idx = (*idx + 1) % size;
    for (uint8_t i = 0; i < size; i++) sum += buf[i];
    return (int16_t)(sum / size);
}

static void oled_update(void)
{
    char buf[17];
    sprintf(buf, "M1 In:%+d", disp_m1_in);
    OLED_ShowString(0, 0, (u8 *)buf, 16);
    sprintf(buf, " Out:%-5d", disp_m1_out);
    OLED_ShowString(0, 16, (u8 *)buf, 16);
    sprintf(buf, "M2 In:%+d", disp_m2_in);
    OLED_ShowString(0, 32, (u8 *)buf, 16);
    sprintf(buf, " Out:%-5d", disp_m2_out);
    OLED_ShowString(0, 48, (u8 *)buf, 16);
    OLED_Refresh();
}

int main(void)
{
    SYSCFG_DL_init();
    OLED_Init();
    OLED_ColorTurn(0); OLED_DisplayTurn(0);
    OLED_Clear();
    key6_init(); key7_init();
    NVIC_EnableIRQ(K230_INST_INT_IRQN);

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

        /* ---- M1: K230误差 → 滑动平均 → 角度换算 → 0x11 ---- */
        if (bujin_x == 1)
        {
            int16_t raw = (dir_x == 0x01) ? (int16_t)pulse_x : -(int16_t)pulse_x;
            int16_t flt = moving_avg(filter_buf_m1, FILTER_WINDOW, raw, &filter_idx_m1);
            uint8_t sdir = (flt >= 0) ? 0x01 : 0x00;
            uint16_t sval = (uint16_t)((flt >= 0) ? flt : -flt);
            dianji_roll_pulse(sval, sdir);

            disp_m1_in  = raw;
            disp_m1_out = (flt >= 0) ? flt : -flt;
            oled_dirty = 1;
            bujin_x = 0;
        }

        /* ---- M2 ---- */
        if (bujin_y == 1)
        {
            int16_t raw = (dir_y == 0x01) ? (int16_t)pulse_y : -(int16_t)pulse_y;
            int16_t flt = moving_avg(filter_buf_m2, FILTER_WINDOW, raw, &filter_idx_m2);
            uint8_t sdir = (flt >= 0) ? 0x01 : 0x00;
            uint16_t sval = (uint16_t)((flt >= 0) ? flt : -flt);
            dianji2_roll_pulse(sval, sdir);

            disp_m2_in  = raw;
            disp_m2_out = (flt >= 0) ? flt : -flt;
            oled_dirty = 1;
            bujin_y = 0;
        }

        if (oled_dirty) { oled_update(); oled_dirty = 0; }
    }
}