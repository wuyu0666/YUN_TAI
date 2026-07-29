#ifndef __GC_H
#define __GC_H

/**
 * @file    GC.h
 * @brief   鍏ㄥ眬澶存枃浠舵眹鎬?
 *
 * 缁熶竴鍖呭惈鎵€鏈夐」鐩緷璧栦笌鍏ㄥ眬鍙橀噺澹版槑锛屽叾浠栨簮鏂囦欢鍙渶 #include "GC.h"
 */

/* ====== 纭欢椹卞姩閰嶇疆锛圫ysConfig 鑷姩鐢熸垚锛?====== */
#include "ti_msp_dl_config.h"       /* MSPM0 澶栬閰嶇疆锛欸PIO/UART/PWM/鏃堕挓 */

/* ====== 鍩虹宸ュ叿妯″潡 ====== */
#include "delay.h"                  /* 姣绾у欢鏃跺嚱鏁?delay_ms() */
#include "uart.h"                   /* 涓插彛鏀跺彂锛歎ART_send_char/string/buffer */

/* ====== 鎸夐敭妯″潡 ====== */
#include "key.h"                    /* 鎸夐敭1(PB9)锛歡et_key_state / key_read / key6_init */

/* ====== 姝ヨ繘鐢垫満鎺у埗妯″潡 ====== */
#include "bujin.h"                  /* 鐢垫満1(UART2/PA21/PA22)锛歞ianji_roll() */

/* ====== PID 鎺у埗妯″潡 ====== */
#include "bihuan_pid.h"             /* 澧為噺寮?PID 鎺у埗鍣?*/

/* ====== OLED 鏄剧ず妯″潡 ====== */
#include "oled.h"                   /* SSD1306 128x64 OLED 椹卞姩 */

/* ====== 鏍囧噯 C 搴?====== */
#include <stdio.h>                  /* printf/sprintf 绛夋爣鍑?I/O */
#include <stdlib.h>                 /* malloc/free/atoi 绛夋爣鍑嗗簱鍑芥暟 */

/* ====== 鍏ㄥ眬鍙橀噺澹版槑锛坋xtern锛夆€斺€?瀹為檯瀹氫箟鍦ㄥ搴?.c 鏂囦欢涓?====== */

/* 鐢垫満閫氫俊鍗忚甯х紦鍐插尯锛堝畾涔夊湪 main.c锛?*/
extern uint8_t DCC_v1_2[50];        /* 鐢垫満1 鍗忚甯э紝閫氳繃 UART2(PA21/PA22) 鍙戦€?*/

/* 鐢垫満鎺у埗鏍囧織锛?=鎸変綇鎸佺画杞紝0=鏉惧紑鍋滄锛?*/
extern volatile uint8_t motor1_forward_active;   /* 鐢垫満1姝ｅ悜 PB9  (瀹氫箟鍦?bujin.c)  */
extern volatile uint8_t motor1_reverse_active;   /* 鐢垫満1鍙嶅悜 PB8  (瀹氫箟鍦?bujin.c)  */
extern volatile uint8_t bujin_x;
extern volatile uint8_t dir_x;
extern volatile uint16_t pulse_x;
extern volatile int32_t motor_angle;   /* 累积旋转角度（度） */
extern volatile int32_t motor_pulse;   /* 累积脉冲数 */
#endif
