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
    pid->deriv_out_prev = 0.0f;
    pid->alpha = 0.4f;
}

float PID_Inc_Calc(PID_Inc_t *pid, float target, float feedback)
{
    float e0 = target - feedback;

    /* ---- 姝诲尯 ---- */
    if (e0 < pid->deadband && e0 > -pid->deadband) {
        e0 = 0.0f;
    }

    /*
     * 鏍囧噯澧為噺寮?PID 鍏紡:
     *   dU = Kp*(e0-e1) + Ki*e0 + Kd*(e0-2e1+e2)
     *
     * 绉垎鏁堟灉鐢?Ki*e0 绱姞鍒?output 涓嚜鐒朵骇鐢燂紝
     * 涓嶉渶瑕佸澶栫殑绉垎绱姞鍣紒
     */
    float p_term = pid->Kp * (e0 - pid->error[1]);
    float i_term = pid->Ki * e0;
    float d_term_raw = pid->Kd * (e0 - 2.0f * pid->error[1] + pid->error[2]);

    /* 瀵瑰井鍒嗚緭鍑哄仛浣庨€氭护娉紝鑰岄潪婊ゆ尝鍘熷璇樊锛屼繚鐣欐爣鍑嗗叕寮忓舰寮?*/
    float d_term = pid->alpha * d_term_raw + (1.0f - pid->alpha) * pid->deriv_out_prev;
    pid->deriv_out_prev = d_term;

    float dU = p_term + i_term + d_term;

    /* 杈撳嚭鍙樺寲鐜囬檺鍒讹紝闃叉鍗曟璺宠穬 */
    float max_step = pid->output_max * 0.2f;
    if (dU > max_step)  dU = max_step;
    if (dU < -max_step) dU = -max_step;

    pid->output += dU;

    /* 杈撳嚭闄愬箙锛堢畝鍗曟埅鏂嵆鍙紝澧為噺寮忚嚜甯︽姉楗卞拰锛?*/
    if (pid->output > pid->output_max) pid->output = pid->output_max;
    if (pid->output < pid->output_min) pid->output = pid->output_min;

    /* 鏇存柊璇樊鍘嗗彶 */
    pid->error[2] = pid->error[1];
    pid->error[1] = pid->error[0];
    pid->error[0] = e0;

    return pid->output;
}

/* ===== 位置式 PID（TASK2 风格） ===== */

#define PID_POS_DT_S 0.01f   /* 固定采样周期 10ms，与 TASK2 一致 */

void PID_Pos_Init(PID_Pos_t *pid, float kp, float ki, float kd, float output_limit)
{
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->integral = 0.0f;
    pid->previous_error = 0.0f;
    pid->output_limit = output_limit;
    pid->initialized = 0U;
}

float PID_Pos_Calc(PID_Pos_t *pid, float error)
{
    float derivative = 0.0f;   /* 首拍不计算微分 */
    float output;

    if (pid->initialized != 0U) {
        derivative = (error - pid->previous_error) / PID_POS_DT_S;
    } else {
        pid->initialized = 1U;
    }
    pid->integral += error * PID_POS_DT_S;
    output = pid->kp * error + pid->ki * pid->integral + pid->kd * derivative;
    pid->previous_error = error;

    /* 输出对称限幅 */
    if (output > pid->output_limit) {
        output = pid->output_limit;
    } else if (output < -pid->output_limit) {
        output = -pid->output_limit;
    }
    return output;
}
