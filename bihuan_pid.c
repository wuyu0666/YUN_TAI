#include "bihuan_pid.h"

void PID_Inc_Init(PID_Inc_t *pid, float Kp, float Ki, float Kd, float deadband,
                   float output_max, float output_min)
{
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    pid->deadband = deadband;
    pid->error[0] = 0.0f;
    pid->error[1] = 0.0f;
    pid->error[2] = 0.0f;
    pid->output   = 0.0f;
    pid->output_max = output_max;
    pid->output_min = output_min;
}

float PID_Inc_Calc(PID_Inc_t *pid, float target, float feedback)
{
    float e0 = target - feedback;

    if (e0 < pid->deadband && e0 > -pid->deadband) {
        e0 = 0.0f;
    }

    float dU = pid->Kp * (e0 - pid->error[1])
             + pid->Ki * e0
             + pid->Kd * (e0 - 2.0f * pid->error[1] + pid->error[2]);

    pid->output  += dU;
    if (pid->output > pid->output_max) pid->output = pid->output_max;
    if (pid->output < pid->output_min) pid->output = pid->output_min;
    pid->error[2] = pid->error[1];
    pid->error[1] = pid->error[0];
    pid->error[0] = e0;

    return pid->output;
}
