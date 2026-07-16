/*
 * Copyright (c) 2023, Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ============ ti_msp_dl_config.h =============
 *  Configured MSPM0 DriverLib module declarations
 *
 *  DO NOT EDIT - This file is generated for the MSPM0G350X
 *  by the SysConfig tool.
 */
#ifndef ti_msp_dl_config_h
#define ti_msp_dl_config_h

#define CONFIG_MSPM0G350X
#define CONFIG_MSPM0G3507

#if defined(__ti_version__) || defined(__TI_COMPILER_VERSION__)
#define SYSCONFIG_WEAK __attribute__((weak))
#elif defined(__IAR_SYSTEMS_ICC__)
#define SYSCONFIG_WEAK __weak
#elif defined(__GNUC__)
#define SYSCONFIG_WEAK __attribute__((weak))
#endif

#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m0p/dl_core.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  ======== SYSCFG_DL_init ========
 *  Perform all required MSP DL initialization
 *
 *  This function should be called once at a point before any use of
 *  MSP DL.
 */


/* clang-format off */

#define POWER_STARTUP_DELAY                                                (16)


#define GPIO_HFXT_PORT                                                     GPIOA
#define GPIO_HFXIN_PIN                                             DL_GPIO_PIN_5
#define GPIO_HFXIN_IOMUX                                         (IOMUX_PINCM10)
#define GPIO_HFXOUT_PIN                                            DL_GPIO_PIN_6
#define GPIO_HFXOUT_IOMUX                                        (IOMUX_PINCM11)
#define CPUCLK_FREQ                                                     80000000
/* Defines for SYSPLL_ERR_01 Workaround */
/* Represent 1.000 as 1000 */
#define FLOAT_TO_INT_SCALE                                               (1000U)
#define FCC_EXPECTED_RATIO                                                  2500
#define FCC_UPPER_BOUND                       (FCC_EXPECTED_RATIO * (1 + 0.003))
#define FCC_LOWER_BOUND                       (FCC_EXPECTED_RATIO * (1 - 0.003))

bool SYSCFG_DL_SYSCTL_SYSPLL_init(void);


/* Defines for DCC_PWM */
#define DCC_PWM_INST                                                       TIMA0
#define DCC_PWM_INST_IRQHandler                                 TIMA0_IRQHandler
#define DCC_PWM_INST_INT_IRQN                                   (TIMA0_INT_IRQn)
#define DCC_PWM_INST_CLK_FREQ                                            5000000
/* GPIO defines for channel 0 */
#define GPIO_DCC_PWM_C0_PORT                                               GPIOA
#define GPIO_DCC_PWM_C0_PIN                                        DL_GPIO_PIN_0
#define GPIO_DCC_PWM_C0_IOMUX                                     (IOMUX_PINCM1)
#define GPIO_DCC_PWM_C0_IOMUX_FUNC                    IOMUX_PINCM1_PF_TIMA0_CCP0
#define GPIO_DCC_PWM_C0_IDX                                  DL_TIMER_CC_0_INDEX




/* Defines for OLED */
#define OLED_INST                                                           I2C1
#define OLED_INST_IRQHandler                                     I2C1_IRQHandler
#define OLED_INST_INT_IRQN                                         I2C1_INT_IRQn
#define OLED_BUS_SPEED_HZ                                                 100000
#define GPIO_OLED_SDA_PORT                                                 GPIOB
#define GPIO_OLED_SDA_PIN                                          DL_GPIO_PIN_3
#define GPIO_OLED_IOMUX_SDA                                      (IOMUX_PINCM16)
#define GPIO_OLED_IOMUX_SDA_FUNC                       IOMUX_PINCM16_PF_I2C1_SDA
#define GPIO_OLED_SCL_PORT                                                 GPIOB
#define GPIO_OLED_SCL_PIN                                          DL_GPIO_PIN_2
#define GPIO_OLED_IOMUX_SCL                                      (IOMUX_PINCM15)
#define GPIO_OLED_IOMUX_SCL_FUNC                       IOMUX_PINCM15_PF_I2C1_SCL


/* Defines for PRINT */
#define PRINT_INST                                                         UART0
#define PRINT_INST_FREQUENCY                                            40000000
#define PRINT_INST_IRQHandler                                   UART0_IRQHandler
#define PRINT_INST_INT_IRQN                                       UART0_INT_IRQn
#define GPIO_PRINT_RX_PORT                                                 GPIOA
#define GPIO_PRINT_TX_PORT                                                 GPIOA
#define GPIO_PRINT_RX_PIN                                         DL_GPIO_PIN_31
#define GPIO_PRINT_TX_PIN                                         DL_GPIO_PIN_28
#define GPIO_PRINT_IOMUX_RX                                       (IOMUX_PINCM6)
#define GPIO_PRINT_IOMUX_TX                                       (IOMUX_PINCM3)
#define GPIO_PRINT_IOMUX_RX_FUNC                        IOMUX_PINCM6_PF_UART0_RX
#define GPIO_PRINT_IOMUX_TX_FUNC                        IOMUX_PINCM3_PF_UART0_TX
#define PRINT_BAUD_RATE                                                 (115200)
#define PRINT_IBRD_40_MHZ_115200_BAUD                                       (21)
#define PRINT_FBRD_40_MHZ_115200_BAUD                                       (45)
/* Defines for DCC101_v1_2 */
#define DCC101_v1_2_INST                                                   UART2
#define DCC101_v1_2_INST_FREQUENCY                                      40000000
#define DCC101_v1_2_INST_IRQHandler                             UART2_IRQHandler
#define DCC101_v1_2_INST_INT_IRQN                                 UART2_INT_IRQn
#define GPIO_DCC101_v1_2_RX_PORT                                           GPIOA
#define GPIO_DCC101_v1_2_TX_PORT                                           GPIOA
#define GPIO_DCC101_v1_2_RX_PIN                                   DL_GPIO_PIN_22
#define GPIO_DCC101_v1_2_TX_PIN                                   DL_GPIO_PIN_21
#define GPIO_DCC101_v1_2_IOMUX_RX                                (IOMUX_PINCM47)
#define GPIO_DCC101_v1_2_IOMUX_TX                                (IOMUX_PINCM46)
#define GPIO_DCC101_v1_2_IOMUX_RX_FUNC                 IOMUX_PINCM47_PF_UART2_RX
#define GPIO_DCC101_v1_2_IOMUX_TX_FUNC                 IOMUX_PINCM46_PF_UART2_TX
#define DCC101_v1_2_BAUD_RATE                                           (115200)
#define DCC101_v1_2_IBRD_40_MHZ_115200_BAUD                                 (21)
#define DCC101_v1_2_FBRD_40_MHZ_115200_BAUD                                 (45)
/* Defines for DCC101_v1_3 */
#define DCC101_v1_3_INST                                                   UART1
#define DCC101_v1_3_INST_FREQUENCY                                      40000000
#define DCC101_v1_3_INST_IRQHandler                             UART1_IRQHandler
#define DCC101_v1_3_INST_INT_IRQN                                 UART1_INT_IRQn
#define GPIO_DCC101_v1_3_RX_PORT                                           GPIOB
#define GPIO_DCC101_v1_3_TX_PORT                                           GPIOB
#define GPIO_DCC101_v1_3_RX_PIN                                    DL_GPIO_PIN_7
#define GPIO_DCC101_v1_3_TX_PIN                                    DL_GPIO_PIN_6
#define GPIO_DCC101_v1_3_IOMUX_RX                                (IOMUX_PINCM24)
#define GPIO_DCC101_v1_3_IOMUX_TX                                (IOMUX_PINCM23)
#define GPIO_DCC101_v1_3_IOMUX_RX_FUNC                 IOMUX_PINCM24_PF_UART1_RX
#define GPIO_DCC101_v1_3_IOMUX_TX_FUNC                 IOMUX_PINCM23_PF_UART1_TX
#define DCC101_v1_3_BAUD_RATE                                           (115200)
#define DCC101_v1_3_IBRD_40_MHZ_115200_BAUD                                 (21)
#define DCC101_v1_3_FBRD_40_MHZ_115200_BAUD                                 (45)
/* Defines for K230 */
#define K230_INST                                                          UART3
#define K230_INST_FREQUENCY                                             80000000
#define K230_INST_IRQHandler                                    UART3_IRQHandler
#define K230_INST_INT_IRQN                                        UART3_INT_IRQn
#define GPIO_K230_RX_PORT                                                  GPIOA
#define GPIO_K230_TX_PORT                                                  GPIOA
#define GPIO_K230_RX_PIN                                          DL_GPIO_PIN_13
#define GPIO_K230_TX_PIN                                          DL_GPIO_PIN_26
#define GPIO_K230_IOMUX_RX                                       (IOMUX_PINCM35)
#define GPIO_K230_IOMUX_TX                                       (IOMUX_PINCM59)
#define GPIO_K230_IOMUX_RX_FUNC                        IOMUX_PINCM35_PF_UART3_RX
#define GPIO_K230_IOMUX_TX_FUNC                        IOMUX_PINCM59_PF_UART3_TX
#define K230_BAUD_RATE                                                  (115200)
#define K230_IBRD_80_MHZ_115200_BAUD                                        (43)
#define K230_FBRD_80_MHZ_115200_BAUD                                        (26)





/* Port definition for Pin Group LED */
#define LED_PORT                                                         (GPIOA)

/* Defines for LED0: GPIOA.14 with pinCMx 36 on package pin 7 */
#define LED_LED0_PIN                                            (DL_GPIO_PIN_14)
#define LED_LED0_IOMUX                                           (IOMUX_PINCM36)
/* Port definition for Pin Group anjian6 */
#define anjian6_PORT                                                     (GPIOB)

/* Defines for PIN_0: GPIOB.9 with pinCMx 26 on package pin 61 */
#define anjian6_PIN_0_PIN                                        (DL_GPIO_PIN_9)
#define anjian6_PIN_0_IOMUX                                      (IOMUX_PINCM26)
/* Port definition for Pin Group anjian7 */
#define anjian7_PORT                                                     (GPIOA)

/* Defines for PIN_7: GPIOA.16 with pinCMx 38 on package pin 9 */
#define anjian7_PIN_7_PIN                                       (DL_GPIO_PIN_16)
#define anjian7_PIN_7_IOMUX                                      (IOMUX_PINCM38)
/* Port definition for Pin Group fan1 */
#define fan1_PORT                                                        (GPIOB)

/* Defines for PIN_1: GPIOB.8 with pinCMx 25 on package pin 60 */
#define fan1_PIN_1_PIN                                           (DL_GPIO_PIN_8)
#define fan1_PIN_1_IOMUX                                         (IOMUX_PINCM25)
/* Port definition for Pin Group fan2 */
#define fan2_PORT                                                        (GPIOA)

/* Defines for PIN_2: GPIOA.15 with pinCMx 37 on package pin 8 */
#define fan2_PIN_2_PIN                                          (DL_GPIO_PIN_15)
#define fan2_PIN_2_IOMUX                                         (IOMUX_PINCM37)


/* clang-format on */

void SYSCFG_DL_init(void);
void SYSCFG_DL_initPower(void);
void SYSCFG_DL_GPIO_init(void);
void SYSCFG_DL_SYSCTL_init(void);

bool SYSCFG_DL_SYSCTL_SYSPLL_init(void);
void SYSCFG_DL_DCC_PWM_init(void);
void SYSCFG_DL_OLED_init(void);
void SYSCFG_DL_PRINT_init(void);
void SYSCFG_DL_DCC101_v1_2_init(void);
void SYSCFG_DL_DCC101_v1_3_init(void);
void SYSCFG_DL_K230_init(void);


bool SYSCFG_DL_saveConfiguration(void);
bool SYSCFG_DL_restoreConfiguration(void);

#ifdef __cplusplus
}
#endif

#endif /* ti_msp_dl_config_h */
