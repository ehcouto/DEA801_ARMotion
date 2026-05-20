/*
 * drv_config.h
 *
 *  Created on: 10 Şub 2025
 *      Author: 26098107
 */

#ifndef COMMON_DRV_DRV_CONFIG_H_
#define COMMON_DRV_DRV_CONFIG_H_

/*
 * Copyright 2021,2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_common.h"
#include "fsl_port.h"
#include "fsl_gpio.h"
#include "pin_mux.h"
#include "board.h"
#include "fsl_lpuart.h"
#include "fsl_lpadc.h"
#include "fsl_lpcmp.h"
#include "fsl_clock.h"
#include "fsl_reset.h"
#include "fsl_spc.h"
#include "fsl_inputmux.h"
#include "fsl_lpspi.h"
#include "fsl_dac.h"
#include "fsl_ctimer.h"
#include <stdbool.h>
#include "freemaster.h"
#include "freemaster_serial_lpuart.h"
#include "fsl_dac.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/


#define SPC_CFG1_USB3v_DET_EN		0x00000002
#define SPC_CFG1_DAC0_EN			0x00000010
#define SPC_CFG1_OPAMP0_EN			0x00000100
#define SPC_CFG1_OPAMP1_EN			0x00000200
#define SPC_CFG1_OPAMP2_EN			0x00000400
#define SPC_CFG1_OPAMP3_EN			0x00000400
#define SPC_CFG1_CMP0_EN			0x00010000
#define SPC_CFG1_CMP1_EN			0x00020000
#define SPC_CFG1_CMP2_EN			0x00040000
#define SPC_CFG1_CMP0_DAC_EN		0x00100000
#define SPC_CFG1_CMP1_DAC_EN		0x00200000
#define SPC_CFG1_CMP2_DAC_EN		0x00400000


/* Enable PWM outputs */
#define PWM_EN(x)        (x->OUTEN |= (PWM_OUTEN_PWMA_EN(0xF) | PWM_OUTEN_PWMB_EN(0xF)))
/* Disable PWM outputs */
#define PWM_DIS(x)       (x->OUTEN &= ~(PWM_OUTEN_PWMA_EN(0xF) | PWM_OUTEN_PWMB_EN(0xF)))


#define PWM_CLOCK_HZ          	 181125000
#define PWM_FREQ_HZ          	 7550U
#define PWM_MODULO          	 PWM_CLOCK_HZ/PWM_FREQ_HZ
#define PWM_DEADTIME          	 271

#define FREQ_SLOWLOOP           1000U


/* ADC CHANNELS DEFINES */
#define ADC0CH_CURR_U			2
#define ADC0CH_VBUS_1			18
#define ADC0CH_VBUS_2			19
#define ADC0CH_3V3_MEAS			15

#define ADC1CH_CURR_V			2
#define ADC1CH_CURR_W			3
#define ADC1CH_IPMTEMP			20

#define ADC2CH_CURR_W			2

#define ADC0CH_DIVERTER			23

#define ADC3CH_SYNCPIN			13


#define FREEMASTER_REC_0_SIZE	1024

#define CTIMER          CTIMER2         /* Timer 2 */
#define CTIMER_MAT0_OUT kCTIMER_Match_0 /* Match output 0 */
#define CTIMER_EMT0_OUT (1u << kCTIMER_Match_0)
#define CTIMER_CLK_FREQ CLOCK_GetCTimerClkFreq(2U)

typedef struct
{
	int16_t i16Iu;
	int16_t i16Ivw;
	int16_t i16Iw;
	int16_t i16Vbus1;
	int16_t i16Vbus2;
	int16_t i16IpmTemp;
	int16_t i16V3v3;
	int16_t i16Diverter;
	int16_t i16SyncPin;
} sAdcResult;

typedef struct
{
	uint32_t ui32Iu;
	uint32_t ui32Ivw;
	uint32_t ui32Iw;
	uint32_t ui32Vbus1;
	uint32_t ui32Vbus2;
	uint32_t ui32IpmTemp;
	uint32_t ui32V3v3;
	uint32_t ui32Diverter;
	uint32_t ui32SyncPin;
} sAdcRawResult;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void InitFlexPWM(void);
void InitADC0(void);
void InitADC1(void);
void InitADC2(void);
void InitADC3(void);
void InitUART(void);
void InitOpamps(void);
void InitComps(void);
void InitInputmux();
void InitDac(void);
void UpdatePWM(int16_t i16PhA, int16_t i16PhB, int16_t i16PhC, PWM_Type *pPWMBase);
void SlowLoopInt();
void ctimer_match0_callback(uint32_t flags);
void InitGpioInt(void);
/*******************************************************************************
 * Variables
 ******************************************************************************/







#endif /* COMMON_DRV_DRV_CONFIG_H_ */
