#ifndef BIHUAN_PID_H
#define BIHUAN_PID_H

#include <stdint.h>

typedef struct {
    float Kp;
    float Ki;
    float Kd;
    float error[3];
    float output;
    float deadband;
    float output_max;
    float output_min;
    float deriv_out_prev;  /* 涓婁竴娆″井鍒嗚緭鍑猴紝鐢ㄤ簬浣庨€氭护娉?*/
    float alpha;           /* 寰垎椤逛綆閫氭护娉㈢郴鏁?(0~1锛岃秺灏忔护娉㈣秺寮?*/
} PID_Inc_t;

void  PID_Inc_Init(PID_Inc_t *pid, float Kp, float Ki, float Kd, float deadband,
                   float output_max, float output_min);
float PID_Inc_Calc(PID_Inc_t *pid, float target, float feedback);

/* ===== 位置式 PID（TASK2 风格）：out = kp*e + ki*integral + kd*derivative ===== */
typedef struct {
    float kp;              /* 比例增益 */
    float ki;              /* 积分增益（1/s） */
    float kd;              /* 微分增益（s） */
    float integral;        /* 误差×时间 的累积（积分项） */
    float previous_error;  /* 上一次误差（用于微分项） */
    float output_limit;    /* 输出对称限幅（角度或脉冲域） */
    uint8_t initialized;   /* 是否已初始化（首拍不计算微分） */
} PID_Pos_t;

void  PID_Pos_Init(PID_Pos_t *pid, float kp, float ki, float kd, float output_limit);
float PID_Pos_Calc(PID_Pos_t *pid, float error);

#endif
