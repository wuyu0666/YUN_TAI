#ifndef __BUJIN2_H
#define __BUJIN2_H

/**
 * @file    bujin2.h
 * @brief   步进电机2控制模块头文件
 *
 * 通过UART1(PB6/PB7)发送指令控制第二个步进电机滚动指定角度
 */

/* 角度换算系数：编码器分辨率(16384) / 360度 = 每度对应的编码器计数 */
#define jiaodu2 (16384/360)

/* 电机2滚动函数：正值为正向旋转，负值为反向旋转，参数单位为度 */
void dianji2_roll(int16_t roll);

#endif
