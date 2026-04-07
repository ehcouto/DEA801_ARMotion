/**
*
*
*/

#ifndef BRD_H
#define BRD_H

#include "env.h"
#if defined(MKV42F16) || defined(RX66T_SONIC) || defined(RX24T) || defined(RX13T) || defined(RX66T_AIRCON) || defined(GD32F303) || defined(MCXA166_DW)
#include "drv.h"
#include <stdbool.h>
#include <stdint.h>
#elif defined(QT_CPP_SIMULATOR)

#elif defined(VISUAL_STUDIO)

#else
#error Process environment should be defined;
#endif


extern float currentUb;
extern float currentVb;
extern float currentVa;
extern float currentWa;

extern float currentUb_M2;
extern float currentVb_M2;
extern float currentVa_M2;
extern float currentWa_M2;

extern uint16_t adcOffsetWa;
extern uint16_t adcOffsetVa;
extern uint16_t adcOffsetVb;
extern uint16_t adcOffsetUb;

extern uint16_t adcOffsetWa_M2;
extern uint16_t adcOffsetVa_M2;
extern uint16_t adcOffsetVb_M2;
extern uint16_t adcOffsetUb_M2;

typedef struct{
    float sampledCurrentU;
    float sampledCurrentV;
    float sampledCurrentW;
} brd_t;


void brdInit(void (*_McFuncFastLoop)(int32_t MxIndex), void (*_McFuncSlowLoop)(int32_t MxIndex),
             void (*_AppFuncFastLoop)(void), void (*_AppFuncSlowLoop)(void),
             void (*_AppCommunicationHandler)(void));

void brdTurnOnInrushRelay(void);
void brdTurnOffInrushRelay(void);

#ifdef MC_ENABLE_FIRST_MOTOR

void brdPwmEnablePhaseU_M1(void);
void brdPwmEnablePhaseV_M1(void);
void brdPwmEnablePhaseW_M1(void);
void brdPwmEnablePhaseUVW_M1(void);

void brdMapAdcChannels_M1(uint8_t sec);

void brdSampleCurrentsUVW_M1(uint8_t sec);
float brdGetPhaseCurrentU_M1(void);
float brdGetPhaseCurrentV_M1(void);
float brdGetPhaseCurrentW_M1(void);

float brdGetDcBusVoltage_M1(void);

void brdSetPwmDuties_M1(float dutyU, float dutyV, float dutyW);

bool brdCurrentCalibrationInit_M1(void);
bool brdCurrentCalibrationHandler_M1(void);

float brdGetIpmTemperature_M1(void);
bool brdGetIpmFaultOutState_M1(void);
void brdResetIpmFaultOutState_M1(void);

void brdPwmEnable_M1(void);
void brdPwmDisable_M1(void);

void brdShortCircuitBottomTransistors_M1(void);

bool brdGetEncoderData_M1(float *_pos, float *_sp);

bool brdGetDCBusOpenStatus(void);



#endif

#ifdef MC_ENABLE_SECOND_MOTOR

void brdPwmEnablePhaseU_M2(void);
void brdPwmEnablePhaseV_M2(void);
void brdPwmEnablePhaseW_M2(void);
void brdPwmEnablePhaseUVW_M2(void);

void brdMapAdcChannels_M2(uint8_t sec);

void brdSampleCurrentsUVW_M2(uint8_t sec);
float brdGetPhaseCurrentU_M2(void);
float brdGetPhaseCurrentV_M2(void);
float brdGetPhaseCurrentW_M2(void);

float brdGetDcBusVoltage_M2(void);

void brdSetPwmDuties_M2(float dutyU, float dutyV, float dutyW);

bool brdCurrentCalibrationInit_M2(void);
bool brdCurrentCalibrationHandler_M2(void);

float brdGetIpmTemperature_M2(void);
bool brdGetIpmFaultOutState_M2(void);
void brdResetIpmFaultOutState_M2(void);

void brdPwmEnable_M2(void);
void brdPwmDisable_M2(void);

void brdShortCircuitBottomTransistors_M2(void);

bool brdGetEncoderData_M2(float *_pos, float *_sp);

extern int drvGetAdcWa_M2(void);
extern int drvGetAdcUb_M2(void);

#endif

void brdHandlerML(void);

#ifdef RX66T_AIRCON
float brdSampleCompressorTemp(void);
float brdSampleCondenserTemp(void);
float brdSampleAmbientTemp(void);
float brdSampleCompressorIpmTemp(void);
float brdSampleFanIpmTemp(void);

void brdPcbLedOn(void);
void brdPcbLedOff(void);
void brdPcbLedToggle(void);
void brdRead4Wayon(void);
void brdRead4Wayoff(void);
	
void brdStepMotorAset(void);
void brdStepMotorBset(void);
void brdStepMotorCset(void);
void brdStepMotorDset(void);
void brdStepMotorAclear(void);
void brdStepMotorBclear(void);
void brdStepMotorCclear(void);
void brdStepMotorDclear(void);

void brdHighFanSet(void);
void brdHighFanClear(void);
void brdMidFanSet(void);
void brdMidFanClear(void);

unsigned char brdCompOvlPort(void);
unsigned char brdIpmProtectPort(void);
unsigned char brdDcFanFaultPort(void);
unsigned char brdPfcProtectPort(void);

void brdIpmBrkFlgClear(void);
void brdDcFanBrkFlgClear(void);
void brdPfcBrkFlgClear(void);
#endif

#ifdef MCXA166_DW

void brdPumpSet(void);
void brdPumpReset(void);
void brdDispenserSet(void);
void brdDispenserReset(void);
void brdRegenSet(void);
void brdRegenReset(void);
void brdH2OSet(void);
void brdH2OReset(void);
void brdDiverterSet(void);
void brdDiverterReset(void);
float brdGetTempIPM_M1(void);
float brdGetTempIPM_M2(void);
void brdRelayHeaterSet(void);
void brdRelayHeaterReset(void);
void brdSwitchHeaterSet(void);
void brdSwitchHeaterReset(void);


#endif

void brdWatchdogResetHandlerFL(void);
void brdResetWdg(void);

void brdEnableInterrupts(void);
void brdDisableInterrupts(void);


#endif
