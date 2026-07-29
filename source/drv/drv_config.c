#include "drv_config.h"
#include "drv.h"

/* Array of function pointers for callback for each channel */
ctimer_callback_t ctimer_callback_table[] = {
    ctimer_match0_callback,NULL , NULL, NULL, NULL, NULL, NULL, NULL};

ctimer_match_config_t matchConfig0;
ctimer_config_t config;
ctimer_status_flags_t ctimer_status_flags;



void InitGpioInt(void)
{
    /* Interrupt configuration on GPIO0_16 (pin 54): Interrupt on either edge */
    GPIO_SetPinInterruptConfig(PIN_ZC_IN_GPIO, PIN_ZC_IN_PIN, kGPIO_InterruptEitherEdge);
    NVIC_SetPriority(GPIO0_IRQn, 4U);
    NVIC_EnableIRQ(GPIO0_IRQn);

}

void SlowLoopInt()
{

    /* Init hardware*/
    CLOCK_SetClockDiv(kCLOCK_DivCTIMER2, 1u);
    CLOCK_AttachClk(kFRO_HF_to_CTIMER2);

    CTIMER_GetDefaultConfig(&config);

    CTIMER_Init(CTIMER, &config);

    /* Configuration 0 */
    matchConfig0.enableCounterReset = true;
    matchConfig0.enableCounterStop  = false;
    matchConfig0.matchValue         = CTIMER_CLK_FREQ / FREQ_SLOWLOOP;
    matchConfig0.outControl         = kCTIMER_Output_Toggle;
    matchConfig0.outPinInitState    = false;
    matchConfig0.enableInterrupt    = true;

    /* Create different ctimer_callback_table array for different CTimer instance. */
    CTIMER_RegisterCallBack(CTIMER, &ctimer_callback_table[0], kCTIMER_MultipleCallback);

    CTIMER_SetupMatch(CTIMER, CTIMER_MAT0_OUT, &matchConfig0);
    CTIMER_StartTimer(CTIMER);

    NVIC_SetPriority(CTIMER0_IRQn, 1U);
    NVIC_EnableIRQ(CTIMER0_IRQn);
}

void InitFlexPWM()
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
    PWMBase->SM[0].VAL4 = PWM_VAL4_VAL4((uint16_t)((-PWM_MODULO / 2)+806));//340
    PWMBase->SM[1].VAL4 = PWM_VAL4_VAL4((uint16_t)((-PWM_MODULO / 2)+806));//340
    PWMBase->SM[2].VAL4 = PWM_VAL4_VAL4((uint16_t)((-PWM_MODULO / 2)+806));//340

    PWMBase->SM[0].VAL5 = PWM_VAL5_VAL5((uint16_t)(0));
    PWMBase->SM[1].VAL5 = PWM_VAL5_VAL5((uint16_t)(0));
    PWMBase->SM[2].VAL5 = PWM_VAL5_VAL5((uint16_t)(0));

    /* PWM sub-module 0 trigger on VAL4 enabled for ADC synchronization */
    PWMBase->SM[0].TCTRL |= PWM_TCTRL_OUT_TRIG_EN(1 << 4) | PWM_TCTRL_TRGFRQ(1);
    PWMBase->SM[1].TCTRL |= PWM_TCTRL_OUT_TRIG_EN(1 << 4) | PWM_TCTRL_TRGFRQ(1);
    PWMBase->SM[2].TCTRL |= PWM_TCTRL_OUT_TRIG_EN(1 << 4) | PWM_TCTRL_TRGFRQ(1);

        /* Set dead-time register */
    PWMBase->SM[0].DTCNT0 = PWM_DTCNT0_DTCNT0(PWM_DEADTIME);
    PWMBase->SM[1].DTCNT0 = PWM_DTCNT0_DTCNT0(PWM_DEADTIME);
    PWMBase->SM[2].DTCNT0 = PWM_DTCNT0_DTCNT0(PWM_DEADTIME);
    PWMBase->SM[0].DTCNT1 = PWM_DTCNT1_DTCNT1(PWM_DEADTIME);
    PWMBase->SM[1].DTCNT1 = PWM_DTCNT1_DTCNT1(PWM_DEADTIME);
    PWMBase->SM[2].DTCNT1 = PWM_DTCNT1_DTCNT1(PWM_DEADTIME);

    /* Channels A and B disabled when fault 0,1,2 occurs */
    /* CMP0,1 fault to PWM is enabled there is 0x3 value */
    /* CMP2 fault to PWM is disabled at the moment, there will  be 0x7 value */


    PWMBase->SM[0].DISMAP[0] = ((PWMBase->SM[0].DISMAP[0] & ~PWM_DISMAP_DIS0A_MASK) | PWM_DISMAP_DIS0A(0x7));
    PWMBase->SM[1].DISMAP[0] = ((PWMBase->SM[1].DISMAP[0] & ~PWM_DISMAP_DIS0A_MASK) | PWM_DISMAP_DIS0A(0x7));
    PWMBase->SM[2].DISMAP[0] = ((PWMBase->SM[2].DISMAP[0] & ~PWM_DISMAP_DIS0A_MASK) | PWM_DISMAP_DIS0A(0x7));
    PWMBase->SM[0].DISMAP[0] = ((PWMBase->SM[0].DISMAP[0] & ~PWM_DISMAP_DIS0B_MASK) | PWM_DISMAP_DIS0B(0x7));
    PWMBase->SM[1].DISMAP[0] = ((PWMBase->SM[1].DISMAP[0] & ~PWM_DISMAP_DIS0B_MASK) | PWM_DISMAP_DIS0B(0x7));
    PWMBase->SM[2].DISMAP[0] = ((PWMBase->SM[2].DISMAP[0] & ~PWM_DISMAP_DIS0B_MASK) | PWM_DISMAP_DIS0B(0x7));

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
    PWMBase->FSTS = (PWMBase->FSTS & ~PWM_FSTS_FFULL_MASK) | PWM_FSTS_FFULL(0x7);

    /* PWM fault filter - 5 Fast peripheral clocks sample rate, 5 agreeing
       samples to activate */
    PWMBase->FFILT = (PWMBase->FFILT & ~PWM_FFILT_FILT_PER_MASK) | PWM_FFILT_FILT_PER(0);
    PWMBase->FFILT = (PWMBase->FFILT & ~PWM_FFILT_FILT_CNT_MASK) | PWM_FFILT_FILT_CNT(0);

    /* Start PWMs (set load OK flags and run) */
    PWMBase->MCTRL = (PWMBase->MCTRL & ~PWM_MCTRL_CLDOK_MASK) | PWM_MCTRL_CLDOK(0xF);
    PWMBase->MCTRL = (PWMBase->MCTRL & ~PWM_MCTRL_LDOK_MASK) | PWM_MCTRL_LDOK(0xF);
    PWMBase->MCTRL = (PWMBase->MCTRL & ~PWM_MCTRL_RUN_MASK) | PWM_MCTRL_RUN(0x0);
}

void InitInputmux()
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
	/* PWM0_SM0_OUT_TRIG0 is selected as trigger input for ADC3 */
	INPUTMUX0->ADC3_TRIG[0] = INPUTMUX_ADC3_TRIGM_ADC3_TRIG_TRIGIN(0b010010);

	/*  Cmp0Out connect to FlexPwm0Fault 0 */
	INPUTMUX_AttachSignal(INPUTMUX0, 0U, kINPUTMUX_Cmp0OutToFlexPwm0Fault);
	/*  Cmp1Out connect to FlexPwm0Fault 1 */
	INPUTMUX_AttachSignal(INPUTMUX0, 1U, kINPUTMUX_Cmp1OutToFlexPwm0Fault);
	/*  Cmp2Out connect to FlexPwm0Fault 2 */
	INPUTMUX_AttachSignal(INPUTMUX0, 2U, kINPUTMUX_Cmp2OutToFlexPwm0Fault);
}

void InitADC0()
{
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
    lpadcConfig.conversionAverageMode = kLPADC_ConversionAverage256;

    LPADC_Init(ADC0, &lpadcConfig);

    LPADC_DoOffsetCalibration(ADC0);
    LPADC_DoAutoCalibration(ADC0);

    LPADC_GetDefaultConvCommandConfig(&lpadcCommandConfig);
    lpadcCommandConfig.sampleChannelMode = kLPADC_SampleChannelSingleEndSideA;
    lpadcCommandConfig.conversionResolutionMode = kLPADC_ConversionResolutionStandard;
    lpadcCommandConfig.sampleTimeMode = kLPADC_SampleTimeADCK35;

    /* Init ADC channels. */
    lpadcCommandConfig.channelNumber = ADC0CH_CURR_U;		// or ADC0CH_CURR_B in channel mapping function according to FOC sector
    lpadcCommandConfig.chainedNextCommandNumber = 2U;
    LPADC_SetConvCommandConfig( ADC0, 1U, &lpadcCommandConfig );

    lpadcCommandConfig.channelNumber = ADC0CH_DIVERTER;
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

void InitADC1()
{
	lpadc_conv_trigger_config_t lpadcTriggerConfig;
    lpadc_conv_command_config_t lpadcCommandConfig;
    lpadc_config_t lpadcConfig;

    /* Init the lpadcConfig struct */
    LPADC_GetDefaultConfig(&lpadcConfig);
    lpadcConfig.enableAnalogPreliminary = true;
    lpadcConfig.referenceVoltageSource = kLPADC_ReferenceVoltageAlt3;
    lpadcConfig.conversionAverageMode = kLPADC_ConversionAverage256;

    LPADC_Init(ADC1, &lpadcConfig);

    LPADC_DoOffsetCalibration(ADC1);
    LPADC_DoAutoCalibration(ADC1);

    LPADC_GetDefaultConvCommandConfig(&lpadcCommandConfig);
    lpadcCommandConfig.sampleChannelMode = kLPADC_SampleChannelSingleEndSideA;
    lpadcCommandConfig.conversionResolutionMode = kLPADC_ConversionResolutionStandard;
    lpadcCommandConfig.sampleTimeMode = kLPADC_SampleTimeADCK35;

    /* Init ADC channels */
    lpadcCommandConfig.channelNumber = ADC1CH_CURR_V;// or ADC1CH_CURR_W channel mapping function according to FOC sector
    lpadcCommandConfig.chainedNextCommandNumber = 2U;
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

void InitADC2()
{
    lpadc_conv_trigger_config_t lpadcTriggerConfig;
    lpadc_conv_command_config_t lpadcCommandConfig;
    lpadc_config_t lpadcConfig;

    /* Init the lpadcConfig struct */
    LPADC_GetDefaultConfig(&lpadcConfig);
    lpadcConfig.enableAnalogPreliminary = true;
    lpadcConfig.referenceVoltageSource = kLPADC_ReferenceVoltageAlt3;
    lpadcConfig.conversionAverageMode = kLPADC_ConversionAverage256;

    LPADC_Init(ADC2, &lpadcConfig);

    LPADC_DoOffsetCalibration(ADC2);
    LPADC_DoAutoCalibration(ADC2);

    LPADC_GetDefaultConvCommandConfig(&lpadcCommandConfig);
    lpadcCommandConfig.sampleChannelMode = kLPADC_SampleChannelSingleEndSideA;
    lpadcCommandConfig.conversionResolutionMode = kLPADC_ConversionResolutionStandard;
    lpadcCommandConfig.sampleTimeMode = kLPADC_SampleTimeADCK35;

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

void InitADC3()
{
    lpadc_conv_trigger_config_t lpadcTriggerConfig;
    lpadc_conv_command_config_t lpadcCommandConfig;
    lpadc_config_t lpadcConfig;

    LPADC_GetDefaultConfig(&lpadcConfig);
    lpadcConfig.enableAnalogPreliminary = true;
    lpadcConfig.referenceVoltageSource = kLPADC_ReferenceVoltageAlt3;
    lpadcConfig.conversionAverageMode = kLPADC_ConversionAverage256;

    LPADC_Init(ADC3, &lpadcConfig);

    LPADC_DoOffsetCalibration(ADC3);
    LPADC_DoAutoCalibration(ADC3);

    LPADC_GetDefaultConvCommandConfig(&lpadcCommandConfig);
    lpadcCommandConfig.sampleChannelMode = kLPADC_SampleChannelSingleEndSideA;
    lpadcCommandConfig.conversionResolutionMode = kLPADC_ConversionResolutionStandard;
    lpadcCommandConfig.sampleTimeMode = kLPADC_SampleTimeADCK35;


    lpadcCommandConfig.channelNumber = ADC3CH_SYNCPIN;
    lpadcCommandConfig.chainedNextCommandNumber = 0;
    LPADC_SetConvCommandConfig( ADC3, 1U, &lpadcCommandConfig );


    LPADC_GetDefaultConvTriggerConfig(&lpadcTriggerConfig);
    lpadcTriggerConfig.targetCommandId = 1U;
    lpadcTriggerConfig.enableHardwareTrigger = true;
    LPADC_SetConvTriggerConfig(ADC3, 0U, &lpadcTriggerConfig);

}


void InitComps()
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
    CLOCK_AttachClk(kFRO_LF_DIV_to_CMP0);

    CLOCK_SetClockDiv(kCLOCK_DivCMP1_FUNC, 1u);
    CLOCK_AttachClk(kFRO_LF_DIV_to_CMP1);

    CLOCK_SetClockDiv(kCLOCK_DivCMP2_FUNC, 1u);
    CLOCK_AttachClk(kFRO_LF_DIV_to_CMP2);


	 lpcmp_config_t mLpcmpConfigStruct;
	 lpcmp_dac_config_t mLpcmpDacConfigStruct;

	 /*
	  *   mLpcmpConfigStruct->enableStopMode      = false;
	  *   mLpcmpConfigStruct->enableOutputPin     = false;
	  *   mLpcmpConfigStruct->useUnfilteredOutput = false;
	  *   mLpcmpConfigStruct->enableInvertOutput  = false;
	  *   mLpcmpConfigStruct->hysteresisMode      = kLPCMP_HysteresisLevel0;
	  *   mLpcmpConfigStruct->powerMode           = kLPCMP_LowSpeedPowerMode;
	  *   mLpcmpConfigStruct->functionalSourceClock = kLPCMP_FunctionalClockSource0;
	  */
	 LPCMP_GetDefaultConfig(&mLpcmpConfigStruct);
	 mLpcmpConfigStruct.enableOutputPin     = true;
	 mLpcmpConfigStruct.powerMode           = kLPCMP_HighSpeedPowerMode;
	 mLpcmpConfigStruct.useUnfilteredOutput = true;
	 mLpcmpConfigStruct.enableInvertOutput  = true;

	 /* Init the LPCMP module. */
	 LPCMP_Init(CMP0, &mLpcmpConfigStruct);
	 LPCMP_Init(CMP1, &mLpcmpConfigStruct);
	 LPCMP_Init(CMP2, &mLpcmpConfigStruct);

	 /* Configure the internal DAC to output half of reference voltage. */
	 mLpcmpDacConfigStruct.enableLowPowerMode     = false;
	 mLpcmpDacConfigStruct.referenceVoltageSource = kLPCMP_VrefSourceVin1;
	 /* Overcurrent threshold, max value 255 */
	 mLpcmpDacConfigStruct.DACValue = (20u);

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

void InitDac()
{
    dac_config_t dacConfigStruct;

	SPC_EnableActiveModeAnalogModules(SPC0, (kSPC_controlDac0));
	/* Attach peripheral clock */
	CLOCK_SetClockDiv(kCLOCK_DivDAC0, 1u);
	CLOCK_AttachClk(kFRO_LF_DIV_to_DAC0);
	/* Configure the DAC. */
	DAC_GetDefaultConfig(&dacConfigStruct);
	dacConfigStruct.referenceVoltageSource = kDAC_ReferenceVoltageSourceAlt1;
	DAC_Init(DAC0, &dacConfigStruct);
	DAC_Enable(DAC0, true); /* Enable the logic and output. */

	/* EXAMPLE OF SET DAC VALUE */
	DAC_SetData(DAC0, 200);

}

void InitOpamps()
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
	RESET_ReleasePeripheralReset(kOPAMP0_RST_SHIFT_RSTn);
	RESET_ReleasePeripheralReset(kOPAMP1_RST_SHIFT_RSTn);
	RESET_ReleasePeripheralReset(kOPAMP2_RST_SHIFT_RSTn);

	// Configure and turn on the OPAMP
	OPAMP0->OPAMP_CTRL =
			OPAMP_OPAMP_CTRL_OPA_EN(1U)         |   /* Enable the opamp */
	        OPAMP_OPAMP_CTRL_OPA_BC_SEL(0b00)	|	/* Bias current config selection. Default value. Keep power consumption constant */
			OPAMP_OPAMP_CTRL_OPA_CC_SEL(0b00);	   	/* Compensation capacitor config selection. Fit 2X gains */

	// Configure and turn on the OPAMP
	OPAMP1->OPAMP_CTRL =
			OPAMP_OPAMP_CTRL_OPA_EN(1U)         |   /* Enable the opamp */
	        OPAMP_OPAMP_CTRL_OPA_BC_SEL(0b00)	|	/* Bias current config selection. Default value. Keep power consumption constant */
			OPAMP_OPAMP_CTRL_OPA_CC_SEL(0b00);   	/* Compensation capacitor config selection. Fit 2X gains */

	// Configure and turn on the OPAMP
	OPAMP2->OPAMP_CTRL =
			OPAMP_OPAMP_CTRL_OPA_EN(1U)         |   /* Enable the opamp */
	        OPAMP_OPAMP_CTRL_OPA_BC_SEL(0b00)	|	/* Bias current config selection. Default value. Keep power consumption constant */
			OPAMP_OPAMP_CTRL_OPA_CC_SEL(0b00);   	/* Compensation capacitor config selection. Fit 2X gains */
}



void InitUART()
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
    config.baudRate_Bps = 9600u;
    config.enableTx     = true;
    config.enableRx     = true;

    LPUART_Init(LPUART4, &config, 12000000U);
    LPUART_EnableInterrupts(LPUART4, kLPUART_RxDataRegFullInterruptEnable);
    NVIC_SetPriority(LPUART4_IRQn,2U);
    NVIC_EnableIRQ(LPUART4_IRQn);

}

void UpdatePWM(int16_t i16PhA, int16_t i16PhB, int16_t i16PhC, PWM_Type *pPWMBase)
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

