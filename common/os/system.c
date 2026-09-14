/**
*
*
*/

#include "system.h"

os_t os;

/***************** Defined motor control objects - start **********************/
mcInit_t mcInit;
mcMpvInit_t mcMpvInit1;
mcMpvInit_t mcMpvInit2;
/***************** Defined motor control objects - stop ***********************/


/***************** Operating system function declerations - start *************/
void osInitSys(void);
void osHandlerML(void);

static void osInitBrd(void);
static void osInitMc(void);
static void osInitFs(void);
static void osInitApp(void);
#ifdef ENABLE_AIRCON_APPLICATION
static void osInitPFC(void);
#endif
/***************** Operating system function declerations - stop **************/


/******************* Operating system function definitions - start ************/
void osInitSys(void)
{
    brdDisableInterrupts();

    osInitBrd();
    osInitMc();
    osInitFs();
    osInitApp();
#ifdef ENABLE_AIRCON_APPLICATION
    osInitPFC();
#endif
    brdEnableInterrupts();
}

static void osInitBrd(void)
{
    brdInit(mcMxHandlerFL, mcMxHandlerSL, appHandlerFL, appHandlerSL, appHandlerCom);
}

static void osInitMc(void)
{
    mcInit.mcDrvTurnOnInrushRelay = brdTurnOnInrushRelay;
    mcInitMotorControl(&mcInit);
    
#ifdef MC_ENABLE_FIRST_MOTOR
    mcMpvInit1.freqFL = MOTOR1_FAST_LOOP_FREQUENCY;
    mcMpvInit1.freqSL = MOTOR1_SLOW_LOOP_FREQUENCY;
    mcMpvInit1.mcBrdPwmEnable = brdPwmEnable_M1;
    mcMpvInit1.mcBrdPwmDisable = brdPwmDisable_M1;
    mcMpvInit1.mcBrdPwmEnablePhaseU = brdPwmEnablePhaseU_M1;
    mcMpvInit1.mcBrdPwmEnablePhaseV = brdPwmEnablePhaseV_M1;
    mcMpvInit1.mcBrdPwmEnablePhaseW = brdPwmEnablePhaseW_M1;
    mcMpvInit1.mcBrdPwmEnablePhaseUVW = brdPwmEnablePhaseUVW_M1;
    mcMpvInit1.mcBrdMapAdcChannels = brdMapAdcChannels_M1;
    mcMpvInit1.mcBrdSampleCurrentsUVW = brdSampleCurrentsUVW_M1;
    mcMpvInit1.mcBrdGetPhaseCurrentU = brdGetPhaseCurrentU_M1;
    mcMpvInit1.mcBrdGetPhaseCurrentV = brdGetPhaseCurrentV_M1;
    mcMpvInit1.mcBrdGetPhaseCurrentW = brdGetPhaseCurrentW_M1;
    mcMpvInit1.mcBrdCurrentCalibrationInit = brdCurrentCalibrationInit_M1;
    mcMpvInit1.mcBrdCurrentCalibrationHandler = brdCurrentCalibrationHandler_M1;
    mcMpvInit1.mcBrdGetDcBusVoltage = brdGetDcBusVoltage_M1;
    mcMpvInit1.mcBrdGetIpmTemperature = brdGetIpmTemperature_M1;
    mcMpvInit1.mcBrdGetIpmFaultOutState = brdGetIpmFaultOutState_M1;
    mcMpvInit1.mcBrdResetIpmFaultOutState = brdResetIpmFaultOutState_M1;
    mcMpvInit1.mcBrdSetPwmDuties = brdSetPwmDuties_M1;
    mcMpvInit1.mcBrdGetEncoderData = brdGetEncoderData_M1;
    mcMpvInit1.mcBrdGetDCBusOpenStatus = brdGetDCBusOpenStatus;
    mcMpvInit1.mcBrdShortCircBotTransistors = brdShortCircuitBottomTransistors_M1;
    mcAddMotor(&mcMpvInit1);
#endif
    
#ifdef MC_ENABLE_SECOND_MOTOR
    mcMpvInit2.freqFL = MOTOR2_FAST_LOOP_FREQUENCY;
    mcMpvInit2.freqSL = MOTOR2_SLOW_LOOP_FREQUENCY;
    mcMpvInit2.mcBrdPwmEnable = brdPwmEnable_M2;
    mcMpvInit2.mcBrdPwmDisable = brdPwmDisable_M2;
    mcMpvInit2.mcBrdPwmEnablePhaseU = brdPwmEnablePhaseU_M2;
    mcMpvInit2.mcBrdPwmEnablePhaseV = brdPwmEnablePhaseV_M2;
    mcMpvInit2.mcBrdPwmEnablePhaseW = brdPwmEnablePhaseW_M2;
    mcMpvInit2.mcBrdPwmEnablePhaseUVW = brdPwmEnablePhaseUVW_M2;
    mcMpvInit2.mcBrdMapAdcChannels = brdMapAdcChannels_M2;
    mcMpvInit2.mcBrdSampleCurrentsUVW = brdSampleCurrentsUVW_M2;
    mcMpvInit2.mcBrdGetPhaseCurrentU = brdGetPhaseCurrentU_M2;
    mcMpvInit2.mcBrdGetPhaseCurrentV = brdGetPhaseCurrentV_M2;
    mcMpvInit2.mcBrdGetPhaseCurrentW = brdGetPhaseCurrentW_M2;
    mcMpvInit2.mcBrdCurrentCalibrationInit = brdCurrentCalibrationInit_M2;
    mcMpvInit2.mcBrdCurrentCalibrationHandler = brdCurrentCalibrationHandler_M2;
    mcMpvInit2.mcBrdGetDcBusVoltage = brdGetDcBusVoltage_M2;
    mcMpvInit2.mcBrdGetIpmTemperature = brdGetIpmTemperature_M2;
    mcMpvInit2.mcBrdGetIpmFaultOutState = brdGetIpmFaultOutState_M2;
    mcMpvInit2.mcBrdResetIpmFaultOutState = brdResetIpmFaultOutState_M2;
    mcMpvInit2.mcBrdSetPwmDuties = brdSetPwmDuties_M2;
    mcMpvInit2.mcBrdGetEncoderData = brdGetEncoderData_M2;
    mcMpvInit2.mcBrdGetDCBusOpenStatus = brdGetDCBusOpenStatus;
    mcMpvInit2.mcBrdShortCircBotTransistors = brdShortCircuitBottomTransistors_M2;
    mcAddMotor(&mcMpvInit2);
#endif
}

static void osInitFs(void)
{

}

static void osInitApp(void)
{
    appInitApp();
}
#ifdef ENABLE_AIRCON_APPLICATION
static void osInitPFC(void)
{
	pfc_Init();
}
#endif
void osHandlerML(void)
{
    os.counterML++;

    mcHandlerML();
    appHandlerML();
    brdHandlerML();
#ifdef ENABLE_AIRCON_APPLICATION
    PFCHandler();	// 8.78us
#endif
}
/******************* Operating system function definitions - stop *************/
