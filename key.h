#ifndef __KEY_H
#define __KEY_H

/**
 * @file    key.h
 * @brief   按键检测与初始化模块头文件
 *
 * 提供按键状态的读取、消抖及PWM中断初始化功能
 */

#include "ti_msp_dl_config.h"
#include "uart.h"
#include "delay.h"

/* 轮询方式读取按键6状态（含消抖），触发电机滚动 */
void key_read(void);

/* 读取指定端口的按键电平状态，返回1(按下)或0(松开) */
uint8_t get_key_state(GPIO_Regs * const port, uint32_t key);

/* 初始化按键6：配置GPIO、使能PWM定时器中断用于消抖扫描 */
void key6_init(void);

#endif