#ifndef BIHUAN_PID_H
#define BIHUAN_PID_H

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

#endif
