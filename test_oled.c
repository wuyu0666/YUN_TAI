#include "ti_msp_dl_config.h"
#include "oled.h"
#include "delay.h"
#include <stdio.h>

static char disp_buf[32];

int main(void)
{
    /* ---- System + OLED init ---- */
    SYSCFG_DL_init();
    OLED_Init();
    OLED_ColorTurn(0);
    OLED_DisplayTurn(0);
    OLED_Clear();

    /* ---- Startup screen ---- */
    OLED_ShowString(0, 0,  (u8 *)"OLED TEST OK", 12);
    OLED_ShowString(0, 16, (u8 *)"No Motor/UART", 12);
    OLED_ShowString(0, 32, (u8 *)"System Ready",  12);
    OLED_Refresh();
    delay_ms(1500);

    OLED_Clear();

    uint8_t cnt = 0;
    while (1)
    {
        sprintf(disp_buf, "Cnt:%03d", cnt);
        OLED_ShowString(0, 0,  (u8 *)disp_buf, 12);
        OLED_ShowString(0, 16, (u8 *)"Running...", 12);
        OLED_ShowString(0, 32, (u8 *)"Hello OLED!", 12);
        OLED_ShowNum(0, 48, cnt % 10, 2, 12);

        OLED_Refresh();
        delay_ms(500);
        cnt++;
    }
}