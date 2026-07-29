#ifndef __GC_H
#define __GC_H

/**
 * @file    GC.h
 * @brief   全局头文件汇总
 *
 * 统一包含所有项目依赖与全局变量声明，其他源文件只需 #include "GC.h"
 */

/* ====== 硬件驱动配置（SysConfig 自动生成） ====== */
#include "ti_msp_dl_config.h"       /* MSPM0 外设配置：GPIO/UART/PWM/时钟 */

/* ====== 基础工具模块 ====== */
#include "delay.h"                  /* 毫秒级延时函数 delay_ms() */
#include "uart.h"                   /* 串口收发：UART_send_char/string/buffer */

/* ====== 按键模块 ====== */
#include "key.h"                    /* 按键1(PB9)：get_key_state / key_read / key6_init */

/* ====== 步进电机控制模块 ====== */
#include "bujin.h"                  /* 电机1(UART2/PA21/PA22)：dianji_roll() */

/* ====== PID 控制模块 ====== */
#include "bihuan_pid.h"             /* 增量式 PID 控制器 */

/* ====== OLED 显示模块 ====== */
#include "oled.h"                   /* SSD1306 128x64 OLED 驱动 */

/* ====== 标准 C 库 ====== */
#include <stdio.h>                  /* printf/sprintf 等标准 I/O */
#include <stdlib.h>                 /* malloc/free/atoi 等标准库函数 */

/* ====== 全局变量声明（extern）—— 实际定义在对应 .c 文件中 ====== */

/* 电机通信协议帧缓冲区（定义在 main.c） */
extern uint8_t DCC_v1_2[50];        /* 电机1 协议帧，通过 UART2(PA21/PA22) 发送 */

/* 电机控制标志（1=按住持续转，0=松开停止） */
extern volatile uint8_t motor1_forward_active;   /* 电机1正向 PB9  (定义在 bujin.c)  */
extern volatile uint8_t motor1_reverse_active;   /* 电机1反向 PB8  (定义在 bujin.c)  */
extern volatile uint8_t bujin_x;
extern volatile uint8_t dir_x;
extern volatile uint16_t pulse_x;
#endif
