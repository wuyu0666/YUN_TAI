#ifndef __GC_H
#define __GC_H

/**
 * @file    GC.h
 * @brief   闁稿繈鍔岄惇顒佸緞鐎涙ɑ鐎ù鐘哄煐閻綊骞€?
 *
 * 缂備胶鍠嶇粩鎾礌閸涱厽鍎撻柟纰樺亾闁哄牆顦甸妴宥夋儎椤旇崵璐╅悹褎鐗旂粭宀勫礂閵娿儳婀伴柛娆愶耿閸ｇ儤绔熼悧鍫燁潠闁挎稑鑻崣鐐閺嶃劎鐖遍柡鍌氭矗濞嗐垽宕ｉ鍫熶粯 #include "GC.h"
 */

/* ====== 缁绢収鍏涘▎銏°仚閸楃偛袟闂佹澘绉堕悿鍡涙晬閸︾弫sConfig 闁煎浜滄慨鈺呮偨閻旂鐏囬柨?====== */
#include "ti_msp_dl_config.h"       /* MSPM0 濠㈣埖鐗為鏇㈡煀瀹ュ洨鏋傞柨娑欘儞PIO/UART/PWM/闁哄啫鐖奸幐?*/

/* ====== 闁糕晞娅ｉ、鍛啅閵夈儱寰旀俊顖椻偓铏仴 ====== */
#include "delay.h"                  /* 婵綆鍋嗛～妤冪棯瑜嶅▎銏ゅ籍鐠哄搫姣愰柡?delay_ms() */
#include "uart.h"                   /* 濞戞挻褰冭ぐ娑㈠绩鐠哄搫绲洪柨娑欑摋ART_send_char/string/buffer */

/* ====== 闁圭顦甸弫顓炍熼垾铏仴 ====== */
#include "key.h"                    /* 闁圭顦甸弫?(PB9)闁挎稒顒歟t_key_state / key_read / key6_init */

/* ====== 婵縿鍎寸换姗€鎮介崹顐ｇ皻闁硅矇鍐ㄧ厬婵☆垪鈧櫕鍋?====== */
#include "bujin.h"                  /* 闁汇垹鐏氬┃鈧?(UART2/PA21/PA22)闁挎稒鐡癷anji_roll() */

/* ====== PID 闁硅矇鍐ㄧ厬婵☆垪鈧櫕鍋?====== */
#include "bihuan_pid.h"             /* 濠⒀呭仱閸ｅ搫顕?PID 闁硅矇鍐ㄧ厬闁?*/

/* ====== OLED 闁哄嫬澧介妵姘熼垾铏仴 ====== */
#include "oled.h"                   /* SSD1306 128x64 OLED 濡炵懓宕慨?*/

/* ====== 闁哄秴娲ら崳?C 閹?====== */
#include <stdio.h>                  /* printf/sprintf 缂佹稑顦伴悥锝夊礄?I/O */
#include <stdlib.h>                 /* malloc/free/atoi 缂佹稑顦伴悥锝夊礄閸℃姘ㄩ柛鎴ｅГ閺?*/

/* ====== 闁稿繈鍔岄惇顒勫矗濮椻偓閸ｇ儤绔熼悧鍫燁潠闁挎稑娼抶tern闁挎稑顦埀顒佹煛閳?閻庡湱鍋ゅ顖溾偓瑙勭煯缁犵喖宕烽妸銉殸閹?.c 闁哄倸娲ｅ▎銏＄▔?====== */

/* 闁汇垹鐏氬┃鈧梺顐ｇ煯娣囧﹪宕¤箛姘煎敶閻㈩垎鍛闁告劖褰冪亸顖炴晬閸繄鏆板☉鏂款槸濠€?main.c闁?*/
extern uint8_t DCC_v1_2[50];        /* 闁汇垹鐏氬┃鈧? 闁告绻楅鍛暜瑜濈槐婵嬫焻濮樺磭绠?UART2(PA21/PA22) 闁告瑦鍨块埀?*/

/* 闁汇垹鐏氬┃鈧柟璨夊啫鐓戦柡宥呮搐缁绘棃鏁?=闁圭顦紞鍥箰娴ｈ櫣鏁鹃弶鐑嗗墾缁?=闁哄鍎崇槐鎴﹀磻濠婂嫷鍓鹃柨?*/
extern volatile uint8_t btn1_active;   /* 鎸夐敭1 PB9  */
extern volatile uint8_t btn2_active;   /* 鎸夐敭2 PB8  */
extern volatile uint8_t btn3_active;   /* 鎸夐敭3 PA16 */
extern volatile uint8_t btn4_active;   /* 鎸夐敭4 PA15 */
extern volatile uint8_t bujin_x;
extern volatile uint8_t dir_x;
extern volatile uint16_t pulse_x;
extern volatile int32_t motor_angle;   /* 缁鳖垳袧閺冨娴嗙憴鎺戝閿涘牆瀹抽敍?*/
extern volatile int32_t motor_pulse;   /* 缁鳖垳袧閼村鍟块弫?*/
extern volatile int16_t motor_target;  /* 目标角度（钳位前） */
#endif
