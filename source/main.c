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
#include <stdbool.h>
#include "freemaster.h"
#include "freemaster_serial_lpuart.h"
#include "mlib_FP.h"
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

#define MAINBOARD_COMM_UART_BAUDRATE	115200
#define MAINBOARD_COMM         	 		LPUART2
#define MAINBOARD_COMM_LPUART_CLK_FREQ (BOARD_DEBUG_UART_CLK_FREQ)

#define SPI_TRANSFER_SIZE     (512U)    /*! Transfer dataSize .*/
#define SPI_TRANSFER_BAUDRATE (500000U) /*! Transfer baudrate - 500k */

#define PWM_CLOCK_HZ          	 181125000
#define PWM_FREQ_HZ          	 16000
#define PWM_MODULO          	 PWM_CLOCK_HZ/PWM_FREQ_HZ
#define PWM_DEADTIME          	 100


/* ADC CHANNELS DEFINES */
#define ADC0CH_CURR_U			2
#define ADC0CH_VBUS_1			18
#define ADC0CH_VBUS_2			19
#define ADC0CH_3V3_MEAS			15

#define ADC1CH_CURR_V			2
#define ADC1CH_CURR_W			3
#define ADC1CH_IPMTEMP			20

#define ADC2CH_CURR_W			2




#define DCB_VOLT_SCALE			400.0
#define FREEMASTER_REC_0_SIZE	1024
typedef struct
{
	int16_t i16Iu;
	int16_t i16Ivw;
	int16_t i16Iw;
	int16_t i16Vbus1;
	int16_t i16Vbus2;
	int16_t i16IpmTemp;
	int16_t i16V3v3;
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
} sAdcRawResult;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void InitFlexPWM(void);
static void InitADC0(void);
static void InitADC1(void);
static void InitADC2(void);
static void InitUART(void);
static void InitOpamps(void);
static void InitComps(void);
static void InitInputmux();
static void InitDac(void);
static void UpdatePWM(int16_t i16PhA, int16_t i16PhB, int16_t i16PhC, PWM_Type *pPWMBase);

/*******************************************************************************
 * Variables
 ******************************************************************************/

uint8_t txbuff[]   = "Lpuart polling example\r\nBoard will send back received characters\r\n";
uint8_t rxbuff[20] = {0};

volatile uint32_t ui32Fcount0;
volatile uint32_t ui32Fcount1;
volatile uint32_t ui32Fcount2;
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
	BOARD_InitPins();

    InitADC0();
    InitADC1();
    InitADC2();
    InitInputmux();
    InitOpamps();
    InitFlexPWM();
    InitUART();
    InitComps();
    FMSTR_Init();
    /* Example of PILOT GND HEATER control */
    /* All pins, port numbers,masks and names are defines in board/pin_mux.h file */
    //GPIO_PortSet(PIN_PILOT_GND_REL_HEAT_GPIO, PIN_PILOT_GND_REL_HEAT_GPIO_PIN_MASK);

    /* Enable PWM outputs */
    PWM_EN(FLEXPWM0);
	ADC0->CTRL |= ADC_CTRL_RSTFIFO_MASK;	/* Reset FIFO */
	ADC1->CTRL |= ADC_CTRL_RSTFIFO_MASK;	/* Reset FIFO */
	ADC2->CTRL |= ADC_CTRL_RSTFIFO_MASK;	/* Reset FIFO */
	FLEXPWM0->MCTRL|= PWM_MCTRL_RUN(0xF);	/* Run PWM */
    /* Global ISR enable */
    EnableGlobalIRQ(ui32PrimaskReg);

    while (1)
    {
    	FMSTR_Poll();
    }
}



void ADC0_IRQHandler()
{
	ui32AdcIsrCnt++;

/***** ADC0 RESULTS ******************/
/* It is reasonable to read ADC0 first because the ISR handler was triggered by ADC0 FIFO wattermark */

	/* Get number of results in FIFO */
	ui32Fcount0 = ADC0->FCTRL & ADC_FCTRL_FCOUNT_MASK;
	/* Read and store ADC0 */
	sAdcRaw.ui32Iu = ADC0->RESFIFO;
	sAdcRes.i16Iu = (int16_t)(sAdcRaw.ui32Iu & ADC_RESFIFO_D_MASK);

	/* Read and store ADC0 */
	sAdcRaw.ui32Vbus1 = ADC0->RESFIFO;
	sAdcRes.i16Vbus1 = (int16_t)(sAdcRaw.ui32Vbus1 & ADC_RESFIFO_D_MASK)<<1;
	/* Example of scaling to float */
	fVdcBus = MLIB_ConvSc_FLTsf(sAdcRes.i16Vbus1, fltDcBusVoltageScale);

	/* Read and store ADC0 */
	sAdcRaw.ui32Vbus2 = ADC0->RESFIFO;
	sAdcRes.i16Vbus2 = (int16_t)(sAdcRaw.ui32Vbus2 & ADC_RESFIFO_D_MASK);

	/* Read and store ADC0 */
	sAdcRaw.ui32V3v3 = ADC0->RESFIFO;
	sAdcRes.i16V3v3 = (int16_t)(sAdcRaw.ui32V3v3 & ADC_RESFIFO_D_MASK);

/***** ADC1 RESULTS ******************/
	/* Get number of results in FIFO */
	ui32Fcount1 = ADC1->FCTRL & ADC_FCTRL_FCOUNT_MASK;

	/* Read and store ADC1 - depend on FOC sector and ADC CH setup, it could be current Iv or Iw */
	sAdcRaw.ui32Ivw= ADC1->RESFIFO;
	sAdcRes.i16Ivw = (int16_t)(sAdcRaw.ui32Ivw & ADC_RESFIFO_D_MASK);

	/* Read and store ADC1 - depend on FOC sector adn ADC ch setup it could be current Iv or Iw */
	sAdcRaw.ui32IpmTemp= ADC1->RESFIFO;
	sAdcRes.i16IpmTemp = (int16_t)(sAdcRaw.ui32IpmTemp & ADC_RESFIFO_D_MASK);

/***** ADC2 RESULTS ******************/
	/* Get number of results in FIFO */
	ui32Fcount2 = ADC2->FCTRL & ADC_FCTRL_FCOUNT_MASK;
	/* Read and store ADC2 */
	sAdcRaw.ui32Iw = ADC2->RESFIFO;
	sAdcRes.i16Iw = (int16_t)(sAdcRaw.ui32Iw & ADC_RESFIFO_D_MASK);



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
    PWMBase->SM[0].DISMAP[0] = ((PWMBase->SM[0].DISMAP[0] & ~PWM_DISMAP_DIS0A_MASK) | PWM_DISMAP_DIS0A(0x7));
    PWMBase->SM[1].DISMAP[0] = ((PWMBase->SM[0].DISMAP[0] & ~PWM_DISMAP_DIS0A_MASK) | PWM_DISMAP_DIS0A(0x7));
    PWMBase->SM[2].DISMAP[0] = ((PWMBase->SM[0].DISMAP[0] & ~PWM_DISMAP_DIS0A_MASK) | PWM_DISMAP_DIS0A(0x7));
    PWMBase->SM[0].DISMAP[0] = ((PWMBase->SM[0].DISMAP[0] & ~PWM_DISMAP_DIS0B_MASK) | PWM_DISMAP_DIS0B(0x7));
    PWMBase->SM[1].DISMAP[0] = ((PWMBase->SM[0].DISMAP[0] & ~PWM_DISMAP_DIS0B_MASK) | PWM_DISMAP_DIS0B(0x7));
    PWMBase->SM[2].DISMAP[0] = ((PWMBase->SM[0].DISMAP[0] & ~PWM_DISMAP_DIS0B_MASK) | PWM_DISMAP_DIS0B(0x7));

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

    /* Fault 0,1,2 active in logic level one, manual clearing */
    PWMBase->FCTRL = (PWMBase->FCTRL & ~PWM_FCTRL_FLVL_MASK) | PWM_FCTRL_FLVL(0x7);
    PWMBase->FCTRL = (PWMBase->FCTRL & ~PWM_FCTRL_FAUTO_MASK) | PWM_FCTRL_FAUTO(0x0);

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

static void InitInputmux()
{
	/* dont use SDK driver for that, mistake for ADC was reported there */
#if 0
/*  Pwm0Sm0OutTrig0 connect to Adc0Trigger 0 */
INPUTMUX_AttachSignal(INPUTMUX0, 0U, kINPUTMUX_Pwm0Sm0OutTrig0ToAdc0Trigger);
/*  Pwm0Sm0OutTrig0 connect to Adc1Trigger 0 */
INPUTMUX_AttachSignal(INPUTMUX0, 0U, kINPUTMUX_Pwm0Sm0OutTrig0ToAdc1Trigger);
/*  Pwm0Sm0OutTrig0 connect to Adc2Trigger 0 */
INPUTMUX_AttachSignal(INPUTMUX0, 0U, kINPUTMUX_Pwm0Sm0OutTrig0ToAdc2Trigger);
/*  Cmp0Out connect to FlexPwm0Fault 0 */
INPUTMUX_AttachSignal(INPUTMUX0, 0U, kINPUTMUX_Cmp0OutToFlexPwm0Fault);
/*  Cmp1Out connect to FlexPwm0Fault 1 */
INPUTMUX_AttachSignal(INPUTMUX0, 1U, kINPUTMUX_Cmp1OutToFlexPwm0Fault);
/*  Cmp2Out connect to FlexPwm0Fault 2 */
INPUTMUX_AttachSignal(INPUTMUX0, 2U, kINPUTMUX_Cmp2OutToFlexPwm0Fault);
#endif

	CLOCK_EnableClock(kCLOCK_InputMux);
	/* Release peripheral reset */
	RESET_ReleasePeripheralReset(kINPUTMUX0_RST_SHIFT_RSTn);

	/* PWM0_SM0_OUT_TRIG0 is selected as trigger input for ADC0 */
	INPUTMUX0->ADC0_TRIG[0] = INPUTMUX_ADC0_TRIGM_ADC0_TRIG_TRIGIN(0b010010);
	/* PWM0_SM0_OUT_TRIG0 is selected as trigger input for ADC1 */
	INPUTMUX0->ADC1_TRIG[0] = INPUTMUX_ADC1_TRIGM_ADC1_TRIG_TRIGIN(0b010010);
	/* PWM0_SM0_OUT_TRIG0 is selected as trigger input for ADC2 */
	INPUTMUX0->ADC2_TRIG[0] = INPUTMUX_ADC2_TRIGM_ADC2_TRIG_TRIGIN(0b010010);

	/*  Cmp0Out connect to FlexPwm0Fault 0 */
	INPUTMUX_AttachSignal(INPUTMUX0, 0U, kINPUTMUX_Cmp0OutToFlexPwm0Fault);
	/*  Cmp1Out connect to FlexPwm0Fault 1 */
	INPUTMUX_AttachSignal(INPUTMUX0, 1U, kINPUTMUX_Cmp1OutToFlexPwm0Fault);
	/*  Cmp2Out connect to FlexPwm0Fault 2 */
	INPUTMUX_AttachSignal(INPUTMUX0, 2U, kINPUTMUX_Cmp2OutToFlexPwm0Fault);
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
    lpadcCommandConfig.channelNumber = ADC0CH_CURR_U;		// or ADC0CH_CURR_B in channel mapping function according to FOC sector
    lpadcCommandConfig.chainedNextCommandNumber = 2U;
    LPADC_SetConvCommandConfig( ADC0, 1U, &lpadcCommandConfig );

    lpadcCommandConfig.channelNumber = ADC0CH_VBUS_1;
    lpadcCommandConfig.chainedNextCommandNumber = 3U;
    LPADC_SetConvCommandConfig( ADC0, 2U, &lpadcCommandConfig );

    lpadcCommandConfig.channelNumber = ADC0CH_VBUS_2;
    lpadcCommandConfig.chainedNextCommandNumber = 4U;
    LPADC_SetConvCommandConfig( ADC0, 3U, &lpadcCommandConfig );

    lpadcCommandConfig.channelNumber = ADC0CH_3V3_MEAS;
    lpadcCommandConfig.chainedNextCommandNumber = 0U;
    LPADC_SetConvCommandConfig( ADC0, 4U, &lpadcCommandConfig );



    /* Init triggers (use trigger 0). */
    LPADC_GetDefaultConvTriggerConfig(&lpadcTriggerConfig);
    lpadcTriggerConfig.targetCommandId = 1U;
    lpadcTriggerConfig.enableHardwareTrigger = true;
    LPADC_SetConvTriggerConfig(ADC0, 0U, &lpadcTriggerConfig);

	/* Set watermark for FIFO0 */
	ADC0->FCTRL |= ADC_FCTRL_FWMARK(3);
	/* Enable FIFO0 watermark interrupt */
	ADC0->IE |= ADC_IE_FWMIE0(1);

    NVIC_SetPriority(ADC0_IRQn, 0U);
    NVIC_EnableIRQ(ADC0_IRQn);
}

static void InitComps()
{
	SPC_EnableActiveModeAnalogModules(SPC0, (kSPC_controlCmp0 | kSPC_controlCmp0Dac));
	SPC_EnableActiveModeAnalogModules(SPC0, (kSPC_controlCmp1 | kSPC_controlCmp1Dac));
	SPC_EnableActiveModeAnalogModules(SPC0, (kSPC_controlCmp2 | kSPC_controlCmp2Dac));
	/*
    // Enable CMP power
    SPC0->ACTIVE_CFG1 |= SPC_CFG1_CMP0_EN;
    SPC0->ACTIVE_CFG1 |= SPC_CFG1_CMP1_EN;
    SPC0->ACTIVE_CFG1 |= SPC_CFG1_CMP2_EN;
    // Enable CMP DAC power
    SPC0->ACTIVE_CFG1 |= SPC_CFG1_CMP0_DAC_EN;
    SPC0->ACTIVE_CFG1 |= SPC_CFG1_CMP1_DAC_EN;
    SPC0->ACTIVE_CFG1 |= SPC_CFG1_CMP2_DAC_EN;
    */

    CLOCK_SetClockDiv(kCLOCK_DivCMP0_FUNC, 1u);
    CLOCK_AttachClk(kFRO_HF_DIV_to_CMP0);

    CLOCK_SetClockDiv(kCLOCK_DivCMP1_FUNC, 1u);
    CLOCK_AttachClk(kFRO_HF_DIV_to_CMP1);

    CLOCK_SetClockDiv(kCLOCK_DivCMP2_FUNC, 1u);
    CLOCK_AttachClk(kFRO_HF_DIV_to_CMP2);

	// Turn on the OPAMP clock
	CLOCK_EnableClock(kCLOCK_GateCMP0);
	CLOCK_EnableClock(kCLOCK_GateCMP1);
	CLOCK_EnableClock(kCLOCK_GateCMP2);



	 lpcmp_config_t mLpcmpConfigStruct;
	 lpcmp_dac_config_t mLpcmpDacConfigStruct;

	 /*
	  *   k_LpcmpConfigStruct->enableStopMode      = false;
	  *   k_LpcmpConfigStruct->enableOutputPin     = false;
	  *   k_LpcmpConfigStruct->useUnfilteredOutput = false;
	  *   k_LpcmpConfigStruct->enableInvertOutput  = false;
	  *   k_LpcmpConfigStruct->hysteresisMode      = kLPCMP_HysteresisLevel0;
	  *   k_LpcmpConfigStruct->powerMode           = kLPCMP_LowSpeedPowerMode;
	  *   k_LpcmpConfigStruct->functionalSourceClock = kLPCMP_FunctionalClockSource0;
	  */
	 LPCMP_GetDefaultConfig(&mLpcmpConfigStruct);

	 /* Init the LPCMP module. */
	 LPCMP_Init(CMP0, &mLpcmpConfigStruct);
	 LPCMP_Init(CMP1, &mLpcmpConfigStruct);
	 LPCMP_Init(CMP2, &mLpcmpConfigStruct);

	 /* Configure the internal DAC to output half of reference voltage. */
	 mLpcmpDacConfigStruct.enableLowPowerMode     = false;
	 mLpcmpDacConfigStruct.referenceVoltageSource = kLPCMP_VrefSourceVin1;
	 /* Overcurrent threshold, max value 255 */
	 mLpcmpDacConfigStruct.DACValue = (230U);

	 LPCMP_SetDACConfig(CMP0, &mLpcmpDacConfigStruct);
	 LPCMP_SetDACConfig(CMP1, &mLpcmpDacConfigStruct);
	 LPCMP_SetDACConfig(CMP2, &mLpcmpDacConfigStruct);


	 /* Configure LPCMP input channels. */
	 /* CH7 - internal cmp dac */
	 /* CH4 - opamp out */
	 LPCMP_SetInputChannels(CMP0, 4U, 7U);
	 LPCMP_SetInputChannels(CMP1, 4U, 7U);
	 LPCMP_SetInputChannels(CMP2, 4U, 7U);

}

static void InitDac()
{
	SPC_EnableActiveModeAnalogModules(SPC0, (kSPC_controlDac0));
	// Turn on the DAC clock
	CLOCK_EnableClock(kCLOCK_GateDAC0);


}

static void InitOpamps()
{
	SPC_EnableActiveModeAnalogModules(SPC0, kSPC_controlOpamp0);
	SPC_EnableActiveModeAnalogModules(SPC0, kSPC_controlOpamp1);
	SPC_EnableActiveModeAnalogModules(SPC0, kSPC_controlOpamp2);
/*
    // Enable OPMAP power
    SPC0->ACTIVE_CFG1 |= SPC_CFG1_OPAMP0_EN;
    SPC0->ACTIVE_CFG1 |= SPC_CFG1_OPAMP1_EN;
    SPC0->ACTIVE_CFG1 |= SPC_CFG1_OPAMP2_EN;
*/
	// Turn on the OPAMP clock
	CLOCK_EnableClock(kCLOCK_GateOPAMP0);
	CLOCK_EnableClock(kCLOCK_GateOPAMP1);
	CLOCK_EnableClock(kCLOCK_GateOPAMP2);
	// Configure and turn on the OPAMP
	OPAMP0->OPAMP_CTRL =
			OPAMP_OPAMP_CTRL_OPA_EN(1U)         |   /* Enable the opamp */
	        OPAMP_OPAMP_CTRL_OPA_BC_SEL(0b00)	|	/* Bias current config selection. Default value. Keep power consumption constant */
			OPAMP_OPAMP_CTRL_OPA_CC_SEL(0b11);   	/* Compensation capacitor config selection. Fit 16X gains */
	// Configure and turn on the OPAMP
	OPAMP1->OPAMP_CTRL =
			OPAMP_OPAMP_CTRL_OPA_EN(1U)         |   /* Enable the opamp */
	        OPAMP_OPAMP_CTRL_OPA_BC_SEL(0b00)	|	/* Bias current config selection. Default value. Keep power consumption constant */
			OPAMP_OPAMP_CTRL_OPA_CC_SEL(0b11);   	/* Compensation capacitor config selection. Fit 16X gains */

	// Configure and turn on the OPAMP
	OPAMP2->OPAMP_CTRL =
			OPAMP_OPAMP_CTRL_OPA_EN(1U)         |   /* Enable the opamp */
	        OPAMP_OPAMP_CTRL_OPA_BC_SEL(0b00)	|	/* Bias current config selection. Default value. Keep power consumption constant */
			OPAMP_OPAMP_CTRL_OPA_CC_SEL(0b11);   	/* Compensation capacitor config selection. Fit 16X gains */
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
    lpadcCommandConfig.sampleTimeMode = kLPADC_SampleTimeADCK3;

    /* Init ADC channels */
    lpadcCommandConfig.channelNumber = ADC1CH_CURR_V;// or ADC1CH_CURR_W channel mapping function according to FOC sector
    lpadcCommandConfig.chainedNextCommandNumber = 2;
    LPADC_SetConvCommandConfig( ADC1, 1U, &lpadcCommandConfig );

    lpadcCommandConfig.channelNumber = ADC1CH_IPMTEMP;
    lpadcCommandConfig.chainedNextCommandNumber = 0U;
    LPADC_SetConvCommandConfig( ADC1, 2U, &lpadcCommandConfig );


    /* Init triggers (use trigger 0). */
    LPADC_GetDefaultConvTriggerConfig(&lpadcTriggerConfig);
    lpadcTriggerConfig.targetCommandId = 1U;
    lpadcTriggerConfig.enableHardwareTrigger = true;
    LPADC_SetConvTriggerConfig(ADC1, 0U, &lpadcTriggerConfig);

}

static void InitADC2()
{
	RESET_ReleasePeripheralReset(kADC2_RST_SHIFT_RSTn);

    lpadc_conv_trigger_config_t lpadcTriggerConfig;
    lpadc_conv_command_config_t lpadcCommandConfig;
    lpadc_config_t lpadcConfig;

    /* Init the lpadcConfig struct */
    LPADC_GetDefaultConfig(&lpadcConfig);
    lpadcConfig.enableAnalogPreliminary = true;
    lpadcConfig.referenceVoltageSource = kLPADC_ReferenceVoltageAlt3;
    lpadcConfig.conversionAverageMode = kLPADC_ConversionAverage1;

    LPADC_Init(ADC2, &lpadcConfig);

    LPADC_DoOffsetCalibration(ADC2);
    LPADC_DoAutoCalibration(ADC2);

    LPADC_GetDefaultConvCommandConfig(&lpadcCommandConfig);
    lpadcCommandConfig.sampleChannelMode = kLPADC_SampleChannelSingleEndSideA;
    lpadcCommandConfig.conversionResolutionMode = kLPADC_ConversionResolutionStandard;
    lpadcCommandConfig.sampleTimeMode = kLPADC_SampleTimeADCK3;

    /* Init ADC channels */
    lpadcCommandConfig.channelNumber = ADC2CH_CURR_W;
    lpadcCommandConfig.chainedNextCommandNumber = 0;
    LPADC_SetConvCommandConfig( ADC2, 1U, &lpadcCommandConfig );


    /* Init triggers (use trigger 0). */
    LPADC_GetDefaultConvTriggerConfig(&lpadcTriggerConfig);
    lpadcTriggerConfig.targetCommandId = 1U;
    lpadcTriggerConfig.enableHardwareTrigger = true;
    LPADC_SetConvTriggerConfig(ADC2, 0U, &lpadcTriggerConfig);

}

static void InitUART()
{
	CLOCK_SetClockDiv(kCLOCK_DivLPUART4, 1u);
	CLOCK_AttachClk(kFRO_LF_DIV_to_LPUART4);
	RESET_PeripheralReset(kLPUART4_RST_SHIFT_RSTn);

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
