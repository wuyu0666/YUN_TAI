#ifndef __BUJIN_H
#define __BUJIN_H

/**
 * @file    bujin.h
 * @brief   步进电机控制模块头文件
 *
 * 通过UART发送指令控制步进电机滚动指定角度
 */

/* 角度换算系数：编码器分辨率(16384) / 360度 = 每度对应的编码器计数 */
#define jiaodu 45.5111f   /* 16384/360 精确值 */

/* 电机滚动函数：正值为正向旋转，负值为反向旋转，参数单位为度 */
void dianji_roll(int16_t roll);


void dianji_roll_pulse(uint16_t pulse, uint8_t dir);

#endif