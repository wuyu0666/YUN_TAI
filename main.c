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

#include "ti_msp_dl_config.h"
#include "delay.h"
#include "oled.h"
#include <stdio.h>
#include "uart.h"
#include "key.h"
#include "bujin.h"
#include "bujin2.h"
#include "key2.h"

/* 电机1协议帧缓冲区（通过UART2: PA21/PA22发送） */
uint8_t DCC_v1_2[50]={0};

/* 电机2协议帧缓冲区（通过UART1: PB6/PB7发送） */
uint8_t DCC_v1_3[50]={0};

extern volatile uint8_t motor_trigger;
extern volatile uint8_t motor2_trigger;
int main(void)
{
    SYSCFG_DL_init();
    OLED_Init();
    OLED_ColorTurn(0);//0正常显示，1 反色显示
    OLED_DisplayTurn(0);//0正常显示 1 屏幕翻转显示
    OLED_Clear();
    key6_init();
    key7_init();
    NVIC_EnableIRQ(PRINT_INST_INT_IRQN);
    while (1) {
        // OLED_Refresh();
        /* 电机1协议帧头预填充（UART2: PA21/PA22） */
        DCC_v1_2[0]=0xAA;
        DCC_v1_2[1]=0x55;
        DCC_v1_2[2]=0x01;
        DCC_v1_2[3]=0x11;
        DCC_v1_2[4]=0x05;
        DCC_v1_2[5]=0x01;
        DCC_v1_2[6]=0x00;
        DCC_v1_2[7]=0x00;
        /* 电机2协议帧头预填充（UART1: PB6/PB7） */
        DCC_v1_3[0]=0xAA;
        DCC_v1_3[1]=0x55;
        DCC_v1_3[2]=0x01;
        DCC_v1_3[3]=0x11;
        DCC_v1_3[4]=0x05;
        DCC_v1_3[5]=0x01;
        DCC_v1_3[6]=0x00;
        DCC_v1_3[7]=0x00;

        if (motor_trigger)
        {
            motor_trigger = 0;
            dianji_roll(30);
            delay_ms(200);    // 30° 转到位大概需要几百毫秒，根据实际转速调整
        }
        if (motor2_trigger)
        {
            motor2_trigger = 0;
            dianji2_roll(30);
            delay_ms(200);    // 30° 转到位大概需要几百毫秒，根据实际转速调整
        }
    }
}
