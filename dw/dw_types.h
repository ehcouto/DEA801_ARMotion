#include "app_config.h"

#ifdef ENABLE_DISHWASHER_APPLICATION

#ifndef SOURCE_APP_DW_TYPES_H_
#define SOURCE_APP_DW_TYPES_H_

#define	APP_NULL	((void*)0)


typedef signed char app_int8_t;
typedef unsigned char app_uint8_t;
typedef short app_int16_t;
typedef unsigned short app_uint16_t;
typedef int app_int32_t;
typedef unsigned int app_uint32_t;
typedef float app_float_t;
typedef _Bool app_bool_t;
typedef void app_void_t;

typedef struct{
	volatile app_float_t current2;
	volatile app_float_t sum2;
	volatile app_uint32_t numSample;
	volatile app_uint32_t rmsCounter;
} rmsCalcPar_t;

typedef struct
{
	volatile app_uint8_t preambleByteRx;
	volatile app_uint8_t commandDiverter;
	volatile app_uint8_t pilotDiverter;
	volatile app_uint8_t pilotValves;
	volatile app_uint16_t spRefM1Rx;
	volatile app_uint16_t spRefM2Rx;
	volatile app_uint8_t selectComponent;
	volatile app_uint8_t heaterFirstCfg;
	volatile app_uint8_t rsvByte1;
	volatile app_uint8_t rsvByte2;
	volatile app_uint8_t rsvByte3;
	volatile app_uint8_t rsvByte4;
	volatile app_uint8_t rsvByte5;
	volatile app_uint8_t rsvByte6;
	volatile app_uint8_t rsvByte7;
	volatile app_uint8_t rsvByte8;
	volatile app_uint8_t receivedCRCRx;
	volatile app_uint8_t CRCRx;
	volatile app_uint8_t rxIndex;
	volatile app_bool_t rxComplete;
	volatile app_uint16_t spRefCirc;
	volatile app_uint16_t spRefDrain;
	volatile app_uint8_t pilotValvesRx;
	volatile app_uint8_t commandAppDiverter;
}mcv_rx_uart;


typedef struct
{
	volatile app_uint8_t preambleByteTx;
	volatile app_uint32_t faultTx;
	volatile app_uint8_t diverterFbTx;
	volatile app_uint16_t spEstM1Tx;
	volatile app_uint16_t spEstM2Tx;
	volatile app_uint8_t CRCTx;
	volatile app_bool_t txReady;
	volatile app_uint16_t torqueVal[4];
	volatile app_uint16_t packedVersion;
	volatile app_uint8_t resultHHPTest;

}mcv_tx_uart;

typedef enum
{
	DIVERTER_3WAY = 0x0U,
	DIVERTER_2WAY = 0x1U,
	DIVERTERLESS  = 0x2U,
	DIVERTER_NOT_SELECT = 0x3U,
}diverterType_t;

typedef enum
{
	CIRC_W20007335 = 0x0U,
	CIRC_W11715301 = 0x1U,
	CIRC_W20016965 = 0x2U,
	CIRC_W11652801 = 0x3U,
}circType_t;

typedef enum
{
	DRAIN_W11402566 = 0X0U,
	DRAIN_W11186148 = 0X1U,
	DRAIN_W11377410 = 0x2U,
}drainType_t;

typedef struct{
	app_void_t 	(*appDwPumpSet)(void);
	app_void_t 	(*appDwPumpReset)(void);
	app_void_t 	(*appDwDispenserSet)(void);
	app_void_t 	(*appDwDispenserReset)(void);
	app_void_t 	(*appDwRegenSet)(void);
	app_void_t 	(*appDwRegenReset)(void);
	app_void_t 	(*appDwH2OSet)(void);
	app_void_t 	(*appDwH2OReset)(void);
	app_void_t 	(*appDwDiverterSet)(void);
	app_void_t 	(*appDwDiverterReset)(void);
	app_float_t (*appDwGetTempIPM_M1)(void);
	app_float_t (*appDwGetTempIPM_M2)(void);
	app_void_t 	(*appDwRelayHeaterSet)(void);
	app_void_t 	(*appDwRelayHeaterReset)(void);
	app_void_t 	(*appDwSwitchHeaterSet)(void);
	app_void_t 	(*appDwSwitchHeaterReset)(void);
	app_void_t  (*appPwmDisable_M1)(void);
	app_void_t  (*appPwmDisable_M2)(void);


} appDwFp_t;	// function pointers for Dw application

typedef struct{
	volatile app_uint32_t posDivCnt;
	volatile app_uint32_t divStopCnt;
	volatile app_uint32_t divRotCnt;
	volatile app_uint32_t divRotTimer;
	volatile app_uint32_t faultCntr;
	volatile app_uint8_t divPosReal;
	volatile app_uint8_t divPosRealTx;
	volatile app_bool_t divReadyFlag;
	volatile app_bool_t diverCommand;
	volatile app_bool_t diverInitFlag;
	volatile app_bool_t flag;
	volatile app_uint8_t diverterType;
	volatile app_uint8_t diverterOldType;
	volatile app_bool_t firstFlag2W;
	volatile app_bool_t secondFlag2W;
	volatile app_uint8_t divDesiredType;
	volatile app_bool_t divSyncFlag;
	volatile app_bool_t adcHighPrev;
} appVarDwDiver_t;

typedef struct{
	volatile app_uint32_t faultCntrMax;
	volatile app_uint16_t divAdcLow;
	volatile app_uint16_t div1TurnTime;
	volatile app_uint16_t divHighMax;
	volatile app_uint16_t divHighMin;
	volatile app_uint16_t divLowMax;
	volatile app_uint16_t divLowMin;
	volatile app_uint16_t divPos0HighMax;
	volatile app_uint16_t divPos0HighMin;
	volatile app_uint16_t divPos0LowMax;
	volatile app_uint16_t divPos0LowMin;
	volatile app_uint8_t divPosNum;
	volatile app_bool_t enabled;
	volatile app_uint16_t div2WPos0HighMax;
	volatile app_uint16_t div2WPos0HighMin;
	volatile app_uint16_t div2WPos0LowMax;
	volatile app_uint16_t div2WPos0LowMin;
	volatile app_uint16_t div2WRot0to1;
    volatile app_uint16_t div2WRot0to2;
	volatile app_uint16_t div2WRot1to2;
	volatile app_uint16_t div2WRot1to0;
	volatile app_uint16_t div2WRot2to0;
	volatile app_uint16_t div2WRot2to1;
}appParDwDiver_t;

typedef struct{
	volatile app_uint32_t heaterWaitCnt;
	volatile app_uint32_t heaterOffWaitCnt;
	volatile app_uint8_t activeHeaterRelay;
	volatile app_uint8_t currentHeaterRelay;
	volatile app_bool_t heaterTestDone;
	volatile app_bool_t heaterCheckFlag;
	volatile app_bool_t heaterOnFlag;
	volatile app_bool_t heaterTestFlag;
	volatile app_bool_t heaterOnCommand;
	volatile app_bool_t heaterCntFlag;
	volatile app_bool_t heaterOffCntFlag;
}appVarDwHeaterControl_t;

typedef struct{
	volatile app_uint32_t heaterOffWaitCntLimit;
	volatile app_uint32_t heaterWaitCntLimit;
	volatile app_uint32_t relaySwTime;
}appParDwHeaterControl_t;

typedef struct{
	volatile app_uint8_t posHeater;
	volatile app_bool_t flag;
	volatile app_uint32_t errorCntr;
	volatile app_uint32_t faultCntr;
	volatile appVarDwHeaterControl_t varHeaterControl;
	volatile app_bool_t flagHHP;
	volatile app_bool_t flagDoneHHP;
	volatile app_bool_t flagStartHHP;
	volatile app_uint32_t counterHHP;
}appVarDwHeater_t;

typedef struct{
	volatile app_bool_t enabled;
	volatile app_uint32_t errorCntrMax;
	volatile app_uint32_t faultCntrMax;
	volatile app_uint32_t duration;
	volatile app_uint32_t counterMaxHHP;
	volatile appParDwHeaterControl_t parHeaterControl;
}appParDwHeater_t;

typedef struct{
	volatile app_bool_t flag;
	volatile app_uint32_t faultCntr;
}appVarDwCrcCheck_t;

typedef struct{
	volatile app_bool_t enabled;
	volatile app_uint32_t faultCntrMax;
}appParDwCrcCheck_t;

typedef struct{
	volatile app_bool_t flag;
	volatile app_uint32_t faultCntr;
}appVarDwDoorSwitch_t;

typedef struct{
	volatile app_bool_t enabled;
	volatile app_uint32_t faultCntrMax;
}appParDwDoorSwitch_t;

typedef struct{
	volatile app_uint32_t faultRXCntr;
	volatile app_uint32_t faultTXCntr;
	volatile app_uint16_t faultResCntr;
	volatile app_bool_t flag;
}appVarDwUART_t;

typedef struct{
	volatile app_bool_t enabled;
	volatile app_uint32_t faultCntrMax;
	volatile app_uint16_t faultResMax;
}appParDwUART_t;

typedef struct{
	volatile app_bool_t flag;
	volatile app_uint32_t faultCntr;
}appVarDwRelays_t;

typedef struct{
	volatile app_bool_t enabled;
	volatile app_uint32_t faultCntrMax;
	volatile app_uint32_t adcValSyncMax;
}appParDwRelays_t;

typedef struct{
	volatile app_bool_t enabled;
	volatile app_uint32_t errorCntrMax;
	volatile app_uint32_t faultCntrMax;
	volatile app_float_t minCurr;
}appParDwDRCurrCheck_t;

typedef struct{
	volatile app_bool_t flag;
	volatile app_uint32_t errorCntr;
	volatile app_uint32_t faultCntr;
}appVarDwDRCurrCheck_t;

typedef enum{
    APP_STATE_NO_FAULT,
    APP_STATE_FAULT
} mc_app_state_t;

typedef struct{
	appVarDwDiver_t appVarDwDiverter;
	appVarDwHeater_t appVarDwHeater;
	appVarDwCrcCheck_t appVarDwCrcCheck;
	appVarDwDoorSwitch_t appVarDwDoorSwitch;
	appVarDwUART_t appVarDwUART;
	appVarDwRelays_t appVarDwRelays;
	appVarDwDRCurrCheck_t appVarDwDRCurrCheck;
	mc_app_state_t mc_app_state;
	volatile app_uint32_t cntrML;
	volatile app_uint32_t cntrSL;
	volatile app_uint32_t cntrFL;
	volatile app_uint32_t transTime;
	volatile app_uint32_t transRelTime;
	volatile app_uint32_t drainWorkTime;
	volatile app_uint32_t drainWaitTime;
	volatile app_uint32_t drainWindowTime;
	volatile app_uint32_t halt_duration;
	volatile app_uint32_t halt_counter;
	volatile app_uint32_t generatedFaultsBinary;
	volatile app_uint32_t appTotalGeneratedFault;
	volatile app_uint32_t noFaultCntr;
	volatile app_uint16_t refSpeedCirc;
	volatile app_uint16_t refSpeedDrain;
	volatile app_uint16_t prevRefSpeedCirc;
	volatile app_uint16_t prevRefSpeedDrain;
	volatile app_uint8_t activeState;
	volatile app_uint8_t circType;
	volatile app_uint8_t circOldType;
	volatile app_uint8_t circDesiredType;
	volatile app_uint8_t drainType;
	volatile app_uint8_t drainOldType;
	volatile app_uint8_t drainDesiredType;
	volatile app_bool_t startFlag;
	volatile app_bool_t transFlag;
	volatile app_bool_t transRelFlag;
	volatile app_bool_t drainReadyFlag;
	volatile app_bool_t appFatalFaultFlag;
	volatile app_bool_t fsCircFaultFlag;
	volatile app_bool_t fsDrainFaultFlag;
	volatile app_bool_t appFaultFlag;
	volatile app_bool_t drainWindowFlag;
	volatile app_bool_t drainOverRunFlag;
} appDwVar_t;	// variables for Dw application

typedef struct{
	app_uint32_t transReqTime;
	app_uint32_t transRelReqTime;
	app_uint32_t drainMaxWorkTime;
	app_uint32_t drainMaxWaitTime;
	app_uint32_t appSafetyReqTime;
	appParDwDiver_t appParDwDiverter;
	appParDwHeater_t appParDwHeater;
	appParDwCrcCheck_t appParDwCrcCheck;
	appParDwDoorSwitch_t appParDwDoorSwitch;
	appParDwUART_t appParDwUART;
	appParDwRelays_t appParDwRelays;
	appParDwDRCurrCheck_t appParDwDRCurrCheck;
	app_uint32_t noFaultCntrMax;
	app_uint32_t drainMaxWindowTime;


} appDwPar_t; // parameters for Dw application

typedef struct{
	appDwVar_t v;	// variables for Dw application
	appDwFp_t func;	// function pointers for Dw application
	appDwPar_t p;   // parameters for Dw application
} appDw_t;

extern mcv_rx_uart mcv_rx;
extern mcv_tx_uart mcv_tx;
extern appDw_t appDw;
extern const appDwPar_t *const dwp_px;



#endif
#endif /* SOURCE_APP_DW_TYPES_H_ */
