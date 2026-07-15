#ifndef __KEY2_H
#define __KEY2_H

/**
 * @file    key2.h
 * @brief   按键2(PA16)检测与初始化模块头文件
 *
 * 提供按键2(PA16)状态的读取和GPIO初始化功能
 */

/* 轮询方式读取按键7(PA16)状态（含消抖），触发电机2滚动 */
void key2_read(void);

/* 读取按键7(PA16)的电平状态，返回1(按下)或0(松开) */
uint8_t get_key7_state(GPIO_Regs * const port, uint32_t key);

/* 初始化按键7(PA16)：配置GPIO为输入下拉模式 */
void key7_init(void);

#endif
