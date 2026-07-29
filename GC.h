#ifndef __GC_H
#define __GC_H

/**
 * @file    GC.h
 * @brief   閸忋劌鐪径瀛樻瀮娴犺埖鐪归幀?
 *
 * 缂佺喍绔撮崠鍛儓閹碘偓閺堝銆嶉惄顔荤贩鐠ф牔绗岄崗銊ョ湰閸欐﹢鍣烘竟鐗堟閿涘苯鍙炬禒鏍ㄧ爱閺傚洣娆㈤崣顏堟付 #include "GC.h"
 */

/* ====== 绾兛娆㈡す鍗炲З闁板秶鐤嗛敍鍦珁sConfig 閼奉亜濮╅悽鐔稿灇閿?====== */
#include "ti_msp_dl_config.h"       /* MSPM0 婢舵牞顔曢柊宥囩枂閿涙PIO/UART/PWM/閺冨爼鎸?*/

/* ====== 閸╄櫣顢呭銉ュ徔濡€虫健 ====== */
#include "delay.h"                  /* 濮ｎ偆顫楃痪褍娆㈤弮璺哄毐閺?delay_ms() */
#include "uart.h"                   /* 娑撴彃褰涢弨璺哄絺閿涙瓗ART_send_char/string/buffer */

/* ====== 閹稿鏁Ο鈥虫健 ====== */
#include "key.h"                    /* 閹稿鏁?(PB9)閿涙et_key_state / key_read / key6_init */

/* ====== 濮濄儴绻橀悽鍨簚閹貉冨煑濡€虫健 ====== */
#include "bujin.h"                  /* 閻㈠灚婧€1(UART2/PA21/PA22)閿涙瓰ianji_roll() */

/* ====== PID 閹貉冨煑濡€虫健 ====== */
#include "bihuan_pid.h"             /* 婢х偤鍣哄?PID 閹貉冨煑閸?*/

/* ====== OLED 閺勫墽銇氬Ο鈥虫健 ====== */
#include "oled.h"                   /* SSD1306 128x64 OLED 妞瑰崬濮?*/

/* ====== 閺嶅洤鍣?C 鎼?====== */
#include <stdio.h>                  /* printf/sprintf 缁涘鐖ｉ崙?I/O */
#include <stdlib.h>                 /* malloc/free/atoi 缁涘鐖ｉ崙鍡楃氨閸戣姤鏆?*/

/* ====== 閸忋劌鐪崣姗€鍣烘竟鐗堟閿涘潒xtern閿涘鈧柡鈧?鐎圭偤妾€规矮绠熼崷銊ヮ嚠鎼?.c 閺傚洣娆㈡稉?====== */

/* 閻㈠灚婧€闁矮淇婇崡蹇氼唴鐢呯处閸愭彃灏敍鍫濈暰娑斿婀?main.c閿?*/
extern uint8_t DCC_v1_2[50];        /* 閻㈠灚婧€1 閸楀繗顔呯敮褝绱濋柅姘崇箖 UART2(PA21/PA22) 閸欐垿鈧?*/

/* 閻㈠灚婧€閹貉冨煑閺嶅洤绻旈敍?=閹稿缍囬幐浣虹敾鏉烆剨绱?=閺夋儳绱戦崑婊勵剾閿?*/
extern volatile uint8_t btn1_active;   /* 按键1 PB9  */
extern volatile uint8_t btn2_active;   /* 按键2 PB8  */
extern volatile uint8_t btn3_active;   /* 按键3 PA16 */
extern volatile uint8_t btn4_active;   /* 按键4 PA15 */
extern volatile uint8_t bujin_x;
extern volatile uint8_t dir_x;
extern volatile uint16_t pulse_x;
extern volatile int32_t motor_angle;   /* 绱Н鏃嬭浆瑙掑害锛堝害锛?*/
extern volatile int32_t motor_pulse;   /* 绱Н鑴夊啿鏁?*/
#endif
