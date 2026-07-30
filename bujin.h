#ifndef __BUJIN_H
#define __BUJIN_H

/**
 * @file    bujin.h
 * @brief   步进电机驱动模块头文件
 *
 * 通过 UART 发送指令控制 DCC-101v1 闭环步进电机驱动器
 */

/* 角度转脉冲系数：编码器分辨率(16384) / 360° ≈ 45.5111 脉冲/度 */
#define jiaodu 45.5111f   /* 16384/360 编码器脉冲/度 */

/* 角度限幅：±15° */
#define ANGLE_LIMIT 15

/* 驱动电机相对滚动指定角度（正值为正向，负值为反向） */
void dianji_roll(int16_t roll);

/* 驱动电机相对移动指定脉冲数（正值为正向，负值为反向，自动限幅±65535） */
void dianji1_pulse(int32_t pulse);

void dianji_roll_pulse(uint16_t pulse, uint8_t dir);

/* 设置当前位置为原点（校零），CMD 0x12，自动保存至Flash */
void dianji_set_origin(void);

/* 绝对旋转到指定角度（相对于原点），CMD 0x13，自动选择最短路径 */
void dianji_rotate_to(int16_t angle);

/* 返回当前累计角度值（度） */
int32_t dianji_get_angle(void);

#endif