/*
 * drv.c
 *
 *  Created on: 10 Şub 2025
 *      Author: 26098107
 */


#include "drv.h"
#include "drv_config.h"
#include "mcinclude.h"
#include "dw_globals.h"

#include "core_cm33.h"
#include "safety_cm33_mcx.h"

uint32_t ui32SysticIsrCnt;
uint32_t ui32NoInit FS_NOINIT_RAM_LOC;

/*******************************************************************************
 * Variables
 ******************************************************************************/

volatile uint32_t ui32Fcount0;
volatile uint32_t ui32Fcount1;
volatile uint32_t ui32Fcount2;
volatile uint32_t ui32Fcount3;
uint16_t  ui16Modulo;
sAdcRawResult sAdcRaw;
sAdcResult sAdcRes;


FMSTR_U8 FreeMASTER_RecBuffer0[FREEMASTER_REC_0_SIZE];
FMSTR_REC_BUFF FreeMASTER_Recorder_0 = {
  .name = "Description of recorder 0",
  .addr = (FMSTR_ADDR)FreeMASTER_RecBuffer0,
  .size = FREEMASTER_REC_0_SIZE,
  .basePeriod_ns = 100000UL
};

#define RMS_CYCLE_NUM					(700.0f)

void (*drvHighFreqHandlerMx)(int32_t MxIndex) = NULL;	// callback function pointer for high frequency handler
void (*drvLowFreqHandlerMx)(int32_t MxIndex) = NULL;	// callback function pointer for low frequency handler

void (*drvHighFreqHandlerApp)(void) = NULL;	// callback function pointer for high frequency handler
void (*drvLowFreqHandlerApp)(void) = NULL;	// callback function pointer for high frequency handler



uint16_t adcWa = 16384;
uint16_t adcVa = 16384;
uint16_t adcVb = 16384;
uint16_t adcUb = 16384;

uint16_t adcWa_M2 = 16384;
uint16_t adcVa_M2 = 16384;
uint16_t adcVb_M2 = 16384;
uint16_t adcUb_M2 = 16384;

uint16_t adcOffsetWa = 16384;
uint16_t adcOffsetVa = 16384;
uint16_t adcOffsetVb = 16384;
uint16_t adcOffsetUb = 16384;

uint16_t adcOffsetWa_M2 = 16384;
uint16_t adcOffsetVa_M2 = 16384;
uint16_t adcOffsetVb_M2 = 16384;
uint16_t adcOffsetUb_M2 = 16384;

float currentUb = 0;
float currentVb = 0;
float currentVa = 0;
float currentWa = 0;

float currentUb_M2 = 0;
float currentVb_M2 = 0;
float currentVa_M2 = 0;
float currentWa_M2 = 0;


uint32_t ui32PrimaskReg;

r_mtr_adc_tb mtr_ad_data_M1;
r_mtr_adc_tb mtr_ad_data_M2;

float adcDcBusVoltage;


//uint8_t ipmFaultOutHandled;
uint8_t ipmFaultOutHandled_M1;
uint8_t ipmFaultOutHandled_M2;


uint16_t u2_count_u;
uint16_t u2_count_v;
uint16_t u2_count_w;
uint16_t u2_count_u_M2;
uint16_t u2_count_v_M2;
uint16_t u2_count_w_M2;


float ipmTemperature_M1 = 0.0f;
float ipmTemperature_M2 = 0.0f;
float ipmOldTemperature_M1 = 0.0f;
float ipmOldTemperature_M2 = 0.0f;
float adcTempIPM_M1 = 0.0f;
float adcTempIPM_M2 = 0.0f;


McSpeedReq_t refSpeed_M0;
McSpeedReq_t refSpeed_M1;


uint8_t rxBuffer[RX_BUFFER_SIZE];
uint8_t txBuffer[TX_BUFFER_SIZE];

volatile uint32_t  tickCounter = 0;


uint32_t pwmDisCnt=0;



rmsCalcPar_t rmsCircPhWObj;
rmsCalcPar_t rmsCircPhUObj;
rmsCalcPar_t rmsCircPhW_M2Obj;
rmsCalcPar_t rmsCircPhU_M2Obj;
float rmsCircPhW;
float rmsCircPhU;
float rmsCircPhW_M2;
float rmsCircPhU_M2;
volatile uint32_t uartTimerTest=0;
volatile uint32_t diverterFbCnt=0;
volatile uint16_t adcValDiverter;
volatile uint16_t adcDiverter;
volatile uint16_t adcValSyncPin;
volatile float inputPower;
volatile float powerSum;
volatile float avgPower;
volatile uint32_t powerCnt;
PWM_Type *PWMBase = (PWM_Type *)FLEXPWM0_BASE;

volatile float activePower;
volatile float reactivePower;
volatile float appPower;
volatile float valHeat;
volatile float torqueCirc[4];
volatile float torqueDrain[4];
volatile uint32_t Torque_Meas_Counter;
volatile uint8_t Torque_Meas_Index;
volatile uint8_t shortCircuitFlag = 0;
volatile uint32_t scTimeCnt = 0;
volatile uint8_t shortCircuitFlag_M2 = 0;
volatile uint32_t scTimeCnt_M2 = 0;
volatile uint32_t scFuncCnt = 0;
volatile uint8_t scFuncEn = 0;
volatile float lpfCoeffDiverter = 0.05f;

#define PRESC_25MS_SWFREQ       (uint32_t)(PWM_FREQ_HZ / 40U)


const uint8_t crcTable[] = {
 0x00, 0x07, 0x0e, 0x09, 0x1c, 0x1b, 0x12, 0x15, 0x38, 0x3f, 0x36, 0x31,
 0x24, 0x23, 0x2a, 0x2d, 0x70, 0x77, 0x7e, 0x79, 0x6c, 0x6b, 0x62, 0x65,
 0x48, 0x4f, 0x46, 0x41, 0x54, 0x53, 0x5a, 0x5d, 0xe0, 0xe7, 0xee, 0xe9,
 0xfc, 0xfb, 0xf2, 0xf5, 0xd8, 0xdf, 0xd6, 0xd1, 0xc4, 0xc3, 0xca, 0xcd,
 0x90, 0x97, 0x9e, 0x99, 0x8c, 0x8b, 0x82, 0x85, 0xa8, 0xaf, 0xa6, 0xa1,
 0xb4, 0xb3, 0xba, 0xbd, 0xc7, 0xc0, 0xc9, 0xce, 0xdb, 0xdc, 0xd5, 0xd2,
 0xff, 0xf8, 0xf1, 0xf6, 0xe3, 0xe4, 0xed, 0xea, 0xb7, 0xb0, 0xb9, 0xbe,
 0xab, 0xac, 0xa5, 0xa2, 0x8f, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9d, 0x9a,
 0x27, 0x20, 0x29, 0x2e, 0x3b, 0x3c, 0x35, 0x32, 0x1f, 0x18, 0x11, 0x16,
 0x03, 0x04, 0x0d, 0x0a, 0x57, 0x50, 0x59, 0x5e, 0x4b, 0x4c, 0x45, 0x42,
 0x6f, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7d, 0x7a, 0x89, 0x8e, 0x87, 0x80,
 0x95, 0x92, 0x9b, 0x9c, 0xb1, 0xb6, 0xbf, 0xb8, 0xad, 0xaa, 0xa3, 0xa4,
 0xf9, 0xfe, 0xf7, 0xf0, 0xe5, 0xe2, 0xeb, 0xec, 0xc1, 0xc6, 0xcf, 0xc8,
 0xdd, 0xda, 0xd3, 0xd4, 0x69, 0x6e, 0x67, 0x60, 0x75, 0x72, 0x7b, 0x7c,
 0x51, 0x56, 0x5f, 0x58, 0x4d, 0x4a, 0x43, 0x44, 0x19, 0x1e, 0x17, 0x10,
 0x05, 0x02, 0x0b, 0x0c, 0x21, 0x26, 0x2f, 0x28, 0x3d, 0x3a, 0x33, 0x34,
 0x4e, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5c, 0x5b, 0x76, 0x71, 0x78, 0x7f,
 0x6a, 0x6d, 0x64, 0x63, 0x3e, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2c, 0x2b,
 0x06, 0x01, 0x08, 0x0f, 0x1a, 0x1d, 0x14, 0x13, 0xae, 0xa9, 0xa0, 0xa7,
 0xb2, 0xb5, 0xbc, 0xbb, 0x96, 0x91, 0x98, 0x9f, 0x8a, 0x8d, 0x84, 0x83,
 0xde, 0xd9, 0xd0, 0xd7, 0xc2, 0xc5, 0xcc, 0xcb, 0xe6, 0xe1, 0xe8, 0xef,
 0xfa, 0xfd, 0xf4, 0xf3
};



//////////////////////////// variable definitions - stop ///////////////////////


//////////////////////////// function definitions - start //////////////////////

void drvInitMcu(void (*_McFuncFastLoop)(int32_t MxIndex),
                void (*_McFuncSlowLoop)(int32_t MxIndex),
                void (*drvFastFreqHandlerApp)(void),
                void (*drvSlowFreqHandlerApp)(void),
                void (*drvAposIntHandlerApp)(void))
{


    uint32_t ui32PrimaskReg;

    //ui32PrimaskReg = DisableGlobalIRQ();
    __disable_irq();
    ui32NoInit = ui32NoInit + 1;
	BOARD_BootClockFROHF180M();
	ui16Modulo = PWM_MODULO;
	BOARD_InitPins();

	/*
    //WD_REFRESH;
    FsAfterResetVarInit();

    FsResetCheck();

    FsWd();

    FsAfterResetCpuTests();
    WD_REFRESH;

    FsAfterResetMemTests();
    WD_REFRESH;

    FsAfterResetClockTest();
    WD_REFRESH;

    FsCheckGlobalFault();
    WD_REFRESH;

    FsFlashTestRtInit();
*/


    InitADC0();
    InitADC1();
    InitADC2();
    InitADC3();
    InitInputmux();
    InitOpamps();
    InitFlexPWM();
    InitUART();
    InitComps();
    InitDac();
    SlowLoopInt();
    InitGpioInt();
    FMSTR_Init();
    SysTick_Config(SystemCoreClock / 1000);  // 1ms SysTick (100 ms sayımı için)
    NVIC_SetPriority(SysTick_IRQn, 0U);
    /* Example of PILOT GND HEATER control */
    /* All pins, port numbers,masks and names are defines in board/pin_mux.h file */
    //GPIO_PortSet(PIN_PILOT_GND_REL_HEAT_GPIO, PIN_PILOT_GND_REL_HEAT_GPIO_PIN_MASK);

    /* Enable PWM outputs */
    PWM_EN(FLEXPWM0);
	ADC0->CTRL |= ADC_CTRL_RSTFIFO_MASK;	/* Reset FIFO */
	ADC1->CTRL |= ADC_CTRL_RSTFIFO_MASK;	/* Reset FIFO */
	ADC2->CTRL |= ADC_CTRL_RSTFIFO_MASK;	/* Reset FIFO */
	FLEXPWM0->MCTRL|= PWM_MCTRL_RUN(0xF);	/* Run PWM */

	PWMBase->FSTS = (PWMBase->FSTS & ~PWM_FSTS_FFLAG_MASK) | PWM_FSTS_FFLAG(0xF);
    /* Global ISR enable */
    //EnableGlobalIRQ(ui32PrimaskReg);
	__enable_irq();

    if(_McFuncFastLoop != NULL)
    	drvHighFreqHandlerMx = _McFuncFastLoop;
	if(_McFuncSlowLoop != NULL)
	    drvLowFreqHandlerMx = _McFuncSlowLoop;

	if(drvFastFreqHandlerApp != NULL)
	    drvHighFreqHandlerApp = drvFastFreqHandlerApp;
	if(drvSlowFreqHandlerApp != NULL)
	    drvLowFreqHandlerApp = drvSlowFreqHandlerApp;

	Torque_Meas_Counter = PRESC_25MS_SWFREQ - 1;
	Torque_Meas_Index = 0;

	for(uint8_t i = 0; i<4; i++)
	{
		torqueCirc[i] = 0.0f;
		torqueDrain[i] = 0.0f;
	}

}


void drvDisableInterrupts(void)
{
    //ui32PrimaskReg = DisableGlobalIRQ();
	__disable_irq();
}

void drvEnableInterrupts(void)
{
    //EnableGlobalIRQ(ui32PrimaskReg);
	__enable_irq();
}




/*!
 * @brief   Watchdog configuration function
 *
 * This function inits the watchdog in timeout mode according to defined macros.
 *
 * @param u32Timeout   Timeout value.
 * @param u32Prescaler Prescale of the clock.
 * @param eClockSource Timer clock source selection.
 *
 * @return  None
 */
void drvWatchdogEnable(void)
{

}

/*!
 * @brief   Watchdog disabling function
 *
 * @param   void
 *
 * @return  None
 */
void drvWatchdogDisable(void)
{

}

void drvWatchdogResetHandlerFL(void)
{

}

void drvResetWdg(void)
{

}


void drvPwmEnable(void)
{
    PWM_EN(FLEXPWM0);
}

void drvPwmEnable_M2(void)
{
    PWM_EN(FLEXPWM0);

}

void drvPwmDisable(void)
{
	pwmDisCnt++;
    PWM_DIS(FLEXPWM0);
}


void drvPwmDisable_M2(void)
{
    PWM_DIS(FLEXPWM0);
}

void drvPwmEnablePhaseU(void)
{
	PWM_EN(FLEXPWM0);
}

void drvPwmEnablePhaseU_M2(void)
{
	PWM_EN(FLEXPWM0);
}

void drvPwmEnablePhaseV(void)
{
	PWM_EN(FLEXPWM0);
}

void drvPwmEnablePhaseV_M2(void)
{
	PWM_EN(FLEXPWM0);
}

void drvPwmEnablePhaseW(void)
{
	PWM_EN(FLEXPWM0);
}

void drvPwmEnablePhaseW_M2(void)
{
	PWM_EN(FLEXPWM0);
}

void drvPwmEnablePhaseUVW(void)
{
	PWM_EN(FLEXPWM0);
}

void drvPwmEnablePhaseUVW_M2(void)
{
	PWM_EN(FLEXPWM0);
}


void drvSetPwmDuties(float dutyU, float dutyV, float dutyW)
{
	u2_count_u = (uint16_t)(32768 * (1 - dutyU));
    u2_count_v = (uint16_t)(32768 * (1 - dutyV));
    u2_count_w = (uint16_t)(32768 * (1 - dutyW));

    if(u2_count_u > 31130.0f)
    	u2_count_u = 31130;
    else if(u2_count_u < 1638.0f)
    	u2_count_u = 1638;

    if(u2_count_v > 31130.0f)
    	u2_count_v = 31130;
    else if(u2_count_v < 1638.0f)
    	u2_count_v = 1638;

    if(u2_count_w > 31130.0f)
    	u2_count_w = 31130;
    else if(u2_count_w < 1638.0f)
    	u2_count_w = 1638;

	UpdatePWM(u2_count_u, u2_count_v, u2_count_w, FLEXPWM0);

}

void drvSetPwmDuties_M2(float dutyU, float dutyV, float dutyW)
{

	u2_count_u_M2 = (uint16_t)(32768 * (1 - dutyU));
	u2_count_v_M2 = (uint16_t)(32768 * (1 - dutyV));
	u2_count_w_M2 = (uint16_t)(32768 * (1 - dutyW));

    if(u2_count_u_M2 > 31130.0f)
    	u2_count_u_M2 = 31130;
    else if(u2_count_u_M2 < 1638.0f)
    	u2_count_u_M2 = 1638;

    if(u2_count_v_M2 > 31130.0f)
    	u2_count_v_M2 = 31130;
    else if(u2_count_v_M2 < 1638.0f)
    	u2_count_v_M2 = 1638;

    if(u2_count_w_M2 > 31130.0f)
    	u2_count_w_M2 = 31130;
    else if(u2_count_w_M2 < 1638.0f)
    	u2_count_w_M2 = 1638;


	UpdatePWM(u2_count_u_M2, u2_count_v_M2, u2_count_w_M2, FLEXPWM0);

}

void drvShortCircuitBottomTransistors(void)
{

}

int drvGetAdcWa(void)
{
	return adcWa;
}

int drvGetAdcWa_M2(void)
{
	return adcWa_M2;
}

int drvGetAdcVa(void)
{
	return adcVa;
}

int drvGetAdcVa_M2(void)
{
	return adcVa_M2;
}

int drvGetAdcVb(void)
{
	return adcVb;
}

int drvGetAdcVb_M2(void)
{
	return adcVb_M2;
}

int drvGetAdcUb(void)
{
	return adcUb;
}

int drvGetAdcUb_M2(void)
{
	return adcUb_M2;
}

void M1_AdcGetConvVal( r_mtr_adc_tb *mtr_ad_data )
{
	/***** ADC0 RESULTS ******************/

	/* Get number of results in FIFO */
	ui32Fcount0 = ADC0->FCTRL & ADC_FCTRL_FCOUNT_MASK;

	/* Read and store ADC0 */
	sAdcRaw.ui32Iu = ADC0->RESFIFO;
	sAdcRes.i16Iu = (int16_t)(sAdcRaw.ui32Iu & ADC_RESFIFO_D_MASK);
	mtr_ad_data->u2_iu_ad = (uint16_t)(sAdcRes.i16Iu);

	/* Read and store ADC0 */
	sAdcRaw.ui32Diverter = ADC0->RESFIFO;
	sAdcRes.i16Diverter = (int16_t)(sAdcRaw.ui32Diverter & ADC_RESFIFO_D_MASK);
	adcDiverter = (uint16_t)(sAdcRes.i16Diverter);
	adcValDiverter += lpfCoeffDiverter*(adcDiverter - adcValDiverter);

    /* Read and store ADC0 */
	sAdcRaw.ui32Vbus2 = ADC0->RESFIFO;
	sAdcRes.i16Vbus2 = (int16_t)(sAdcRaw.ui32Vbus2 & ADC_RESFIFO_D_MASK);

	/* Read and store ADC0 */
	sAdcRaw.ui32V3v3 = ADC0->RESFIFO;
	sAdcRes.i16V3v3 = (int16_t)(sAdcRaw.ui32V3v3 & ADC_RESFIFO_D_MASK);
	mtr_ad_data->u2_3v3_ad = sAdcRes.i16V3v3;

	/***** ADC1 RESULTS ******************/
	/* Get number of results in FIFO */
	ui32Fcount1 = ADC1->FCTRL & ADC_FCTRL_FCOUNT_MASK;

	/* Read and store ADC1 - depend on FOC sector and ADC CH setup, it could be current Iv or Iw */
	sAdcRaw.ui32Ivw = ADC1->RESFIFO;
	sAdcRes.i16Ivw = (int16_t)(sAdcRaw.ui32Ivw & ADC_RESFIFO_D_MASK);
	mtr_ad_data->u2_iv_ad = (uint16_t)(sAdcRes.i16Ivw);

	/* Read and store ADC1 */
	sAdcRaw.ui32IpmTemp = ADC1->RESFIFO;
	sAdcRes.i16IpmTemp = (int16_t)(sAdcRaw.ui32IpmTemp & ADC_RESFIFO_D_MASK);
    mtr_ad_data->u2_tempIPM_ad  = (uint16_t)(sAdcRes.i16IpmTemp);
    adcTempIPM_M1 = mtr_ad_data->u2_tempIPM_ad;


    /***** ADC2 RESULTS ******************/
    /* Get number of results in FIFO */
    ui32Fcount2 = ADC2->FCTRL & ADC_FCTRL_FCOUNT_MASK;
    /* Read and store ADC2 */
	sAdcRaw.ui32Iw = ADC2->RESFIFO;
	sAdcRes.i16Iw = (int16_t)(sAdcRaw.ui32Iw & ADC_RESFIFO_D_MASK);
	mtr_ad_data->u2_iw_ad = (uint16_t)(sAdcRes.i16Iw);

    /***** ADC3 RESULTS ******************/

    ui32Fcount3 = ADC3->FCTRL & ADC_FCTRL_FCOUNT_MASK;
	sAdcRaw.ui32SyncPin = ADC3->RESFIFO;
	sAdcRes.i16SyncPin = (int16_t)(sAdcRaw.ui32SyncPin & ADC_RESFIFO_D_MASK);
	adcValSyncPin = (uint16_t)(sAdcRes.i16SyncPin);

}

void M2_AdcGetConvVal( r_mtr_adc_tb *mtr_ad_data )
{
	/***** ADC0 RESULTS ******************/

	/* Get number of results in FIFO */
	ui32Fcount0 = ADC0->FCTRL & ADC_FCTRL_FCOUNT_MASK;

	/* Read and store ADC0 */
	sAdcRaw.ui32Iu = ADC0->RESFIFO;
	sAdcRes.i16Iu = (int16_t)(sAdcRaw.ui32Iu & ADC_RESFIFO_D_MASK);
	mtr_ad_data->u2_iu_ad = (uint16_t)(sAdcRes.i16Iu);

	/* Read and store ADC0 */
	sAdcRaw.ui32Diverter = ADC0->RESFIFO;
	sAdcRes.i16Diverter = (int16_t)(sAdcRaw.ui32Diverter & ADC_RESFIFO_D_MASK);
	adcDiverter = (uint16_t)(sAdcRes.i16Diverter);
	adcValDiverter += lpfCoeffDiverter*(adcDiverter - adcValDiverter);

    /* Read and store ADC0 */
	sAdcRaw.ui32Vbus2 = ADC0->RESFIFO;
	sAdcRes.i16Vbus2 = (int16_t)(sAdcRaw.ui32Vbus2 & ADC_RESFIFO_D_MASK);

	/* Read and store ADC0 */
	sAdcRaw.ui32V3v3 = ADC0->RESFIFO;
	sAdcRes.i16V3v3 = (int16_t)(sAdcRaw.ui32V3v3 & ADC_RESFIFO_D_MASK);
	mtr_ad_data->u2_3v3_ad = sAdcRes.i16V3v3;

	/***** ADC1 RESULTS ******************/
	/* Get number of results in FIFO */
	ui32Fcount1 = ADC1->FCTRL & ADC_FCTRL_FCOUNT_MASK;

	/* Read and store ADC1 - depend on FOC sector and ADC CH setup, it could be current Iv or Iw */
	sAdcRaw.ui32Ivw = ADC1->RESFIFO;
	sAdcRes.i16Ivw = (int16_t)(sAdcRaw.ui32Ivw & ADC_RESFIFO_D_MASK);
	mtr_ad_data->u2_iv_ad = (uint16_t)(sAdcRes.i16Ivw);

	/* Read and store ADC1 */
	sAdcRaw.ui32IpmTemp = ADC1->RESFIFO;
	sAdcRes.i16IpmTemp = (int16_t)(sAdcRaw.ui32IpmTemp & ADC_RESFIFO_D_MASK);
    mtr_ad_data->u2_tempIPM_ad  = (uint16_t)(sAdcRes.i16IpmTemp);
    adcTempIPM_M1 = mtr_ad_data->u2_tempIPM_ad;


    /***** ADC2 RESULTS ******************/
    /* Get number of results in FIFO */
    ui32Fcount2 = ADC2->FCTRL & ADC_FCTRL_FCOUNT_MASK;
    /* Read and store ADC2 */
	sAdcRaw.ui32Iw = ADC2->RESFIFO;
	sAdcRes.i16Iw = (int16_t)(sAdcRaw.ui32Iw & ADC_RESFIFO_D_MASK);
	mtr_ad_data->u2_iw_ad = (uint16_t)(sAdcRes.i16Iw);

    /***** ADC3 RESULTS ******************/

    ui32Fcount3 = ADC3->FCTRL & ADC_FCTRL_FCOUNT_MASK;
	sAdcRaw.ui32SyncPin = ADC3->RESFIFO;
	sAdcRes.i16SyncPin = (int16_t)(sAdcRaw.ui32SyncPin & ADC_RESFIFO_D_MASK);
	adcValSyncPin = (uint16_t)(sAdcRes.i16SyncPin);

}

void drvSampleAdcChannels(uint8_t sec)
{
	//M1_AdcGetConvVal(&mtr_ad_data_M1);
	adcUb = mtr_ad_data_M1.u2_iu_ad;
	adcVa = mtr_ad_data_M1.u2_iv_ad;
	adcWa = mtr_ad_data_M1.u2_iw_ad;
	adcDcBusVoltage = sAdcRes.i16Vbus2/VOLTAGE_AMP*ADC_STEP;
    adcTempIPM_M1 = mtr_ad_data_M1.u2_tempIPM_ad;

}

void drvSampleAdcChannels_M2(uint8_t sec)
{
	//M2_AdcGetConvVal(&mtr_ad_data_M2);
	adcUb_M2 = mtr_ad_data_M2.u2_iu_ad;
	adcVa_M2 = mtr_ad_data_M2.u2_iv_ad;
	adcWa_M2 = mtr_ad_data_M2.u2_iw_ad;
	adcDcBusVoltage = sAdcRes.i16Vbus2/VOLTAGE_AMP*ADC_STEP;
    adcTempIPM_M2 = mtr_ad_data_M2.u2_tempIPM_ad;
}


void drvMapAdcChannels(uint8_t sec)
{

}

void drvMapAdcChannels_M2(uint8_t sec)
{

}

void drvSamplePhaseCurrents(void)
{
	currentUb = ((float)adcUb - (float)adcOffsetUb)/(float)CURRENT_AMP*(float)ADC_STEP;
	currentVa = ((float)adcVa - (float)adcOffsetVa)/(float)CURRENT_AMP*(float)ADC_STEP;
	currentWa = ((float)adcWa - (float)adcOffsetWa)/(float)CURRENT_AMP*(float)ADC_STEP;

	appRMSCalc(&currentWa, &rmsCircPhW, RMS_CYCLE_NUM, &rmsCircPhWObj);

    Torque_Meas_Counter++;
    if(Torque_Meas_Counter >= PRESC_25MS_SWFREQ) //188 is approx. 25ms @ 7.55Khz handler.
    {
    	torqueCirc[Torque_Meas_Index] = rmsCircPhW * mpv[CIRCULATION].p.phys.Kt;
    	Torque_Meas_Index++;
    	if(Torque_Meas_Index >= 4)
    	{
    		Torque_Meas_Index = 0;
    	}
    	Torque_Meas_Counter = 0;
    }
}

void drvSamplePhaseCurrents_M2(void)
{
	currentUb_M2 = ((float)adcUb_M2 - (float)adcOffsetUb_M2)/(float)CURRENT_AMP*(float)ADC_STEP;
	currentVa_M2 = ((float)adcVa_M2 - (float)adcOffsetVa_M2)/(float)CURRENT_AMP*(float)ADC_STEP;
	currentWa_M2 = ((float)adcWa_M2 - (float)adcOffsetWa_M2)/(float)CURRENT_AMP*(float)ADC_STEP;

	appRMSCalc(&currentWa_M2, &rmsCircPhW_M2, RMS_CYCLE_NUM, &rmsCircPhW_M2Obj);

    Torque_Meas_Counter++;
    if(Torque_Meas_Counter >= PRESC_25MS_SWFREQ) //188 is approx. 25ms @ 7.55Khz handler.
    {
    	torqueDrain[Torque_Meas_Index] = rmsCircPhW_M2 * mpv[DRAIN].p.phys.Kt;
    	Torque_Meas_Index++;
    	if(Torque_Meas_Index >= 4)
    	{
    		Torque_Meas_Index = 0;
    	}
    	Torque_Meas_Counter = 0;
    }
}

float drvGetDcBusVoltage(void)
{
    return adcDcBusVoltage;
}

float drvGetDcBusVoltage_M2(void)
{
    return adcDcBusVoltage;
}

void drvPumpSet(void)
{
	GPIO_PortSet(PIN_PUMP_SET_GPIO, 1U << PIN_PUMP_SET_PIN);
}

void drvPumpReset(void)
{
	GPIO_PortClear(PIN_PUMP_SET_GPIO, 1U << PIN_PUMP_SET_PIN);
}

void drvDispenserSet(void)
{
	GPIO_PortSet(PIN_PILOT_EV_DISP_GPIO, 1U << PIN_PILOT_EV_DISP_GPIO_PIN);
}

void drvDispenserReset(void)
{
	GPIO_PortClear(PIN_PILOT_EV_DISP_GPIO, 1U << PIN_PILOT_EV_DISP_GPIO_PIN);
}

void drvRegenSet(void)
{
	GPIO_PortSet(PIN_PILOT_EV_REGEN_GPIO, 1U << PIN_PILOT_EV_REGEN_GPIO_PIN);
}

void drvRegenReset(void)
{
	GPIO_PortClear(PIN_PILOT_EV_REGEN_GPIO, 1U << PIN_PILOT_EV_REGEN_GPIO_PIN);
}

void drvH2OSet(void)
{
	GPIO_PortSet(PIN_PILOT_EV_H2O_GPIO, 1U << PIN_PILOT_EV_H2O_GPIO_PIN);
}
void drvH2OReset(void)
{
	GPIO_PortClear(PIN_PILOT_EV_H2O_GPIO, 1U << PIN_PILOT_EV_H2O_GPIO_PIN);
}

void drvDiverterSet(void)
{
	GPIO_PortSet(PIN_PILOT_DIVER_GPIO, 1U << PIN_PILOT_DIVER_GPIO_PIN);
}
void drvDiverterReset(void)
{
	GPIO_PortClear(PIN_PILOT_DIVER_GPIO, 1U << PIN_PILOT_DIVER_GPIO_PIN);
}

void drvRelayHeaterSet(void)
{
	GPIO_PortSet(PIN_PILOT_GND_REL_HEAT_GPIO, 1U << PIN_PILOT_GND_REL_HEAT_GPIO_PIN);
}

void drvRelayHeaterReset(void)
{
	GPIO_PortClear(PIN_PILOT_GND_REL_HEAT_GPIO, 1U << PIN_PILOT_GND_REL_HEAT_GPIO_PIN);
}

void drvSwitchHeaterSet(void)
{
	GPIO_PortSet(PIN_PILOT_GND_SWITCH_REL_GPIO, 1U << PIN_PILOT_GND_SWITCH_REL_GPIO_PIN);
}

void drvSwitchHeaterReset(void)
{
	GPIO_PortClear(PIN_PILOT_GND_SWITCH_REL_GPIO, 1U << PIN_PILOT_GND_SWITCH_REL_GPIO_PIN);
}

void drvPilotFailureTestSet(void)
{
	GPIO_PortSet(PIN_PILOT_FAIL_TST_BULK_GPIO, 1U << PIN_PILOT_FAIL_TST_BULK_GPIO_PIN);
}

void drvPilotFailureTestReset(void)
{
	GPIO_PortClear(PIN_PILOT_FAIL_TST_BULK_GPIO, 1U << PIN_PILOT_FAIL_TST_BULK_GPIO_PIN);
}

void tempIPMcalc(float* adcTemp, float* realTemp)
{
	*realTemp = -0.000000000009f*(*adcTemp)*(*adcTemp)*(*adcTemp) + 0.0000005f*(*adcTemp)*(*adcTemp) - 0.0131f*(*adcTemp)+210.29f;
}
float drvGetTempIPM_M1(void)
{

	tempIPMcalc(&adcTempIPM_M1, &ipmTemperature_M1);
	ipmTemperature_M1 = (1.0f - 0.8f)*(ipmTemperature_M1) + 0.8f*ipmOldTemperature_M1;

	ipmOldTemperature_M1 = ipmTemperature_M1;
	return ipmTemperature_M1;

}
float drvGetTempIPM_M2(void)
{

	tempIPMcalc(&adcTempIPM_M2, &ipmTemperature_M2);
	ipmTemperature_M2 = (1.0f - 0.8f)*(ipmTemperature_M2) + 0.8f*ipmOldTemperature_M2;

	ipmOldTemperature_M2 = ipmTemperature_M2;
	return ipmTemperature_M2;

}

static unsigned u32HandlerFreqPrescalerCirc = 1;
static unsigned u32HandlerFreqPrescalerCircCnt = 0;

static unsigned u32HandlerFreqPrescalerDrain = 1;
static unsigned u32HandlerFreqPrescalerDrainCnt = 0;



void GPIO0_IRQHandler(void)  //110us
{
	GPIO_GpioClearInterruptFlags(PIN_ZC_IN_GPIO, PIN_ZC_IN_PIN_MASK);

	appValveControl(&appDw);
}

void shortCircuitControl(void)
{
	if(scFuncEn)
	{
	    if(((PWMBase->FSTS) & (PWM_FSTS_FFLAG_MASK)) && !shortCircuitFlag)
	    {
	    	ipmFaultOutHandled_M1 = 1;
	    	shortCircuitFlag = 1;
	    }
	    else
	    {
	    	;
	    }

	    if(shortCircuitFlag)
	    	++scTimeCnt;
	    else
	    {
	    	;
	    }

	    if(scTimeCnt >= 75500)
	    {
	    	shortCircuitFlag = 0;
	    	scTimeCnt = 0;
	    	ipmFaultOutHandled_M1 = 0;
	    	PWMBase->FSTS = (PWMBase->FSTS & ~PWM_FSTS_FFLAG_MASK) | PWM_FSTS_FFLAG(0xF);

	    }
	    else
	    {
	    	;
	    }
	}

}

void shortCircuitControl_M2(void)
{
	if(scFuncEn)
	{
	    if(((PWMBase->FSTS) & (PWM_FSTS_FFLAG_MASK)) && !shortCircuitFlag_M2)
	    {
	    	ipmFaultOutHandled_M2 = 1;
	    	shortCircuitFlag_M2 = 1;
	    }
	    else
	    {
	    	;
	    }

	    if(shortCircuitFlag_M2)
	    	++scTimeCnt_M2;
	    else
	    {
	    	;
	    }

	    if(scTimeCnt_M2 >= 75500)
	    {
	    	shortCircuitFlag_M2 = 0;
	    	scTimeCnt_M2 = 0;
	    	ipmFaultOutHandled_M2 = 0;
	    	PWMBase->FSTS = (PWMBase->FSTS & ~PWM_FSTS_FFLAG_MASK) | PWM_FSTS_FFLAG(0xF);

	    }
	    else
	    {
	    	;
	    }
	}

}

volatile uint32_t uartPri, adcPri, gpioPri, ctimerPri, systickPri;
uint32_t start_, end, cycles;
float time_us;
volatile float maxCurrentU,maxCurrentV,maxOverLoadError, maxCurrentW = 0.0f;
volatile float maxVoltage = 0.0f;
volatile float minVoltage = 500.0f;
volatile uint8_t synOut=0;

void ADC0_IRQHandler(void) //68us
{
    /* Clear fault flags */
    //PWMBase->FSTS = (PWMBase->FSTS & ~PWM_FSTS_FFLAG_MASK) | PWM_FSTS_FFLAG(0xF);

    uartPri=NVIC_GetPriority(LPUART4_IRQn);
    adcPri=NVIC_GetPriority(ADC0_IRQn);
    gpioPri=NVIC_GetPriority(GPIO0_IRQn);
    ctimerPri=NVIC_GetPriority(CTIMER0_IRQn);
    systickPri=NVIC_GetPriority(SysTick_IRQn);




	if(appDw.v.activeState == CIRCULATION)
	{

		M1_AdcGetConvVal(&mtr_ad_data_M1);


		u32HandlerFreqPrescalerCircCnt++;
	    if(drvHighFreqHandlerMx != NULL &&
	    		u32HandlerFreqPrescalerCircCnt >= u32HandlerFreqPrescalerCirc)
	    {

		(*drvHighFreqHandlerMx)(0);
		u32HandlerFreqPrescalerCircCnt = 0;

	    }

	    shortCircuitControl();

	}



	if(appDw.v.activeState == DRAIN)
	{
		M2_AdcGetConvVal(&mtr_ad_data_M2);

		u32HandlerFreqPrescalerDrainCnt++;
	    if(drvHighFreqHandlerMx != NULL &&
	    		u32HandlerFreqPrescalerDrainCnt >= u32HandlerFreqPrescalerDrain)
	    {

		(*drvHighFreqHandlerMx)(1);
		u32HandlerFreqPrescalerDrainCnt = 0;

	    }

	    shortCircuitControl_M2();

	}

	    if(drvHighFreqHandlerApp != NULL)
	        drvHighFreqHandlerApp();

}


uint32_t workCnt = 0;
//Slow Loop Timer Interrupt (1kHz)
void ctimer_match0_callback(uint32_t flags) //35us
{

	if(appDw.v.activeState == CIRCULATION)
	{
		 if(drvLowFreqHandlerMx != NULL)
		 {
		     (*drvLowFreqHandlerMx)(0);
		 }
	}
	else if(appDw.v.activeState == DRAIN)
	{
		 if(drvLowFreqHandlerMx != NULL)
		 {
			 (*drvLowFreqHandlerMx)(1);

		 }

    }

    if(drvLowFreqHandlerApp != NULL)
        drvLowFreqHandlerApp();

    ++scFuncCnt;
    if(scFuncCnt > 100)
    {
    	scFuncCnt = 200;
    	scFuncEn = 1;
    }

    appDrainTimer(&appDw);

	ui32SysticIsrCnt++;
	/*
	FsRuntimeClockUpdate();
	FsFlashTestRt();
	WD_REFRESH;
	FsIsrTests();
	WD_REFRESH;
	*/
}

// 1ms Timer (SysTick)
volatile float uart_temperature = 0.0f;

void SysTick_Handler(void) //15us
{
	/*
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    start_ = DWT->CYCCNT;
*/
	appHeaterTimers(&appDw);

    if(!mcv_tx.txReady)
    	tickCounter++;

    if (tickCounter >= 100) {  //per 100ms
        tickCounter = 0;
        mcv_tx.txReady = true;
    }

    appTransTimer(&appDw);
    appDiverterControl(&appDw);
    if(appDw.v.activeState == CIRCULATION)
    {
    	ipmTemperature_M1 = drvGetTempIPM_M1();
    	uart_temperature = ipmTemperature_M1;

    }
    else if(appDw.v.activeState == DRAIN)
    {
    	ipmTemperature_M2 = drvGetTempIPM_M2();
    	uart_temperature = ipmTemperature_M2;


    }
    appFaultControl(&appDw);
/*
    end = DWT->CYCCNT;
    cycles = end - start_;
    time_us = (float)cycles / (PWM_CLOCK_HZ / 1e6);  // µs cinsinden hesapla
*/


}

volatile uint8_t temp=0;
volatile uint8_t preambleDetect=0;

void LPUART4_IRQHandler()  //4us
{

    uint32_t status = LPUART_GetStatusFlags(LPUART4);

    if (status & kLPUART_RxOverrunFlag)
        {
            LPUART_ClearStatusFlags(LPUART4, kLPUART_RxOverrunFlag);
            mcv_rx.rxIndex = 0;
            preambleDetect = 0;
        }

    if (status & kLPUART_RxDataRegFullFlag) {


    	temp = LPUART_ReadByte(LPUART4);

    	if(preambleDetect == 0) // preamble byte will be searched.
		{
    		if(temp == 0x55) // if the byte is preamble byte
			{
				mcv_rx.rxIndex = 0;
				rxBuffer[mcv_rx.rxIndex++] = temp;
				preambleDetect = 1;
			}
		}
		else // preamble detected. other bytes will be saved here.
		{
			if (mcv_rx.rxIndex < RX_BUFFER_SIZE)
			{
                rxBuffer[mcv_rx.rxIndex++] = temp;
                appDw.v.appVarDwUART.faultCntr = 0;

			}

			if(mcv_rx.rxIndex >= (RX_BUFFER_SIZE))
			{
				mcv_rx.rxComplete = true;
				preambleDetect = 0;
			}
		}
    }

    if ((status & kLPUART_TxDataRegEmptyFlag)
    		&& (LPUART_GetEnabledInterrupts(LPUART4) & kLPUART_TxDataRegEmptyInterruptEnable))
        {
            if (txIndex < TX_BUFFER_SIZE)
            {
                LPUART_WriteByte(LPUART4, txBuffer[txIndex++]);
            }
            else
            {
                LPUART_DisableInterrupts(LPUART4, kLPUART_TxDataRegEmptyInterruptEnable);
            }
        }

}
