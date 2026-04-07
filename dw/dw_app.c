/*
 * dw_app.c
 *
 *  Created on: 10 Şub 2025
 *      Author: 26098107
 */


#include "app_config.h"

#ifdef ENABLE_DISHWASHER_APPLICATION

#include "mc.h"
#include "brd.h"
#include "dw_globals.h"
#include "string.h"
#include "drv_config.h"
#include "drv.h"
#include "stdint.h"

appDwFp_t appDwFunc;

void appDwHandlerFL(void)
{
	appDw.v.cntrFL++;
}


void appDwHandlerSL(void)
{
	appDw.v.cntrSL++;

}

void appDwHandlerML(void)
{
	appDw.v.cntrML++;

	appUARTHandler(&appDw);
	appAdjustSpeedRamp();
	appTransMotor(&appDw);

	appGetCircType(&appDw);
	appSelectCircMotor(&appDw);
	appGetDrainType(&appDw);
	appSelectDrainMotor(&appDw);
	appHotHighPotTest(&appDw);
}


void appDwInit(void)
{
	appDw.p = *dwp_px;

	appDwFunc.appDwDispenserReset = brdDispenserReset;
	appDwFunc.appDwDispenserSet = brdDispenserSet;
	appDwFunc.appDwDiverterReset = brdDiverterReset;
	appDwFunc.appDwDiverterSet = brdDiverterSet;
	appDwFunc.appDwGetTempIPM_M1 = brdGetTempIPM_M1;
	appDwFunc.appDwGetTempIPM_M2 = brdGetTempIPM_M2;
	appDwFunc.appDwH2OReset = brdH2OReset;
	appDwFunc.appDwH2OSet = brdH2OSet;
	appDwFunc.appDwPumpReset = brdPumpReset;
	appDwFunc.appDwPumpSet = brdPumpSet;
	appDwFunc.appDwRegenReset = brdRegenReset;
	appDwFunc.appDwRegenSet = brdRegenSet;
	appDwFunc.appDwRelayHeaterReset = brdRelayHeaterReset;
	appDwFunc.appDwRelayHeaterSet = brdRelayHeaterSet;
	appDwFunc.appDwSwitchHeaterReset = brdSwitchHeaterReset;
	appDwFunc.appDwSwitchHeaterSet = brdSwitchHeaterSet;
	appDwFunc.appPwmDisable_M1 = brdPwmDisable_M1;
	appDwFunc.appPwmDisable_M2 = brdPwmDisable_M2;

	appUARTInit();
	appMotorInit(&appDw);
	appDiverterInit(&appDw);
	appHeaterInit(&appDw);
	appHeaterFlagInit(&appDw);
	appCrcCheckInit(&appDw);
	appDoorSwitchCheckInit(&appDw);
	appRelaysCheckInit(&appDw);


	appResetAllFaultCounters(&appDw);
	appResetErrorCountersFlags(&appDw);
}

void appResetAllFaultCounters(appDw_t* p_appDw)
{
	p_appDw->v.appVarDwHeater.faultCntr = 0;
	p_appDw->v.appVarDwCrcCheck.faultCntr = 0;
	p_appDw->v.appVarDwDoorSwitch.faultCntr = 0;
	p_appDw->v.appFaultFlag = APP_FALSE;
}

void appResetErrorCountersFlags(appDw_t* p_appDw)
{
	p_appDw->v.appVarDwHeater.flag = APP_FALSE;
	p_appDw->v.appVarDwCrcCheck.flag = APP_FALSE;
	p_appDw->v.appVarDwDoorSwitch.flag = APP_FALSE;
}

void appValveControl(appDw_t* p_appDw)
{
	if(mcv_rx.pilotValves & WATER_VALVE_BITMASK)
		appDwFunc.appDwH2OSet();
	else
		appDwFunc.appDwH2OReset();

	if(mcv_rx.pilotValves & DISPENSER_VALVE_BITMASK)
		appDwFunc.appDwDispenserSet();
	else
		appDwFunc.appDwDispenserReset();

	if(mcv_rx.pilotValves & REGEN_VALVE_BITMASK)
		appDwFunc.appDwRegenSet();
	else
		appDwFunc.appDwRegenReset();

	appHeaterControl(&appDw);

}

#endif
