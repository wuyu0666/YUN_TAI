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
} PID_Inc_t;

void  PID_Inc_Init(PID_Inc_t *pid, float Kp, float Ki, float Kd, float deadband,
                   float output_max, float output_min);
float PID_Inc_Calc(PID_Inc_t *pid, float target, float feedback);

#endif
