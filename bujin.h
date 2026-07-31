#ifndef __BUJIN_H
#define __BUJIN_H

#define jiaodu      45.5111f
#define ANGLE_LIMIT 1000

/* 鐢垫満椹卞姩 */
void    dianji_roll(int16_t roll);
void    dianji1_pulse(int32_t pulse);
void    dianji1_pulse_fast(int32_t pulse);   /* 蹇€熺増锛岃繛缁璓ID鏃剁敤锛屾棤20ms寤舵椂 */
void    dianji_roll_pulse(uint16_t pulse, uint8_t dir);
void    dianji_set_origin(void);
void    dianji_rotate_to(int16_t angle);
int32_t dianji_get_angle(void);

/* PID 鎺у埗 */
void    pid_control_init(void);
void    pid_reset_state(void);
void    pid_tune_kp(float delta);
void    pid_tune_ki(float delta);
void    pid_tune_kd(float delta);
void    pid_get_params(float *kp, float *ki, float *kd);
float   pid_get_kp(void);
float   pid_get_ki(void);
float   pid_get_kd(void);
int32_t pid_compute(float feedback, float *prev_out);

#endif
