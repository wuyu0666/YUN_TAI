#ifndef __KEY_H
#define __KEY_H
#include "ti_msp_dl_config.h"
#include "uart.h"
#include "delay.h"
void key_read(void);
uint8_t get_key_state(GPIO_Regs * const port, uint32_t key);
void key6_init(void);
#endif