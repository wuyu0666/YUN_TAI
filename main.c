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
uint8_t DCC_v1_2[50]={0};
extern volatile uint8_t motor_trigger;
int main(void)
{
    SYSCFG_DL_init();
    OLED_Init();
    OLED_ColorTurn(0);//0正常显示，1 反色显示
    OLED_DisplayTurn(0);//0正常显示 1 屏幕翻转显示
    OLED_Clear();
    key6_init();
    NVIC_EnableIRQ(PRINT_INST_INT_IRQN);
    while (1) {
        // OLED_Refresh();
        DCC_v1_2[0]=0xAA;
        DCC_v1_2[1]=0x55;
        DCC_v1_2[2]=0x01;
        DCC_v1_2[3]=0x11;
        DCC_v1_2[4]=0x05;
        DCC_v1_2[5]=0x01;
        DCC_v1_2[6]=0x00;
        DCC_v1_2[7]=0x00;
        //16634/4        8
        // DCC_v1_2[8]=(16384/4)&0xFF;
        // DCC_v1_2[9]=((16384/4)>>8)&0xFF;
        //校验值
        // uint8_t check_sum=0;
        // for(int i=2;i<10;i++)
        // {
        //     check_sum+=DCC_v1_2[i];
        // }
        
        // DCC_v1_2[10]=check_sum;
        // uint8_t key_6=get_key_state(anjian6_PORT,anjian6_PIN_0_PIN);
        // if(key_6==1)
        // {
        //     dianji_roll(30);
        // }
        // key_read();
        // dianji_roll(30);
        // delay_ms(3000);
        if (motor_trigger)
        {
            motor_trigger = 0;
            dianji_roll(30);
            delay_ms(200);    // 30° 转到位大概需要几百毫秒，根据实际转速调整
        }
    }
}
