#ifndef __BUJIN_H
#define __BUJIN_H

/**
 * @file    bujin.h
 * @brief   濮濄儴绻橀悽鍨簚閹貉冨煑濡€虫健婢跺瓨鏋冩禒?
 *
 * 闁俺绻僓ART閸欐垿鈧焦瀵氭禒銈嗗付閸掕埖顒炴潻娑氭暩閺堢儤绮撮崝銊﹀瘹鐎规俺顫楁惔?
 */

/* 鐟欐帒瀹抽幑銏㈢暬缁粯鏆熼敍姘辩椽閻礁娅掗崚鍡氶哺閻?16384) / 360鎼?= 濮ｅ繐瀹崇€电懓绨查惃鍕椽閻礁娅掔拋鈩冩殶 */
#define jiaodu 45.5111f   /* 16384/360 缁墽鈥橀崐?*/

/* 瑙掑害闄愬箙锛毬?掳 */
#define ANGLE_LIMIT 5

/* 閻㈠灚婧€濠婃艾濮╅崙鑺ユ殶閿涙碍顒滈崐闂磋礋濮濓絽鎮滈弮瀣祮閿涘矁绀嬮崐闂磋礋閸欏秴鎮滈弮瀣祮閿涘苯寮弫鏉垮礋娴ｅ秳璐熸惔?*/
void dianji_roll(int16_t roll);

/* 閻㈠灚婧€閼村鍟块崣鎴︹偓浣稿毐閺佸府绱板锝呪偓闂磋礋濮濓絽鎮滈敍宀冪閸婇棿璐熼崣宥呮倻閿涘苯寮弫鏉垮礋娴ｅ秳璐熺紓鏍垳閸ｃ劏鍓﹂崘?*/
void dianji1_pulse(int32_t pulse);

void dianji_roll_pulse(uint16_t pulse, uint8_t dir);

/* 鐠佸墽鐤嗚ぐ鎾冲娴ｅ秶鐤嗘稉鍝勫斧閻愮櫢绱欐潏鍐祩閿涘绱滳MD 0x12 */
void dianji_set_origin(void);

/* 缂佹繂顕弮瀣祮閸掔増瀵氱€规俺顫楁惔锔肩礄閻╃顕禍搴″斧閻愮櫢绱氶敍瀛婱D 0x13 */
void dianji_rotate_to(int16_t angle);

/* 鏉╂柨娲栬ぐ鎾冲缁鳖垵顓哥憴鎺戝閿涘牆瀹抽敍*/
int32_t dianji_get_angle(void);

#endif