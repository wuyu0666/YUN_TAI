#ifndef __GC_H
#define __GC_H

/**
 * @file    GC.h
 * @brief   全局公共头文件
 *
 * 所有模块统一包含此头文件即可获得全部公共声明与依赖，无需逐个 #include "GC.h"
 */

/* ====== MSPM0 SysConfig 自动生成的外设驱动配置 ====== */
#include "ti_msp_dl_config.h"       /* MSPM0 系统配置：时钟/GPIO/UART/PWM/定时器等 */

/* ====== 基础驱动模块 ====== */
#include "delay.h"                  /* 毫秒级延时函数 delay_ms() */
#include "uart.h"                   /* 串口发送函数 UART_send_char/string/buffer */

/* ====== 按键输入模块 ====== */
#include "key.h"                    /* 按键(PB9) 检测 get_key_state / key_read / key6_init */

/* ====== 步进电机驱动模块 ====== */
#include "bujin.h"                  /* 电机(UART2/PA21/PA22) dianji_roll() 等 */

/* ====== PID 闭环控制模块 ====== */
#include "bihuan_pid.h"             /* 增量式 PID 控制算法 */

/* ====== OLED 显示模块 ====== */
#include "oled.h"                   /* SSD1306 128x64 OLED 驱动 */

/* ====== 标准 C 库 ====== */
#include <stdio.h>                  /* 格式化输入输出 printf/sprintf */
#include <stdlib.h>                 /* 通用工具函数 malloc/free/atoi */

/* ====== 全局变量 extern 声明 —— 定义在各自的 .c 文件中 ====== */

/* 电机 协议帧缓冲区，通过 UART2(PA21/PA22) 发送给 DCC 驱动器 */
extern uint8_t DCC_v1_2[50];        /* DCC_v1_2: 协议帧数组，由 UART2(PA21/PA22) 发送 */

/* 按键状态标志（消抖后）：0=松开，1=按住 */
extern volatile uint8_t btn1_active;   /* 按键1 PB9  */
extern volatile uint8_t btn2_active;   /* 按键2 PB8  */
extern volatile uint8_t btn3_active;   /* 按键3 PA16 */
extern volatile uint8_t btn4_active;   /* 按键4 PA15 */
extern volatile uint8_t bujin_x;
extern volatile uint8_t dir_x;
extern volatile uint16_t pulse_x;
extern volatile int32_t motor_angle;   /* 电机当前累计角度（度） */
extern volatile int32_t motor_pulse;   /* 电机当前累计脉冲数 */
extern volatile int16_t motor_target;  /* 目标角度（钳位前） */

/* 100Hz PID 节拍：ISR 置位，主循环清零 */
extern volatile uint8_t pid_timer_tick;
#endif
