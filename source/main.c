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
#include "fsl_clock.h"
#include "fsl_reset.h"
#include "fsl_inputmux.h"
#include "fsl_lpspi.h"
#include <stdbool.h>
#include "freemaster.h"
#include "freemaster_serial_lpuart.h"
#include "mlib_FP.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Enable PWM outputs */
#define PWM_EN(x)        (x->OUTEN |= (PWM_OUTEN_PWMA_EN(0xF) | PWM_OUTEN_PWMB_EN(0xF)))
/* Disable PWM outputs */
#define PWM_DIS(x)       (x->OUTEN &= ~(PWM_OUTEN_PWMA_EN(0xF) | PWM_OUTEN_PWMB_EN(0xF)))

#define MAINBOARD_COMM_UART_BAUDRATE	115200
#define MAINBOARD_COMM         	 		LPUART2
#define MAINBOARD_COMM_LPUART_CLK_FREQ (BOARD_DEBUG_UART_CLK_FREQ)

#define SPI_TRANSFER_SIZE     (512U)    /*! Transfer dataSize .*/
#define SPI_TRANSFER_BAUDRATE (500000U) /*! Transfer baudrate - 500k */

#define PWM_CLOCK_HZ          	 181125000
#define PWM_FREQ_HZ          	 16000
#define PWM_MODULO          	 PWM_CLOCK_HZ/PWM_FREQ_HZ
#define PWM_DEADTIME          	 100

#define INRUSH_REL_GPIO			GPIO3
#define INRUSH_REL_PIN			31U

#define LED_GPIO				GPIO3
#define LED_PIN					18U

#define DC_BUS_HW_PROT_GPIO		GPIO1
#define DC_BUS_HW_PROT_PORT		PORT1
#define DC_BUS_HW_PROT_PIN		13U

/* ADC CHANNELS DEFINES */
#define ADC0CH_CURR_A			7
#define ADC0CH_CURR_B			1
#define ADC1CH_CURR_B			8
#define ADC1CH_CURR_C			21
#define ADC0CH_VBUS_1			20
#define ADC0CH_VBUS_2			19
#define ADC0CH_BEMFA			22
#define ADC1CH_BEMFB			12
#define ADC1CH_BEMFC			1
#define ADC0CH_IPMTEMP			21
#define ADC1CH_GP_ANALOG		12


#define DCB_VOLT_SCALE			400.0
#define FREEMASTER_REC_0_SIZE	1024
typedef struct
{
	int16_t i16Ia;
	int16_t i16Ib;
	int16_t i16Ic;
	int16_t i16Vbus1;
	int16_t i16Vbus2;
	int16_t i16IpmTemp;
	int16_t i16BemfA;
	int16_t i16BemfB;
	int16_t i16BemfC;
	int16_t i16GpAnalog;
} sAdcResult;

typedef struct
{
	uint32_t ui32Ia;
	uint32_t ui32Ib;
	uint32_t ui32Ic;
	uint32_t ui32Vbus1;
	uint32_t ui32Vbus2;
	uint32_t ui32IpmTemp;
	uint32_t ui32BemfA;
	uint32_t ui32BemfB;
	uint32_t ui32BemfC;
	uint32_t ui32GpAnalog;
} sAdcRawResult;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void InitFlexPWM(void);
static void InitADC0(void);
static void InitADC1(void);
static void InitUART(void);
static void InitSPI(void);
static void InitInputmux(void);
static void InitPins(void);
static void UpdatePWM(int16_t i16PhA, int16_t i16PhB, int16_t i16PhC, PWM_Type *pPWMBase);

/*******************************************************************************
 * Variables
 ******************************************************************************/

uint8_t txbuff[]   = "Lpuart polling example\r\nBoard will send back received characters\r\n";
uint8_t rxbuff[20] = {0};

volatile uint32_t ui32Fcount0;
volatile uint32_t ui32Fcount1;
int16_t  i16DutyA;
int16_t  i16DutyB;
int16_t  i16DutyC;
uint16_t ui16PwmFault;
uint32_t ui32AdcIsrCnt;
sAdcRawResult sAdcRaw;
sAdcResult sAdcRes;
float_t fVdcBus;
float_t fltDcBusVoltageScale;
uint16_t  ui16Modulo;
FMSTR_U8 FreeMASTER_RecBuffer0[FREEMASTER_REC_0_SIZE];

FMSTR_REC_BUFF FreeMASTER_Recorder_0 = {
  .name = "Description of recorder 0",
  .addr = (FMSTR_ADDR)FreeMASTER_RecBuffer0,
  .size = FREEMASTER_REC_0_SIZE,
  .basePeriod_ns = 100000UL
};


/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief Main function
 */
int main(void)
{
    uint32_t ui32PrimaskReg;

    ui32PrimaskReg = DisableGlobalIRQ();
    fltDcBusVoltageScale = DCB_VOLT_SCALE;
    i16DutyA = 16000;
	i16DutyB = 25000;
	i16DutyC = 30000;
	BOARD_BootClockFROHF180M();
	ui16Modulo = PWM_MODULO;
    InitPins();
    InitADC0();
    InitADC1();
    InitFlexPWM();
    InitUART();
    InitSPI();
    InitInputmux();
    FMSTR_Init();
    FMSTR_RecorderCreate(0, &FreeMASTER_Recorder_0);
    /* Example of inrush toggle  - for debug step purposes */
    GPIO_PortToggle(INRUSH_REL_GPIO, 1u << INRUSH_REL_PIN);
    GPIO_PortToggle(INRUSH_REL_GPIO, 1u << INRUSH_REL_PIN);
    GPIO_PortToggle(INRUSH_REL_GPIO, 1u << INRUSH_REL_PIN);
    GPIO_PortSet(INRUSH_REL_GPIO, 1u << INRUSH_REL_PIN);
    /* Example of LED toggle - for debug step purposes */
    GPIO_PortToggle(LED_GPIO, 1u << LED_PIN);
    GPIO_PortToggle(LED_GPIO, 1u << LED_PIN);
    GPIO_PortToggle(LED_GPIO, 1u << LED_PIN);
    /* Enable PWM outputs */
    PWM_EN(FLEXPWM0);
	ADC0->CTRL |= ADC_CTRL_RSTFIFO_MASK;	/* Reset FIFO */
	ADC1->CTRL |= ADC_CTRL_RSTFIFO_MASK;	/* Reset FIFO */
	FLEXPWM0->MCTRL|= PWM_MCTRL_RUN(0xF);	/* Run PWM */
    /* Global ISR enable */
    EnableGlobalIRQ(ui32PrimaskReg);

    while (1)
    {
    	FMSTR_Poll();
        //LPUART_ReadBlocking(DEMO_LPUART, &ch, 1);
        //LPUART_WriteBlocking(DEMO_LPUART, &ch, 1);
    }
}

/* DC bus HV protection interrupt */
void GPIO1_IRQHandler(void)
{
	GPIO_GpioClearInterruptFlags(DC_BUS_HW_PROT_GPIO, 1U << DC_BUS_HW_PROT_PIN);
}


void ADC0_IRQHandler()
{
	ui32AdcIsrCnt++;

	/* Get number of results in FIFO */
	ui32Fcount0 = ADC0->FCTRL & ADC_FCTRL_FCOUNT_MASK;
	/* Read and store ADC0 */
	sAdcRaw.ui32Ia = ADC0->RESFIFO;
	sAdcRes.i16Ia = (int16_t)(sAdcRaw.ui32Ia & ADC_RESFIFO_D_MASK);

	/* Read and store ADC0 */
	sAdcRaw.ui32Vbus1 = ADC0->RESFIFO;
	sAdcRes.i16Vbus1 = (int16_t)(sAdcRaw.ui32Vbus1 & ADC_RESFIFO_D_MASK)<<1;
	/* Example of scaling to float */
	fVdcBus = MLIB_ConvSc_FLTsf(sAdcRes.i16Vbus1, fltDcBusVoltageScale);

	/* Read and store ADC0 */
	sAdcRaw.ui32Vbus2 = ADC0->RESFIFO;
	sAdcRes.i16Vbus2 = (int16_t)(sAdcRaw.ui32Vbus2 & ADC_RESFIFO_D_MASK);

	/* Read and store ADC0 */
	sAdcRaw.ui32IpmTemp = ADC0->RESFIFO;
	sAdcRes.i16IpmTemp = (int16_t)(sAdcRaw.ui32IpmTemp & ADC_RESFIFO_D_MASK);

	/* Read and store ADC0 */
	sAdcRaw.ui32BemfA = ADC0->RESFIFO;
	sAdcRes.i16BemfA = (int16_t)(sAdcRaw.ui32BemfA & ADC_RESFIFO_D_MASK);

	/* Get number of results in FIFO */
	ui32Fcount1 = ADC1->FCTRL & ADC_FCTRL_FCOUNT_MASK;

	/* Read and store ADC1 */
	sAdcRaw.ui32Ib = ADC1->RESFIFO;
	sAdcRes.i16Ib = (int16_t)(sAdcRaw.ui32Ib & ADC_RESFIFO_D_MASK);

	/* Read and store ADC1 */
	sAdcRaw.ui32BemfB = ADC1->RESFIFO;
	sAdcRes.i16BemfB = (int16_t)(sAdcRaw.ui32BemfB & ADC_RESFIFO_D_MASK);

	/* Read and store ADC1 */
	sAdcRaw.ui32BemfC = ADC1->RESFIFO;
	sAdcRes.i16BemfC = (int16_t)(sAdcRaw.ui32BemfC & ADC_RESFIFO_D_MASK);

	/* Read and store ADC1 */
	sAdcRaw.ui32GpAnalog = ADC1->RESFIFO;
	sAdcRes.i16GpAnalog = (int16_t)(sAdcRaw.ui32GpAnalog & ADC_RESFIFO_D_MASK);

	FMSTR_Recorder(0);
	/* Set PWM duty */
	UpdatePWM(i16DutyA, i16DutyB, i16DutyB, FLEXPWM0);

	/* Get fault status */
	ui16PwmFault = (FLEXPWM0->FSTS & PWM_FSTS_FFLAG_MASK);
}


static void InitFlexPWM()
{
    PWM_Type *PWMBase = (PWM_Type *)FLEXPWM0_BASE;

    CLOCK_EnableClock(kCLOCK_GateFLEXPWM0);
    // Enable eFlexPWM1 AHB clock
    CLOCK_EnableClock(kCLOCK_GatePWM0SM0);
    CLOCK_EnableClock(kCLOCK_GatePWM0SM1);
    CLOCK_EnableClock(kCLOCK_GatePWM0SM2);
    CLOCK_EnableClock(kCLOCK_GatePWM0SM3);

    RESET_ReleasePeripheralReset(kFLEXPWM0_RST_SHIFT_RSTn);

    /* Full and Half cycle reload */
    PWMBase->SM[0].CTRL |= PWM_CTRL_FULL_MASK;
    PWMBase->SM[1].CTRL |= PWM_CTRL_FULL_MASK;
    PWMBase->SM[2].CTRL |= PWM_CTRL_FULL_MASK;

    /* Value register initial values, duty cycle 50% */
    PWMBase->SM[0].INIT = PWM_INIT_INIT((uint16_t)(-(PWM_MODULO / 2)));
    PWMBase->SM[1].INIT = PWM_INIT_INIT((uint16_t)(-(PWM_MODULO / 2)));
    PWMBase->SM[2].INIT = PWM_INIT_INIT((uint16_t)(-(PWM_MODULO / 2)));

    PWMBase->SM[0].VAL0 = PWM_VAL0_VAL0((uint16_t)(0));
    PWMBase->SM[1].VAL0 = PWM_VAL0_VAL0((uint16_t)(0));
    PWMBase->SM[2].VAL0 = PWM_VAL0_VAL0((uint16_t)(0));

    PWMBase->SM[0].VAL1 = PWM_VAL1_VAL1((uint16_t)((PWM_MODULO / 2) - 1));
    PWMBase->SM[1].VAL1 = PWM_VAL1_VAL1((uint16_t)((PWM_MODULO / 2) - 1));
    PWMBase->SM[2].VAL1 = PWM_VAL1_VAL1((uint16_t)((PWM_MODULO / 2) - 1));

    PWMBase->SM[0].VAL2 = PWM_VAL2_VAL2((uint16_t)(-(PWM_MODULO / 4)));
    PWMBase->SM[1].VAL2 = PWM_VAL2_VAL2((uint16_t)(-(PWM_MODULO / 4)));
    PWMBase->SM[2].VAL2 = PWM_VAL2_VAL2((uint16_t)(-(PWM_MODULO / 4)));

    PWMBase->SM[0].VAL3 = PWM_VAL3_VAL3((uint16_t)(PWM_MODULO / 4));
    PWMBase->SM[1].VAL3 = PWM_VAL3_VAL3((uint16_t)(PWM_MODULO / 4));
    PWMBase->SM[2].VAL3 = PWM_VAL3_VAL3((uint16_t)(PWM_MODULO / 4));

    /* Trigger for ADC synchronization */
    PWMBase->SM[0].VAL4 = PWM_VAL4_VAL4((uint16_t)(-PWM_MODULO / 2));
    PWMBase->SM[1].VAL4 = PWM_VAL4_VAL4((uint16_t)(-PWM_MODULO / 2));
    PWMBase->SM[2].VAL4 = PWM_VAL4_VAL4((uint16_t)(0));

    PWMBase->SM[0].VAL5 = PWM_VAL5_VAL5((uint16_t)(0));
    PWMBase->SM[1].VAL5 = PWM_VAL5_VAL5((uint16_t)(0));
    PWMBase->SM[2].VAL5 = PWM_VAL5_VAL5((uint16_t)(0));

    /* PWM sub-module 0 trigger on VAL4 enabled for ADC synchronization */
    PWMBase->SM[0].TCTRL |= PWM_TCTRL_OUT_TRIG_EN(1 << 4) | PWM_TCTRL_TRGFRQ(1);
    PWMBase->SM[1].TCTRL |= PWM_TCTRL_OUT_TRIG_EN(1 << 4) | PWM_TCTRL_TRGFRQ(1);
        /* Set dead-time register */
    PWMBase->SM[0].DTCNT0 = PWM_DTCNT0_DTCNT0(PWM_DEADTIME);
    PWMBase->SM[1].DTCNT0 = PWM_DTCNT0_DTCNT0(PWM_DEADTIME);
    PWMBase->SM[2].DTCNT0 = PWM_DTCNT0_DTCNT0(PWM_DEADTIME);
    PWMBase->SM[0].DTCNT1 = PWM_DTCNT1_DTCNT1(PWM_DEADTIME);
    PWMBase->SM[1].DTCNT1 = PWM_DTCNT1_DTCNT1(PWM_DEADTIME);
    PWMBase->SM[2].DTCNT1 = PWM_DTCNT1_DTCNT1(PWM_DEADTIME);

    /* Channels A and B disabled when fault 0 occurs */
    PWMBase->SM[0].DISMAP[0] = ((PWMBase->SM[0].DISMAP[0] & ~PWM_DISMAP_DIS0A_MASK) | PWM_DISMAP_DIS0A(0x1));
    PWMBase->SM[1].DISMAP[0] = ((PWMBase->SM[0].DISMAP[0] & ~PWM_DISMAP_DIS0A_MASK) | PWM_DISMAP_DIS0A(0x1));
    PWMBase->SM[2].DISMAP[0] = ((PWMBase->SM[0].DISMAP[0] & ~PWM_DISMAP_DIS0A_MASK) | PWM_DISMAP_DIS0A(0x1));
    PWMBase->SM[0].DISMAP[0] = ((PWMBase->SM[0].DISMAP[0] & ~PWM_DISMAP_DIS0B_MASK) | PWM_DISMAP_DIS0B(0x1));
    PWMBase->SM[1].DISMAP[0] = ((PWMBase->SM[0].DISMAP[0] & ~PWM_DISMAP_DIS0B_MASK) | PWM_DISMAP_DIS0B(0x1));
    PWMBase->SM[2].DISMAP[0] = ((PWMBase->SM[0].DISMAP[0] & ~PWM_DISMAP_DIS0B_MASK) | PWM_DISMAP_DIS0B(0x1));

    /* Modules one and two gets clock from module zero */
    PWMBase->SM[1].CTRL2 = (PWMBase->SM[1].CTRL2 & ~PWM_CTRL2_CLK_SEL_MASK) | PWM_CTRL2_CLK_SEL(0x2);
    PWMBase->SM[2].CTRL2 = (PWMBase->SM[2].CTRL2 & ~PWM_CTRL2_CLK_SEL_MASK) | PWM_CTRL2_CLK_SEL(0x2);

    /* Master reload active for modules one and two */
    PWMBase->SM[1].CTRL2 |= PWM_CTRL2_RELOAD_SEL_MASK;
    PWMBase->SM[2].CTRL2 |= PWM_CTRL2_RELOAD_SEL_MASK;

    /* Master reload is generated every one opportunity */
    PWMBase->SM[0].CTRL = (PWMBase->SM[0].CTRL & ~PWM_CTRL_LDFQ_MASK) | PWM_CTRL_LDFQ(0);

    /* Master sync active for modules one and two*/
    PWMBase->SM[1].CTRL2 = (PWMBase->SM[1].CTRL2 & ~PWM_CTRL2_INIT_SEL_MASK) | PWM_CTRL2_INIT_SEL(0x2);
    PWMBase->SM[2].CTRL2 = (PWMBase->SM[2].CTRL2 & ~PWM_CTRL2_INIT_SEL_MASK) | PWM_CTRL2_INIT_SEL(0x2);

    /* Fault 0 active in logic level one, automatic clearing */
    PWMBase->FCTRL = (PWMBase->FCTRL & ~PWM_FCTRL_FLVL_MASK) | PWM_FCTRL_FLVL(0x1);
    PWMBase->FCTRL = (PWMBase->FCTRL & ~PWM_FCTRL_FAUTO_MASK) | PWM_FCTRL_FAUTO(0x1);

    /* Clear fault flags */
    PWMBase->FSTS = (PWMBase->FSTS & ~PWM_FSTS_FFLAG_MASK) | PWM_FSTS_FFLAG(0xF);

    /* PWMs are re-enabled at PWM full cycle */
    PWMBase->FSTS = (PWMBase->FSTS & ~PWM_FSTS_FFULL_MASK) | PWM_FSTS_FFULL(0x1);

    /* PWM fault filter - 5 Fast peripheral clocks sample rate, 5 agreeing
       samples to activate */
    PWMBase->FFILT = (PWMBase->FFILT & ~PWM_FFILT_FILT_PER_MASK) | PWM_FFILT_FILT_PER(5);
    PWMBase->FFILT = (PWMBase->FFILT & ~PWM_FFILT_FILT_CNT_MASK) | PWM_FFILT_FILT_CNT(5);

    /* Start PWMs (set load OK flags and run) */
    PWMBase->MCTRL = (PWMBase->MCTRL & ~PWM_MCTRL_CLDOK_MASK) | PWM_MCTRL_CLDOK(0xF);
    PWMBase->MCTRL = (PWMBase->MCTRL & ~PWM_MCTRL_LDOK_MASK) | PWM_MCTRL_LDOK(0xF);
    PWMBase->MCTRL = (PWMBase->MCTRL & ~PWM_MCTRL_RUN_MASK) | PWM_MCTRL_RUN(0x0);
}

static void InitADC0()
{
	RESET_ReleasePeripheralReset(kADC0_RST_SHIFT_RSTn);

    /* Attach peripheral clock */
    CLOCK_SetClockDiv(kCLOCK_DivADC, 1u);
    CLOCK_AttachClk(kFRO_LF_DIV_to_ADC);

    lpadc_conv_trigger_config_t lpadcTriggerConfig;
    lpadc_conv_command_config_t lpadcCommandConfig;
    lpadc_config_t lpadcConfig;

    /* Init the lpadcConfig struct */
    LPADC_GetDefaultConfig(&lpadcConfig);
    lpadcConfig.enableAnalogPreliminary = true;
    lpadcConfig.referenceVoltageSource = kLPADC_ReferenceVoltageAlt3;
    lpadcConfig.conversionAverageMode = kLPADC_ConversionAverage1;

    LPADC_Init(ADC0, &lpadcConfig);

    LPADC_DoOffsetCalibration(ADC0);
    LPADC_DoAutoCalibration(ADC0);

    LPADC_GetDefaultConvCommandConfig(&lpadcCommandConfig);
    lpadcCommandConfig.sampleChannelMode = kLPADC_SampleChannelSingleEndSideA;
    lpadcCommandConfig.conversionResolutionMode = kLPADC_ConversionResolutionStandard;
    lpadcCommandConfig.sampleTimeMode = kLPADC_SampleTimeADCK3;

    /* Init ADC channels. */
    lpadcCommandConfig.channelNumber = ADC0CH_CURR_A;		// or ADC0CH_CURR_B in channel mapping function according to FOC sector
    lpadcCommandConfig.chainedNextCommandNumber = 2U;
    LPADC_SetConvCommandConfig( ADC0, 1U, &lpadcCommandConfig );

    lpadcCommandConfig.channelNumber = ADC0CH_VBUS_1;
    lpadcCommandConfig.chainedNextCommandNumber = 3U;
    LPADC_SetConvCommandConfig( ADC0, 2U, &lpadcCommandConfig );

    lpadcCommandConfig.channelNumber = ADC0CH_VBUS_2;
    lpadcCommandConfig.chainedNextCommandNumber = 4U;
    LPADC_SetConvCommandConfig( ADC0, 3U, &lpadcCommandConfig );

    lpadcCommandConfig.channelNumber = ADC0CH_IPMTEMP;
    lpadcCommandConfig.chainedNextCommandNumber = 5U;
    LPADC_SetConvCommandConfig( ADC0, 4U, &lpadcCommandConfig );

    lpadcCommandConfig.channelNumber = ADC0CH_BEMFA;
    lpadcCommandConfig.chainedNextCommandNumber = 0U;
    LPADC_SetConvCommandConfig( ADC0, 5U, &lpadcCommandConfig );

    /* Init triggers (use trigger 0). */
    LPADC_GetDefaultConvTriggerConfig(&lpadcTriggerConfig);
    lpadcTriggerConfig.targetCommandId = 1U;
    lpadcTriggerConfig.enableHardwareTrigger = true;
    LPADC_SetConvTriggerConfig(ADC0, 0U, &lpadcTriggerConfig);

	/* Set watermark for FIFO0 */
	ADC0->FCTRL |= ADC_FCTRL_FWMARK(4);
	/* Enable FIFO0 watermark interrupt */
	ADC0->IE |= ADC_IE_FWMIE0(1);

    NVIC_SetPriority(ADC0_IRQn, 0U);
    NVIC_EnableIRQ(ADC0_IRQn);
}

static void InitADC1()
{
	RESET_ReleasePeripheralReset(kADC1_RST_SHIFT_RSTn);

    lpadc_conv_trigger_config_t lpadcTriggerConfig;
    lpadc_conv_command_config_t lpadcCommandConfig;
    lpadc_config_t lpadcConfig;

    /* Init the lpadcConfig struct */
    LPADC_GetDefaultConfig(&lpadcConfig);
    lpadcConfig.enableAnalogPreliminary = true;
    lpadcConfig.referenceVoltageSource = kLPADC_ReferenceVoltageAlt3;
    lpadcConfig.conversionAverageMode = kLPADC_ConversionAverage1;

    LPADC_Init(ADC1, &lpadcConfig);

    LPADC_DoOffsetCalibration(ADC1);
    LPADC_DoAutoCalibration(ADC1);

    LPADC_GetDefaultConvCommandConfig(&lpadcCommandConfig);
    lpadcCommandConfig.sampleChannelMode = kLPADC_SampleChannelSingleEndSideA;
    lpadcCommandConfig.conversionResolutionMode = kLPADC_ConversionResolutionStandard;
    lpadcCommandConfig.sampleTimeMode = kLPADC_SampleTimeADCK5;

    /* Init ADC channels */
    lpadcCommandConfig.channelNumber = ADC1CH_CURR_B;// or ADC1CH_CURR_C in channel mapping function according to FOC sector
    lpadcCommandConfig.chainedNextCommandNumber = 2;
    LPADC_SetConvCommandConfig( ADC1, 1U, &lpadcCommandConfig );

    lpadcCommandConfig.channelNumber = ADC1CH_BEMFB;
    lpadcCommandConfig.chainedNextCommandNumber = 3U;
    LPADC_SetConvCommandConfig( ADC1, 2U, &lpadcCommandConfig );

    lpadcCommandConfig.channelNumber = ADC1CH_BEMFC;
    lpadcCommandConfig.chainedNextCommandNumber = 4U;
    LPADC_SetConvCommandConfig( ADC1, 3U, &lpadcCommandConfig );

    lpadcCommandConfig.channelNumber = ADC1CH_GP_ANALOG;
    lpadcCommandConfig.chainedNextCommandNumber = 0U;
    LPADC_SetConvCommandConfig( ADC1, 4U, &lpadcCommandConfig );


    /* Init triggers (use trigger 0). */
    LPADC_GetDefaultConvTriggerConfig(&lpadcTriggerConfig);
    lpadcTriggerConfig.targetCommandId = 1U;
    lpadcTriggerConfig.enableHardwareTrigger = true;
    LPADC_SetConvTriggerConfig(ADC1, 0U, &lpadcTriggerConfig);

}

static void InitUART()
{
	CLOCK_SetClockDiv(kCLOCK_DivLPUART2, 1u);
	CLOCK_AttachClk(kFRO_LF_DIV_to_LPUART2);
	RESET_PeripheralReset(kLPUART2_RST_SHIFT_RSTn);

    lpuart_config_t config;
    /*
     * config.baudRate_Bps = 115200U;
     * config.parityMode = kLPUART_ParityDisabled;
     * config.stopBitCount = kLPUART_OneStopBit;
     * config.txFifoWatermark = 0;
     * config.rxFifoWatermark = 0;
     * config.enableTx = false;
     * config.enableRx = false;
     */
    LPUART_GetDefaultConfig(&config);
    config.baudRate_Bps = MAINBOARD_COMM_UART_BAUDRATE;
    config.enableTx     = true;
    config.enableRx     = true;

    LPUART_Init(MAINBOARD_COMM, &config, MAINBOARD_COMM_LPUART_CLK_FREQ);

    FMSTR_SerialSetBaseAddress((LPUART_Type*)LPUART2);
    //LPUART_WriteBlocking(DEMO_LPUART, txbuff, sizeof(txbuff) - 1);
}

static void InitSPI()
{
	lpspi_master_config_t masterConfig;

	/* Attach peripheral clock */
	CLOCK_SetClockDiv(kCLOCK_DivLPSPI0, 1u);
	CLOCK_AttachClk(kFRO_LF_DIV_to_LPSPI0);

	/* Master config */
	LPSPI_MasterGetDefaultConfig(&masterConfig);
	masterConfig.baudRate = SPI_TRANSFER_BAUDRATE;
	masterConfig.whichPcs = kLPSPI_Pcs0;
	masterConfig.pcsToSckDelayInNanoSec        = 1000000000U / (masterConfig.baudRate * 2U);
	masterConfig.lastSckToPcsDelayInNanoSec    = 1000000000U / (masterConfig.baudRate * 2U);
	masterConfig.betweenTransferDelayInNanoSec = 1000000000U / (masterConfig.baudRate * 2U);

	LPSPI_MasterInit(LPSPI0, &masterConfig, CLOCK_GetLpspiClkFreq(0u));

}

static void InitInputmux(void)
{
    CLOCK_EnableClock(kCLOCK_InputMux);
    /* Release peripheral reset */
    RESET_ReleasePeripheralReset(kINPUTMUX0_RST_SHIFT_RSTn);
    /* PWM0_SM0_OUT_TRIG0 is selected as trigger input for ADC0 */
    INPUTMUX0->ADC0_TRIG[0] = INPUTMUX_ADC0_TRIGM_ADC0_TRIG_TRIGIN(0b010010);
    /* PWM0_SM0_OUT_TRIG0 is selected as trigger input for ADC1 */
    INPUTMUX0->ADC1_TRIG[0] = INPUTMUX_ADC1_TRIGM_ADC1_TRIG_TRIGIN(0b010010);

    /* TRIGGER PIN is selected as fault input for PWM */
    INPUTMUX_AttachSignal(INPUTMUX0, 0U, kINPUTMUX_TrigIn10ToFlexPwm0Fault);

}

static void InitPins(void)
{
    /* PORT2: Peripheral clock is enabled */
    CLOCK_EnableClock(kCLOCK_GatePORT0);
    /* PORT2: Peripheral clock is enabled */
    CLOCK_EnableClock(kCLOCK_GatePORT1);
    /* PORT2: Peripheral clock is enabled */
    CLOCK_EnableClock(kCLOCK_GatePORT2);
    /* PORT2: Peripheral clock is enabled */
    CLOCK_EnableClock(kCLOCK_GatePORT3);

    /* GPIO: Peripheral clock is enabled */
    CLOCK_EnableClock(kCLOCK_GateGPIO0);
    CLOCK_EnableClock(kCLOCK_GateGPIO1);
    CLOCK_EnableClock(kCLOCK_GateGPIO2);
    CLOCK_EnableClock(kCLOCK_GateGPIO3);

    /* PORT1 peripheral is released from reset */
    RESET_ReleasePeripheralReset(kPORT0_RST_SHIFT_RSTn);
    /* PORT1 peripheral is released from reset */
    RESET_ReleasePeripheralReset(kPORT1_RST_SHIFT_RSTn);
    /* PORT1 peripheral is released from reset */
    RESET_ReleasePeripheralReset(kPORT2_RST_SHIFT_RSTn);
    /* PORT1 peripheral is released from reset */
    RESET_ReleasePeripheralReset(kPORT3_RST_SHIFT_RSTn);

    /* PORT1 peripheral is released from reset */
    RESET_ReleasePeripheralReset(kGPIO0_RST_SHIFT_RSTn);
    /* PORT1 peripheral is released from reset */
    RESET_ReleasePeripheralReset(kGPIO1_RST_SHIFT_RSTn);
    /* PORT1 peripheral is released from reset */
    RESET_ReleasePeripheralReset(kGPIO2_RST_SHIFT_RSTn);
    /* PORT1 peripheral is released from reset */
    RESET_ReleasePeripheralReset(kGPIO3_RST_SHIFT_RSTn);



    /* LPUART2 peripheral is released from reset */
    RESET_ReleasePeripheralReset(kLPUART2_RST_SHIFT_RSTn);

    const port_pin_config_t port2_2_pin35_config = {/* Internal pull-up resistor is enabled */
                                                    .pullSelect = kPORT_PullUp,
                                                    /* Low internal pull resistor value is selected. */
                                                    .pullValueSelect = kPORT_LowPullResistor,
                                                    /* Fast slew rate is configured */
                                                    .slewRate = kPORT_FastSlewRate,
                                                    /* Passive input filter is disabled */
                                                    .passiveFilterEnable = kPORT_PassiveFilterDisable,
                                                    /* Open drain output is disabled */
                                                    .openDrainEnable = kPORT_OpenDrainDisable,
                                                    /* Low drive strength is configured */
                                                    .driveStrength = kPORT_LowDriveStrength,
                                                    /* Normal drive strength is configured */
                                                    .driveStrength1 = kPORT_NormalDriveStrength,
                                                    /* Pin is configured as LPUART2_TXD */
                                                    .mux = kPORT_MuxAlt3,
                                                    /* Digital input enabled */
                                                    .inputBuffer = kPORT_InputBufferEnable,
                                                    /* Digital input is not inverted */
                                                    .invertInput = kPORT_InputNormal,
                                                    /* Pin Control Register fields [15:0] are not locked */
                                                    .lockRegister = kPORT_UnlockRegister};
    /* PORT2_2 (pin 35) is configured as LPUART2_TXD */
    PORT_SetPinConfig(PORT2, 2U, &port2_2_pin35_config);

    const port_pin_config_t port2_3_pin36_config = {/* Internal pull-up resistor is enabled */
                                                    .pullSelect = kPORT_PullUp,
                                                    /* Low internal pull resistor value is selected. */
                                                    .pullValueSelect = kPORT_LowPullResistor,
                                                    /* Fast slew rate is configured */
                                                    .slewRate = kPORT_FastSlewRate,
                                                    /* Passive input filter is disabled */
                                                    .passiveFilterEnable = kPORT_PassiveFilterDisable,
                                                    /* Open drain output is disabled */
                                                    .openDrainEnable = kPORT_OpenDrainDisable,
                                                    /* Low drive strength is configured */
                                                    .driveStrength = kPORT_LowDriveStrength,
                                                    /* Normal drive strength is configured */
                                                    .driveStrength1 = kPORT_NormalDriveStrength,
                                                    /* Pin is configured as LPUART2_RXD */
                                                    .mux = kPORT_MuxAlt3,
                                                    /* Digital input enabled */
                                                    .inputBuffer = kPORT_InputBufferEnable,
                                                    /* Digital input is not inverted */
                                                    .invertInput = kPORT_InputNormal,
                                                    /* Pin Control Register fields [15:0] are not locked */
                                                    .lockRegister = kPORT_UnlockRegister};
    /* PORT2_3 (pin 36) is configured as LPUART2_RXD */
    PORT_SetPinConfig(PORT2, 3U, &port2_3_pin36_config);


    /* LPSPI0 peripheral is released from reset */
    RESET_ReleasePeripheralReset(kLPSPI0_RST_SHIFT_RSTn);
    /* LPI2C1 peripheral is released from reset */
    RESET_ReleasePeripheralReset(kLPI2C1_RST_SHIFT_RSTn);
    /* FLEXCAN0 peripheral is released from reset */
    RESET_ReleasePeripheralReset(kFLEXCAN0_RST_SHIFT_RSTn);
    /* LPSPI1 peripheral is released from reset */
    RESET_ReleasePeripheralReset(kLPSPI1_RST_SHIFT_RSTn);
    /* PORT3 peripheral is released from reset */
    RESET_ReleasePeripheralReset(kPORT3_RST_SHIFT_RSTn);

    const port_pin_config_t port1_0_pin135_config = {/* Internal pull-up/down resistor is disabled */
                                                     .pullSelect = kPORT_PullDisable,
                                                     /* Low internal pull resistor value is selected. */
                                                     .pullValueSelect = kPORT_LowPullResistor,
                                                     /* Fast slew rate is configured */
                                                     .slewRate = kPORT_FastSlewRate,
                                                     /* Passive input filter is disabled */
                                                     .passiveFilterEnable = kPORT_PassiveFilterDisable,
                                                     /* Open drain output is disabled */
                                                     .openDrainEnable = kPORT_OpenDrainDisable,
                                                     /* Low drive strength is configured */
                                                     .driveStrength = kPORT_LowDriveStrength,
                                                     /* Normal drive strength is configured */
                                                     .driveStrength1 = kPORT_NormalDriveStrength,
                                                     /* Pin is configured as LPSPI0_SDO */
                                                     .mux = kPORT_MuxAlt2,
                                                     /* Digital input enabled */
                                                     .inputBuffer = kPORT_InputBufferEnable,
                                                     /* Digital input is not inverted */
                                                     .invertInput = kPORT_InputNormal,
                                                     /* Pin Control Register fields [15:0] are not locked */
                                                     .lockRegister = kPORT_UnlockRegister};
    /* PORT1_0 (pin 135) is configured as LPSPI0_SDO */
    PORT_SetPinConfig(PORT1, 0U, &port1_0_pin135_config);

    const port_pin_config_t port1_1_pin136_config = {/* Internal pull-up/down resistor is disabled */
                                                     .pullSelect = kPORT_PullDisable,
                                                     /* Low internal pull resistor value is selected. */
                                                     .pullValueSelect = kPORT_LowPullResistor,
                                                     /* Fast slew rate is configured */
                                                     .slewRate = kPORT_FastSlewRate,
                                                     /* Passive input filter is disabled */
                                                     .passiveFilterEnable = kPORT_PassiveFilterDisable,
                                                     /* Open drain output is disabled */
                                                     .openDrainEnable = kPORT_OpenDrainDisable,
                                                     /* Low drive strength is configured */
                                                     .driveStrength = kPORT_LowDriveStrength,
                                                     /* Normal drive strength is configured */
                                                     .driveStrength1 = kPORT_NormalDriveStrength,
                                                     /* Pin is configured as LPSPI0_SCK */
                                                     .mux = kPORT_MuxAlt2,
                                                     /* Digital input enabled */
                                                     .inputBuffer = kPORT_InputBufferEnable,
                                                     /* Digital input is not inverted */
                                                     .invertInput = kPORT_InputNormal,
                                                     /* Pin Control Register fields [15:0] are not locked */
                                                     .lockRegister = kPORT_UnlockRegister};
    /* PORT1_1 (pin 136) is configured as LPSPI0_SCK */
    PORT_SetPinConfig(PORT1, 1U, &port1_1_pin136_config);

    const port_pin_config_t port1_2_pin137_config = {/* Internal pull-up/down resistor is disabled */
                                                     .pullSelect = kPORT_PullDisable,
                                                     /* Low internal pull resistor value is selected. */
                                                     .pullValueSelect = kPORT_LowPullResistor,
                                                     /* Fast slew rate is configured */
                                                     .slewRate = kPORT_FastSlewRate,
                                                     /* Passive input filter is disabled */
                                                     .passiveFilterEnable = kPORT_PassiveFilterDisable,
                                                     /* Open drain output is disabled */
                                                     .openDrainEnable = kPORT_OpenDrainDisable,
                                                     /* Low drive strength is configured */
                                                     .driveStrength = kPORT_LowDriveStrength,
                                                     /* Normal drive strength is configured */
                                                     .driveStrength1 = kPORT_NormalDriveStrength,
                                                     /* Pin is configured as LPSPI0_SDI */
                                                     .mux = kPORT_MuxAlt2,
                                                     /* Digital input enabled */
                                                     .inputBuffer = kPORT_InputBufferEnable,
                                                     /* Digital input is not inverted */
                                                     .invertInput = kPORT_InputNormal,
                                                     /* Pin Control Register fields [15:0] are not locked */
                                                     .lockRegister = kPORT_UnlockRegister};
    /* PORT1_2 (pin 137) is configured as LPSPI0_SDI */
    PORT_SetPinConfig(PORT1, 2U, &port1_2_pin137_config);

    const port_pin_config_t port1_3_pin138_config = {/* Internal pull-up/down resistor is disabled */
                                                     .pullSelect = kPORT_PullDisable,
                                                     /* Low internal pull resistor value is selected. */
                                                     .pullValueSelect = kPORT_LowPullResistor,
                                                     /* Fast slew rate is configured */
                                                     .slewRate = kPORT_FastSlewRate,
                                                     /* Passive input filter is disabled */
                                                     .passiveFilterEnable = kPORT_PassiveFilterDisable,
                                                     /* Open drain output is disabled */
                                                     .openDrainEnable = kPORT_OpenDrainDisable,
                                                     /* Low drive strength is configured */
                                                     .driveStrength = kPORT_LowDriveStrength,
                                                     /* Normal drive strength is configured */
                                                     .driveStrength1 = kPORT_NormalDriveStrength,
                                                     /* Pin is configured as LPSPI0_PCS0 */
                                                     .mux = kPORT_MuxAlt2,
                                                     /* Digital input enabled */
                                                     .inputBuffer = kPORT_InputBufferEnable,
                                                     /* Digital input is not inverted */
                                                     .invertInput = kPORT_InputNormal,
                                                     /* Pin Control Register fields [15:0] are not locked */
                                                     .lockRegister = kPORT_UnlockRegister};
    /* PORT1_3 (pin 138) is configured as LPSPI0_PCS0 */
    PORT_SetPinConfig(PORT1, 3U, &port1_3_pin138_config);

    const port_pin_config_t port_analog_config = {/* Internal pull-up/down resistor is disabled */
                                                        kPORT_PullDisable,
                                                        /* Low internal pull resistor value is selected. */
                                                        kPORT_LowPullResistor,
                                                        /* Fast slew rate is configured */
                                                        kPORT_FastSlewRate,
                                                        /* Passive input filter is disabled */
                                                        kPORT_PassiveFilterDisable,
                                                        /* Open drain output is disabled */
                                                        kPORT_OpenDrainDisable,
                                                        /* Low drive strength is configured */
                                                        kPORT_LowDriveStrength,
                                                        /* Normal drive strength is configured */
                                                        kPORT_NormalDriveStrength,
                                                        /* Pin is configured as ADC1_A8 */
                                                        kPORT_MuxAlt0,
                                                        /* Digital input disabled; it is required for analog functions */
                                                        kPORT_InputBufferDisable,
                                                        /* Digital input is not inverted */
                                                        kPORT_InputNormal,
                                                        /* Pin Control Register fields [15:0] are not locked */
                                                        kPORT_UnlockRegister};
    /* PORT2_7 is configured as ADC0_A7 */
    PORT_SetPinConfig(PORT2, 7U, &port_analog_config);
    /* PORT1_10 is configured as ADC1_A8 */
    PORT_SetPinConfig(PORT1, 10U, &port_analog_config);
    /* PORT2_4 is configured as ADC0_A1 */
    PORT_SetPinConfig(PORT2, 4U, &port_analog_config);
    /* PORT3_30 is configured as ADC1_A21 */
    PORT_SetPinConfig(PORT3, 30U, &port_analog_config);
    /* PORT1_4 is configured as ADC0_A20 */
    PORT_SetPinConfig(PORT1, 4U, &port_analog_config);
    /* PORT1_3 is configured as ADC0_A19 */
    PORT_SetPinConfig(PORT1, 3U, &port_analog_config);
    /* PORT1_5 is configured as ADC0_A21 */
    PORT_SetPinConfig(PORT1, 5U, &port_analog_config);
    /* PORT1_6 is configured as ADC0_A22 */
    PORT_SetPinConfig(PORT1, 6U, &port_analog_config);
    /* PORT1_14 is configured as ADC1_A12 */
    PORT_SetPinConfig(PORT1, 14U, &port_analog_config);
    /* PORT2_5 is configured as ADC1_A1 */
    PORT_SetPinConfig(PORT2, 5U, &port_analog_config);
    /* PORT1_14 is configured as ADC1_A12 */
    PORT_SetPinConfig(PORT1, 14U, &port_analog_config);


    const port_pin_config_t port_pwm_config = {/* Internal pull-up/down resistor is disabled */
    		kPORT_PullDisable,
			/* Low internal pull resistor value is selected. */
			kPORT_LowPullResistor,
			/* Fast slew rate is configured */
			kPORT_FastSlewRate,
			/* Passive input filter is disabled */
			kPORT_PassiveFilterDisable,
			/* Open drain output is disabled */
			kPORT_OpenDrainDisable,
			/* Low drive strength is configured */
			kPORT_LowDriveStrength,
			/* Normal drive strength is configured */
			kPORT_NormalDriveStrength,
			/* Pin is configured as PWM0 */
			kPORT_MuxAlt5,
			/* Digital input enabled */
			kPORT_InputBufferEnable,
			/* Digital input is not inverted */
			kPORT_InputNormal,
			/* Pin Control Register fields [15:0] are not locked */
			kPORT_UnlockRegister};
    /* PWMA0*/
    PORT_SetPinConfig(PORT3, 0U, &port_pwm_config);
    /* PWMB0*/
    PORT_SetPinConfig(PORT3, 1U, &port_pwm_config);
    /* PWMA1*/
    PORT_SetPinConfig(PORT3, 8U, &port_pwm_config);
    /* PWMB1*/
    PORT_SetPinConfig(PORT3, 9U, &port_pwm_config);
    /* PWMA2*/
    PORT_SetPinConfig(PORT3, 10U, &port_pwm_config);
    /* PWMB2*/
    PORT_SetPinConfig(PORT3, 11U, &port_pwm_config);

    const port_pin_config_t port3_31_pin48_config = {/* Internal pull-up/down resistor is disabled */
    		kPORT_PullDisable,
			/* Low internal pull resistor value is selected. */
			kPORT_LowPullResistor,
			/* Fast slew rate is configured */
			kPORT_FastSlewRate,
			/* Passive input filter is disabled */
			kPORT_PassiveFilterDisable,
			/* Open drain output is disabled */
			kPORT_OpenDrainDisable,
			/* Low drive strength is configured */
			kPORT_LowDriveStrength,
			/* Normal drive strength is configured */
			kPORT_NormalDriveStrength,
			/* Pin is configured as TRIG_IN10 */
			kPORT_MuxAlt1,
			/* Digital input enabled */
			kPORT_InputBufferEnable,
			/* Digital input is not inverted */
			kPORT_InputNormal,
			/* Pin Control Register fields [15:0] are not locked */
			kPORT_UnlockRegister};
    /* PORT3_31 (pin 48) is configured as TRIG_IN10 */
    PORT_SetPinConfig(PORT3, 31U, &port3_31_pin48_config);



    gpio_pin_config_t InrushRelay_config = {
    		.pinDirection = kGPIO_DigitalOutput,
			.outputLogic = 0U
    };
    /* Inrush relay GPIO output */
    GPIO_PinInit(GPIO3, 14, &InrushRelay_config);


    gpio_pin_config_t Led_config = {
    		.pinDirection = kGPIO_DigitalOutput,
			.outputLogic = 0U
    };
    /* Led GPIO output */
    GPIO_PinInit(LED_GPIO, LED_PIN, &Led_config);


    const port_pin_config_t dcbus_prot_config = {/* Internal pull-up/down resistor is disabled */
    		kPORT_PullDisable,
			/* Low internal pull resistor value is selected. */
			kPORT_LowPullResistor,
			/* Fast slew rate is configured */
			kPORT_FastSlewRate,
			/* Passive input filter is disabled */
			kPORT_PassiveFilterDisable,
			/* Open drain output is disabled */
			kPORT_OpenDrainDisable,
			/* Low drive strength is configured */
			kPORT_LowDriveStrength,
			/* Normal drive strength is configured */
			kPORT_NormalDriveStrength,

			kPORT_MuxAlt0,
			/* Digital input enabled */
			kPORT_InputBufferEnable,
			/* Digital input is not inverted */
			kPORT_InputNormal,
			/* Pin Control Register fields [15:0] are not locked */
			kPORT_UnlockRegister};
    /* configured as GPIO IN with interrupt */
    /* Define the init structure for the input switch pin */
    gpio_pin_config_t input_config = {
    		kGPIO_DigitalInput,
			0,
    };

    PORT_SetPinConfig(DC_BUS_HW_PROT_PORT, DC_BUS_HW_PROT_PIN, &dcbus_prot_config);
    GPIO_SetPinInterruptConfig(DC_BUS_HW_PROT_GPIO, DC_BUS_HW_PROT_PIN, kGPIO_InterruptRisingEdge);
    EnableIRQ(GPIO1_IRQn);
    GPIO_PinInit(DC_BUS_HW_PROT_GPIO, DC_BUS_HW_PROT_PIN, &input_config);

}

static void UpdatePWM(int16_t i16PhA, int16_t i16PhB, int16_t i16PhC, PWM_Type *pPWMBase)
{
    /* PWM duty cycles calculation */
    uint16_t ui16Modulo;
    int16_t i16DutyCycle;

    /* Modulo read from PWM val1 register */
    ui16Modulo = pPWMBase->SM[0].VAL1 + 1;

    /* Phase A - duty cycle calculation */

    i16DutyCycle = (int16_t)((int32_t)(ui16Modulo*i16PhA)>>15);
    pPWMBase->SM[0].VAL2 = (uint16_t)(-i16DutyCycle);
    pPWMBase->SM[0].VAL3 = (uint16_t)i16DutyCycle;

    /* Phase B - duty cycle calculation */
    i16DutyCycle = (int16_t)((int32_t)(ui16Modulo*i16PhB)>>15);
    pPWMBase->SM[1].VAL2 = (uint16_t)(-i16DutyCycle);
    pPWMBase->SM[1].VAL3 = (uint16_t)i16DutyCycle;

    /* Phase C - duty cycle calculation */
    i16DutyCycle = (int16_t)((int32_t)(ui16Modulo*i16PhC)>>15);
    pPWMBase->SM[2].VAL2 = (uint16_t)(-i16DutyCycle);
    pPWMBase->SM[2].VAL3 = (uint16_t)i16DutyCycle;

    /* Set LDOK bit for all sub0, sub1, sub2 and sub3 */
    pPWMBase->MCTRL |= PWM_MCTRL_LDOK(15);
}
