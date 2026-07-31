#include "GC.h"

/* ---- 鎸夐敭鐘舵€?---- */
volatile uint8_t btn1_active = 0;
volatile uint8_t btn2_active = 0;
volatile uint8_t btn3_active = 0;
volatile uint8_t btn4_active = 0;

/* ---- 100Hz PID 鑺傛媿鏍囧織锛孖SR 缃綅锛屼富寰幆娓呴浂 ---- */
volatile uint8_t pid_timer_tick = 0;

/* ---- PID 闂幆瀹炰緥锛屼富寰幆璁块棶 ---- */
/* ===== 双环 PID（TASK2 式：位置环 + 速度环），参数统一在 bujin.h 调参区修改 ===== */

/* 位置环 PID 实例（按键调参与 OLED 显示都读它） */
static PID_Pos_t s_pos_pid;
/* 速度环 PID 实例 */
static PID_Pos_t s_vel_pid;
/* 速度估算状态：上一帧误差、是否已记录、球速（像素/帧） */
static float   s_prev_error;
static uint8_t s_has_prev_error;
static float   s_ball_velocity;

/*
 * 瀹氭椂鍣ㄥ垎棰? DCC_PWM 711Hz / 7 鈮?100Hz
 */
#define PID_TIMER_DIVIDER  3
static uint8_t pid_tick_cnt = 0;

void pid_control_init(void)
{
    /* 增益在 bujin.h 的 PID 调参区统一修改 */
    PID_Pos_Init(&s_pos_pid, PID_POS_KP, PID_POS_KI, PID_POS_KD,
                 PID_POS_OUTPUT_LIMIT_DEG);
    PID_Pos_Init(&s_vel_pid, PID_VEL_KP, PID_VEL_KI, PID_VEL_KD,
                 PID_VEL_OUTPUT_LIMIT_DEG);
    s_prev_error = 0.0f;
    s_has_prev_error = 0U;
    s_ball_velocity = 0.0f;
    pid_tick_cnt = 0;
    pid_timer_tick = 0;
}

/* 复位双环 PID 状态（保留已调好的增益） */
static void pid_pos_reset_all(void)
{
    PID_Pos_Init(&s_pos_pid, s_pos_pid.kp, s_pos_pid.ki, s_pos_pid.kd,
                 PID_POS_OUTPUT_LIMIT_DEG);
    PID_Pos_Init(&s_vel_pid, s_vel_pid.kp, s_vel_pid.ki, s_vel_pid.kd,
                 PID_VEL_OUTPUT_LIMIT_DEG);
    s_prev_error = 0.0f;
    s_has_prev_error = 0U;
    s_ball_velocity = 0.0f;
}

void pid_reset_state(void)
{
    pid_pos_reset_all();
}

void pid_tune_kp(float delta) { s_pos_pid.kp += delta; }
void pid_tune_ki(float delta) { s_pos_pid.ki += delta; }
void pid_tune_kd(float delta) { s_pos_pid.kd += delta; }
void  pid_get_params(float *kp, float *ki, float *kd) { *kp = s_pos_pid.kp; *ki = s_pos_pid.ki; *kd = s_pos_pid.kd; }
float pid_get_kp(void)  { return s_pos_pid.kp; }
float pid_get_ki(void)  { return s_pos_pid.ki; }
float pid_get_kd(void)  { return s_pos_pid.kd; }

/**
 * @brief  涓诲惊鐜皟鐢?PID 璁＄畻锛岃繑鍥?motor delta
 * @param  feedback  褰撳墠 K230 鍙嶉鍊?
 * @param  prev_out  涓婃杈撳嚭鍊硷紙鐢变富寰幆缁存姢锛?
 * @return 鐢垫満鑴夊啿澧為噺
 */
int32_t pid_compute(float feedback, float *prev_out)
{
    float pos_error;   /* 位置环误差（像素，限幅后） */
    float pos_out;     /* 位置环输出（角度） */
    float vel_out;     /* 速度环输出（角度） */
    float out_deg;     /* 合成输出（角度） */
    float out_pulse;   /* 换算后的输出（脉冲） */
    int32_t delta;     /* 与上次输出的差值（脉冲） */

    /* 死区：误差足够小视为到位，清零双环状态，电机停止 */
    if (feedback < PID_DEADBAND_PX && feedback > -PID_DEADBAND_PX) {
        pid_pos_reset_all();
        return 0;
    }

    /* 速度估算：只有误差变化（收到新帧）才更新球速；
     * 帧间误差不变时保持上次球速，保证刹车持续有效（同 TASK2） */
    if (s_has_prev_error == 0U) {
        s_prev_error = feedback;      /* 首帧只记录，不算速度 */
        s_has_prev_error = 1U;
    } else if (feedback != s_prev_error) {
        s_ball_velocity = s_prev_error - feedback;
        s_prev_error = feedback;
    }

    /* 位置环：误差限幅 ±60px 后进 PID，输出角度 */
    pos_error = feedback;
    if (pos_error > PID_POS_INPUT_LIMIT_PX) {
        pos_error = PID_POS_INPUT_LIMIT_PX;
    } else if (pos_error < -PID_POS_INPUT_LIMIT_PX) {
        pos_error = -PID_POS_INPUT_LIMIT_PX;
    }
    pos_out = PID_Pos_Calc(&s_pos_pid, pos_error);

    /* 速度环：目标球速 0，球滚得越快反向抵消越大（刹车） */
    vel_out = PID_Pos_Calc(&s_vel_pid, s_ball_velocity);

    /* 合成：位置环定方向，速度环作刹车，再限幅（角度） */
    out_deg = pos_out - vel_out;
    if (out_deg > PID_POS_OUTPUT_LIMIT_DEG) {
        out_deg = PID_POS_OUTPUT_LIMIT_DEG;
    } else if (out_deg < -PID_POS_OUTPUT_LIMIT_DEG) {
        out_deg = -PID_POS_OUTPUT_LIMIT_DEG;
    }

    /* 角度 -> 脉冲 */
    out_pulse = out_deg * jiaodu;
    if (PID_OUTPUT_REVERSE != 0U) {
        out_pulse = -out_pulse;
    }

    /* 差值输出：电机位置跟踪 out_pulse（等价于 TASK2 的绝对角度输出） */
    delta = (int32_t)(out_pulse - *prev_out
        + ((out_pulse >= *prev_out) ? 0.5f : -0.5f));
    *prev_out = out_pulse;
    return delta;
}

/**----------------------------------------------------------
 * 鐢垫満椹卞姩鎺ュ彛
 *----------------------------------------------------------*/
void dianji_roll(int16_t roll)
{
    uint8_t fangxiang=0x01;
    if(roll<=0) { roll=-roll; fangxiang=0x00; }
    DCC_v1_2[8]=((uint16_t)(jiaodu*roll))&0xFF;
    DCC_v1_2[9]=(((uint16_t)(jiaodu*roll))>>8)&0xFF;
    DCC_v1_2[5]=fangxiang;
    uint8_t cs=0; for(int i=2;i<10;i++) cs+=DCC_v1_2[i];
    DCC_v1_2[10]=cs;
    uint8_t wu[]={0xAA,0xAA,0xAA};
    UART_send_buffer(DCC101_v1_2_INST,wu,3); delay_ms(20);
    UART_send_buffer(DCC101_v1_2_INST,DCC_v1_2,DCC_v1_2[4]+6);
}

/*
 * dianji1_pulse_fast 鈥斺€?蹇€熺増锛屽幓鎺?delay_ms(20)
 * 杩炵画 PID 鎺у埗鏃?DCC 椹卞姩鍣ㄥ凡鍦ㄧ嚎锛屼笉闇€瑕佸敜閱?20ms 绛夊緟
 * 鍙湁闀挎椂闂存病鍙戦€佸懡浠ゅ悗鎵嶉渶瑕佸甫寤舵椂鐨勭増鏈?
 */
static uint8_t dcc_was_active = 0;  /* DCC 鏄惁鍦ㄧ嚎鐘舵€?*/

void dianji1_pulse_fast(int32_t pulse)
{
    if (pulse > 65535) pulse = 65535;
    if (pulse < -65535) pulse = -65535;
    uint8_t fangxiang = (pulse > 0) ? 0x01 : 0x00;
    if (pulse < 0) pulse = -pulse;

    DCC_v1_2[0]=0xAA; DCC_v1_2[1]=0x55; DCC_v1_2[2]=0x01;
    DCC_v1_2[3]=0x11; DCC_v1_2[4]=0x05;
    DCC_v1_2[5]=fangxiang;
    DCC_v1_2[6]=0x00; DCC_v1_2[7]=0x00;
    DCC_v1_2[8]=pulse&0xFF;
    DCC_v1_2[9]=(pulse>>8)&0xFF;
    uint8_t cs=0; for(int i=2;i<10;i++) cs+=DCC_v1_2[i];
    DCC_v1_2[10]=cs;

    if (!dcc_was_active) {
        /* 棣栨鍙戦€佹垨闀挎椂闂撮棿闅旓細鍙戦€佸敜閱?+ 绛夊緟 */
        uint8_t wu[]={0xAA,0xAA,0xAA};
        UART_send_buffer(DCC101_v1_2_INST,wu,3);
        delay_ms(20);
        dcc_was_active = 1;
    }
    UART_send_buffer(DCC101_v1_2_INST,DCC_v1_2,DCC_v1_2[4]+6);
}

void dianji1_pulse(int32_t pulse)
{
    if (pulse > 65535) pulse = 65535;
    if (pulse < -65535) pulse = -65535;
    uint8_t fangxiang=0x01;
    if(pulse<=0) { pulse=-pulse; fangxiang=0x00; }
    DCC_v1_2[8]=pulse&0xFF;
    DCC_v1_2[9]=(pulse>>8)&0xFF;
    DCC_v1_2[5]=fangxiang;
    uint8_t cs=0; for(int i=2;i<10;i++) cs+=DCC_v1_2[i];
    DCC_v1_2[10]=cs;
    uint8_t wu[]={0xAA,0xAA,0xAA};
    UART_send_buffer(DCC101_v1_2_INST,wu,3); delay_ms(20);
    UART_send_buffer(DCC101_v1_2_INST,DCC_v1_2,DCC_v1_2[4]+6);
}

void dianji_roll_pulse(uint16_t pulse, uint8_t dir)
{
    DCC_v1_2[0]=0xAA; DCC_v1_2[1]=0x55; DCC_v1_2[2]=0x01;
    DCC_v1_2[3]=0x11; DCC_v1_2[4]=0x05; DCC_v1_2[5]=dir;
    DCC_v1_2[6]=0x00; DCC_v1_2[7]=0x00;
    DCC_v1_2[8]=pulse&0xFF; DCC_v1_2[9]=(pulse>>8)&0xFF;
    uint8_t cs=0; for(int i=2;i<10;i++) cs+=DCC_v1_2[i];
    DCC_v1_2[10]=cs;
    uint8_t wu[]={0xAA,0xAA,0xAA};
    UART_send_buffer(DCC101_v1_2_INST,wu,3); delay_ms(20);
    UART_send_buffer(DCC101_v1_2_INST,DCC_v1_2,DCC_v1_2[4]+6);
}

void dianji_set_origin(void)
{
    DCC_v1_2[0]=0xAA; DCC_v1_2[1]=0x55; DCC_v1_2[2]=0x01;
    DCC_v1_2[3]=0x12; DCC_v1_2[4]=0x00;
    uint8_t cs=0; for(int i=2;i<5;i++) cs+=DCC_v1_2[i];
    DCC_v1_2[5]=cs;
    uint8_t wu[]={0xAA,0xAA,0xAA};
    UART_send_buffer(DCC101_v1_2_INST,wu,3); delay_ms(20);
    UART_send_buffer(DCC101_v1_2_INST,DCC_v1_2,6);
    motor_angle = 0;
    motor_pulse = 0;
    dcc_was_active = 0;  /* 褰掗浂鍚庨渶瑕侀噸鏂板敜閱?*/
}

void dianji_rotate_to(int16_t angle)
{
    if(angle > ANGLE_LIMIT)  angle = ANGLE_LIMIT;
    if(angle < -ANGLE_LIMIT) angle = -ANGLE_LIMIT;
    int16_t target = angle % 360;
    if(target < 0) target += 360;
    int16_t current = motor_angle % 360;
    if(current < 0) current += 360;
    int16_t diff = target - current;
    uint8_t dir;
    if(diff > 180) dir = 0x00;
    else if(diff < -180) dir = 0x01;
    else if(diff >= 0) dir = 0x01;
    else dir = 0x00;
    uint16_t pulse = (uint16_t)(jiaodu * (float)target);
    DCC_v1_2[0]=0xAA; DCC_v1_2[1]=0x55; DCC_v1_2[2]=0x01;
    DCC_v1_2[3]=0x13; DCC_v1_2[4]=0x03;
    DCC_v1_2[5]=dir;
    DCC_v1_2[6]=pulse & 0xFF;
    DCC_v1_2[7]=(pulse >> 8) & 0xFF;
    uint8_t cs=0; for(int i=2;i<8;i++) cs+=DCC_v1_2[i];
    DCC_v1_2[8]=cs;
    uint8_t wu[]={0xAA,0xAA,0xAA};
    UART_send_buffer(DCC101_v1_2_INST,wu,3); delay_ms(20);
    UART_send_buffer(DCC101_v1_2_INST,DCC_v1_2,9);
    motor_angle = angle;
    motor_pulse = (int32_t)pulse * (dir==0x01 ? 1 : -1);
}

int32_t dianji_get_angle(void) { return motor_angle; }

/**
 * @brief  瀹氭椂鍣ㄤ腑鏂細鎸夐敭娑堟姈 + 100Hz PID 鑺傛媿
 */
void DCC_PWM_INST_IRQHandler()
{
    static uint8_t  btn_stable  = 0, btn_cnt     = 0;
    static uint8_t  btn2_stable = 0, btn2_cnt    = 0;
    static uint8_t  btn3_stable = 0, btn3_cnt    = 0;
    static uint8_t  btn4_stable = 0, btn4_cnt    = 0;

    switch (DL_Timer_getPendingInterrupt(DCC_PWM_INST))
    {
        case DL_TIMER_IIDX_LOAD:
        {
            /* ====== 鎸夐敭娑堟姈 ====== */
            uint8_t k;
            k = get_key_state(anjian6_PORT, anjian6_PIN_0_PIN);
            if (k == btn_stable) btn_cnt = 0;
            else { btn_cnt++; if (btn_cnt >= 15) { btn_stable = k; btn_cnt = 0; } }

            k = get_key_state(fan1_PORT, fan1_PIN_1_PIN);
            if (k == btn2_stable) btn2_cnt = 0;
            else { btn2_cnt++; if (btn2_cnt >= 15) { btn2_stable = k; btn2_cnt = 0; } }

            k = get_key_state(anjian7_PORT, anjian7_PIN_7_PIN);
            if (k == btn3_stable) btn3_cnt = 0;
            else { btn3_cnt++; if (btn3_cnt >= 15) { btn3_stable = k; btn3_cnt = 0; } }

            k = get_key_state(fan2_PORT, fan2_PIN_2_PIN);
            if (k == btn4_stable) btn4_cnt = 0;
            else { btn4_cnt++; if (btn4_cnt >= 15) { btn4_stable = k; btn4_cnt = 0; } }

            btn1_active = btn_stable;
            btn2_active = btn2_stable;
            btn3_active = btn3_stable;
            btn4_active = btn4_stable;

            /* ====== 100Hz PID 鑺傛媿 ====== */
            pid_tick_cnt++;
            if (pid_tick_cnt >= PID_TIMER_DIVIDER) {
                pid_tick_cnt = 0;
                pid_timer_tick = 1;
            }
            break;
        }
        default: break;
    }
}
