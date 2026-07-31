#include "GC.h"

/* ---- 鎸夐敭鐘舵€?---- */
volatile uint8_t btn1_active = 0;
volatile uint8_t btn2_active = 0;
volatile uint8_t btn3_active = 0;
volatile uint8_t btn4_active = 0;

/* ---- 100Hz PID 鑺傛媿鏍囧織锛孖SR 缃綅锛屼富寰幆娓呴浂 ---- */
volatile uint8_t pid_timer_tick = 0;

/* ---- PID 闂幆瀹炰緥锛屼富寰幆璁块棶 ---- */
static PID_Inc_t pid_x;

/*
 * 瀹氭椂鍣ㄥ垎棰? DCC_PWM 711Hz / 7 鈮?100Hz
 */
#define PID_TIMER_DIVIDER  3
static uint8_t pid_tick_cnt = 0;

void pid_control_init(void)
{
    PID_Inc_Init(&pid_x, 0.3f, 0.01f, 0.5f, 5.0f, 6000.0f, -6000.0f);
    pid_x.alpha = 0.3f;
    pid_tick_cnt = 0;
    pid_timer_tick = 0;
}

void pid_reset_state(void)
{
    pid_x.error[0] = 0.0f;
    pid_x.error[1] = 0.0f;
    pid_x.error[2] = 0.0f;
    pid_x.deriv_out_prev = 0.0f;
    pid_x.output = 0.0f;
}

void pid_tune_kp(float delta) { pid_x.Kp += delta; }
void pid_tune_ki(float delta) { pid_x.Ki += delta; }
void pid_tune_kd(float delta) { pid_x.Kd += delta; }
void  pid_get_params(float *kp, float *ki, float *kd) { *kp = pid_x.Kp; *ki = pid_x.Ki; *kd = pid_x.Kd; }
float pid_get_kp(void)  { return pid_x.Kp; }
float pid_get_ki(void)  { return pid_x.Ki; }
float pid_get_kd(void)  { return pid_x.Kd; }

/**
 * @brief  涓诲惊鐜皟鐢?PID 璁＄畻锛岃繑鍥?motor delta
 * @param  feedback  褰撳墠 K230 鍙嶉鍊?
 * @param  prev_out  涓婃杈撳嚭鍊硷紙鐢变富寰幆缁存姢锛?
 * @return 鐢垫満鑴夊啿澧為噺
 */
int32_t pid_compute(float feedback, float *prev_out)
{
    if (feedback < pid_x.deadband && feedback > -pid_x.deadband) {
        *prev_out = pid_x.output;
        pid_x.error[0] = 0.0f;
        pid_x.error[1] = 0.0f;
        pid_x.error[2] = 0.0f;
        pid_x.deriv_out_prev = 0.0f;
        return 0;
    }

    float out_x = PID_Inc_Calc(&pid_x, 0.0f, feedback);
    int32_t delta = (int32_t)(out_x - *prev_out
        + ((out_x >= *prev_out) ? 0.5f : -0.5f));
    *prev_out = out_x;
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
