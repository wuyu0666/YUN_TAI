#ifndef __BUJIN_H
#define __BUJIN_H

#define jiaodu      45.5111f
#define ANGLE_LIMIT 1000

/* ==================================================
 * =============== PID 调参说明（详细） ==============
 * 改这里的值，重新编译烧录即可。
 *
 * ---- 控制结构（TASK2 式双环） ----
 * 位置环：输入 = K230 误差（目标-实时，像素），先限幅 ±60px，
 *         out = Kp*e + Ki*∫e·dt + Kd*de/dt，dt 固定 10ms
 * 速度环：输入 = 球每帧像素位移（目标为 0），做刹车抑过冲
 * 合成：  out = 位置环输出 - 速度环输出，限幅后 ×45.5 转脉冲
 *
 * ---- OLED 怎么看 ----
 * 第1行：E=当前误差(像素)，O=输出脉冲（电机每次修正量）
 * 第2行：P/I/D = 位置环当前 Kp/Ki/Kd（按键调过会变）
 * 正常现象：E 在正负几十像素内来回收窄，O 随误差减小而减小
 *
 * ---- 铁律 ----
 * 1. 一次只改一个参数，烧录试一轮，记下现象和当前值
 * 2. 先确认方向，方向反了怎么调都没用
 * 3. 从保守值起步逐步逼近，不要一次加很大
 * 4. 按键只能加不能减（Btn2=Kp+、Btn3=Kd+、Btn4=Ki+），
 *    想减小只能改这里重新烧录
 *
 * ===== 调试顺序 =====
 * 第1步 方向（必做）：
 *   把球放到目标一侧，看电机是否把球往目标方向推；
 *   反了 -> PID_OUTPUT_REVERSE 改 1
 *
 * 第2步 位置环 Kp（当前 0.347，先以位置环为主）：
 *   现象：追得动但慢 -> 加：0.35 -> 0.4 -> 0.45 -> 0.5
 *   现象：抖 / 冲 / 猛跳 -> 减：0.35 -> 0.3 -> 0.25
 *   目标：球能从一侧稳定靠近目标，不原地小抖
 *
 * 第3步 速度环 Kp（当前 0.05，只有出现过冲才加）：
 *   现象：冲到目标后过冲、来回摆 -> 0.05 -> 0.1 -> 0.15，
 *        加到过冲消失就停
 *   注意：加多了刹车压过驱动，表现为推不动、原地抖
 *
 * 第4步 位置环 Ki（当前 0.15，只有静差才加）：
 *   现象：停在目标附近但总差几像素到不了位 -> 0.15 -> 0.2
 *        -> 0.25，够用就停，别多加（多了会过冲/振荡）
 *
 * 第5步 Kd 保持 0（当前就是 0）：速度环已承担阻尼，
 *   加 Kd 容易高频抖动
 *
 * ===== 故障速查 =====
 * 电机完全不动 / 只轻微抖：
 *   先查方向（PID_OUTPUT_REVERSE），再确认 OLED 上 E 是否
 *   在变化（E 不变说明没收到 K230 反馈），然后加位置环 Kp
 * 一上来就猛冲、输出顶满：
 *   位置环 Kp 太大 -> 减；或误差输入限幅太大 -> 减 ±60px
 * 追到目标后来回摆（过冲）：
 *   按第3步加速度环 Kp
 * 总是差一点到不了位（静差）：
 *   按第4步加位置环 Ki
 * 到位后高频嗡嗡小抖：
 *   死区 PID_DEADBAND_PX 5 -> 8；或位置环 Kp 略减
 * 追球太慢且 O 一直顶满限幅：
 *   确认 Kp 已够大后 -> 调大 PID_POS_OUTPUT_LIMIT_DEG
 *   （30 -> 40），还慢再调大误差输入限幅 ±60px
 *
 * ---- 各限幅含义 ----
 * PID_POS_OUTPUT_LIMIT_DEG：位置环和合成输出最大幅度（度），
 *   30° ≈ 1365 脉冲/帧；太大猛冲，太小追不上
 * PID_VEL_OUTPUT_LIMIT_DEG：速度环刹车最大幅度（度），
 *   27° ≈ 1229 脉冲/帧
 * PID_POS_INPUT_LIMIT_PX：误差超过 ±60px 按 60px 算，
 *   防止单帧误差跳变直接打满输出
 * PID_DEADBAND_PX：误差小于 ±5px 视为到位、停机并清积分，
 *   防止到位后微抖；抖得厉害可调到 8
 * ================================================== */
/* 电机方向反转：1=反向（硬件方向相反时打开） */
#define PID_OUTPUT_REVERSE       0U

/* 位置环增益（追球主力） */
#define PID_POS_KP               0.347f
#define PID_POS_KI               0.05f
#define PID_POS_KD               0.0f

/* 速度环增益（刹车/抑过冲，出现过冲时才从 0.1 起加） */
#define PID_VEL_KP               0.2f
#define PID_VEL_KI               0.0f
#define PID_VEL_KD               0.0f

/* 输出限幅（角度）：位置环/合成输出、速度环输出 */
#define PID_POS_OUTPUT_LIMIT_DEG 30.0f
#define PID_VEL_OUTPUT_LIMIT_DEG 27.0f

/* 位置误差输入限幅（像素） */
#define PID_POS_INPUT_LIMIT_PX   60.0f
/* 死区（像素）：误差小于此值停机 */
#define PID_DEADBAND_PX          5.0f

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
