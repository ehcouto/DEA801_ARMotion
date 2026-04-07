/*
 * dw_heater.c
 *
 *  Created on: 4 Ara 2025
 *      Author: 26098107
 */
#include "dw_globals.h"

void appHeaterInit(appDw_t* p_appDw)
{
	p_appDw->v.appVarDwHeater.faultCntr = 0;
	p_appDw->v.appVarDwHeater.posHeater = 0;
	p_appDw->v.appVarDwHeater.flag = APP_FALSE;
	p_appDw->v.appVarDwHeater.varHeaterControl.heaterTestDone = APP_FALSE;
	p_appDw->v.appVarDwHeater.varHeaterControl.heaterCheckFlag = APP_FALSE;
	p_appDw->v.appVarDwHeater.varHeaterControl.heaterOnFlag = APP_FALSE;
	p_appDw->v.appVarDwHeater.varHeaterControl.activeHeaterRelay = SWITCH_HEATER;
	p_appDw->v.appVarDwHeater.varHeaterControl.heaterTestFlag = APP_FALSE;
	p_appDw->v.appVarDwHeater.varHeaterControl.heaterOnCommand = APP_FALSE;
	p_appDw->v.appVarDwHeater.varHeaterControl.heaterCntFlag = APP_FALSE;
	p_appDw->v.appVarDwHeater.varHeaterControl.currentHeaterRelay = RELAY_HEATER;
	p_appDw->v.appVarDwHeater.flagDoneHHP = APP_FALSE;
	p_appDw->v.appVarDwHeater.flagStartHHP = APP_FALSE;
	p_appDw->v.appVarDwHeater.counterHHP = 0;
}

void appHeaterFlagInit(appDw_t* p_appDw)
{
	p_appDw->v.appVarDwHeater.varHeaterControl.heaterTestDone = APP_FALSE;
	p_appDw->v.appVarDwHeater.varHeaterControl.heaterCheckFlag = APP_FALSE;
	p_appDw->v.appVarDwHeater.varHeaterControl.heaterOnFlag = APP_FALSE;
	p_appDw->v.appVarDwHeater.varHeaterControl.heaterTestFlag = APP_FALSE;
	p_appDw->v.appVarDwHeater.varHeaterControl.heaterOnCommand = APP_FALSE;
	p_appDw->v.appVarDwHeater.varHeaterControl.heaterCntFlag = APP_FALSE;
	//p_appDw->v.appVarDwHeater.varHeaterControl.heaterOffCntFlag = APP_FALSE;
}


void appHeaterControl(appDw_t* p_appDw)
{
	if((mcv_rx.pilotValves & HEATER_VALVE_BITMASK)
			&& p_appDw->v.appVarDwHeater.varHeaterControl.activeHeaterRelay == RELAY_HEATER
			&& p_appDw->v.appVarDwHeater.varHeaterControl.heaterOnCommand
			&& !p_appDw->v.appVarDwHeater.varHeaterControl.heaterOnFlag
			|| ((mcv_rx.pilotValves & HEATER_VALVE_BITMASK)
			&& p_appDw->v.appVarDwHeater.varHeaterControl.heaterCntFlag))
	{
		p_appDw->v.appVarDwHeater.varHeaterControl.heaterCntFlag = APP_TRUE;
		if((p_appDw->v.appVarDwHeater.varHeaterControl.heaterWaitCnt
				> p_appDw->p.appParDwHeater.parHeaterControl.relaySwTime)
				&& (p_appDw->v.appVarDwHeater.varHeaterControl.heaterWaitCnt
						< p_appDw->p.appParDwHeater.parHeaterControl.relaySwTime*2))
			appDwFunc.appDwRelayHeaterReset();

		if(p_appDw->v.appVarDwHeater.varHeaterControl.heaterWaitCnt
				>= (p_appDw->p.appParDwHeater.parHeaterControl.relaySwTime*3))
			appDwFunc.appDwSwitchHeaterSet();

		if(p_appDw->v.appVarDwHeater.varHeaterControl.heaterWaitCnt
				>= (p_appDw->p.appParDwHeater.parHeaterControl.relaySwTime*4))
			appDwFunc.appDwRelayHeaterSet();


		p_appDw->v.appVarDwHeater.varHeaterControl.heaterOnFlag = APP_TRUE;
		p_appDw->v.appVarDwHeater.varHeaterControl.heaterCheckFlag = APP_FALSE;
		p_appDw->v.appVarDwHeater.varHeaterControl.activeHeaterRelay = SWITCH_HEATER;
		p_appDw->v.appVarDwHeater.varHeaterControl.currentHeaterRelay = SWITCH_HEATER;
		p_appDw->v.appVarDwHeater.varHeaterControl.heaterOffCntFlag = APP_FALSE;

	}

	else if((mcv_rx.pilotValves & HEATER_VALVE_BITMASK)
			&& p_appDw->v.appVarDwHeater.varHeaterControl.activeHeaterRelay == SWITCH_HEATER
			&& p_appDw->v.appVarDwHeater.varHeaterControl.heaterOnCommand
			&& !p_appDw->v.appVarDwHeater.varHeaterControl.heaterOnFlag)
	{
		appDwFunc.appDwRelayHeaterSet();
		appDwFunc.appDwSwitchHeaterSet();
		p_appDw->v.appVarDwHeater.varHeaterControl.heaterOnFlag = APP_TRUE;
		p_appDw->v.appVarDwHeater.varHeaterControl.heaterCheckFlag = APP_FALSE;
		p_appDw->v.appVarDwHeater.varHeaterControl.activeHeaterRelay = RELAY_HEATER;
		p_appDw->v.appVarDwHeater.varHeaterControl.heaterCntFlag = APP_FALSE;
		p_appDw->v.appVarDwHeater.varHeaterControl.heaterOffCntFlag = APP_FALSE;

	}

	else if((mcv_rx.pilotValves & HEATER_VALVE_BITMASK)
			&& p_appDw->v.appVarDwHeater.varHeaterControl.heaterTestFlag
			&& !p_appDw->v.appVarDwHeater.varHeaterControl.heaterOnCommand)
	{
		appDwFunc.appDwRelayHeaterSet();
		appDwFunc.appDwSwitchHeaterReset();
		p_appDw->v.appVarDwHeater.varHeaterControl.heaterTestDone = APP_TRUE;
		p_appDw->v.appVarDwHeater.varHeaterControl.heaterCntFlag = APP_FALSE;
		p_appDw->v.appVarDwHeater.varHeaterControl.heaterOnFlag = APP_FALSE;
		p_appDw->v.appVarDwHeater.varHeaterControl.heaterOffCntFlag = APP_FALSE;

	}


	else if(!(mcv_rx.pilotValves & HEATER_VALVE_BITMASK)
			&& (p_appDw->v.appVarDwHeater.varHeaterControl.activeHeaterRelay == SWITCH_HEATER))
	{
		p_appDw->v.appVarDwHeater.varHeaterControl.heaterOffCntFlag = APP_TRUE;
		if(p_appDw->v.appVarDwHeater.flagHHP && !p_appDw->v.appVarDwDoorSwitch.flag
				&& !p_appDw->v.appVarDwHeater.flagDoneHHP)
		{
			appDwFunc.appDwRelayHeaterSet();
			p_appDw->v.appVarDwHeater.flagStartHHP = APP_TRUE;
		}
		else
		{
			appDwFunc.appDwRelayHeaterReset();
			p_appDw->v.appVarDwHeater.flagStartHHP = APP_TRUE;
		}

		if(p_appDw->v.appVarDwHeater.varHeaterControl.heaterOffWaitCnt
				> (p_appDw->p.appParDwHeater.parHeaterControl.relaySwTime))
			appDwFunc.appDwSwitchHeaterReset();

		appHeaterFlagInit(&appDw);

	}
	else if(!(mcv_rx.pilotValves & HEATER_VALVE_BITMASK)
			&& (p_appDw->v.appVarDwHeater.varHeaterControl.activeHeaterRelay == RELAY_HEATER))
	{
		p_appDw->v.appVarDwHeater.varHeaterControl.heaterOffCntFlag = APP_TRUE;
		appDwFunc.appDwSwitchHeaterReset();

		if(p_appDw->v.appVarDwHeater.varHeaterControl.heaterOffWaitCnt
				> (p_appDw->p.appParDwHeater.parHeaterControl.relaySwTime))
		{
			if(p_appDw->v.appVarDwHeater.flagHHP && !p_appDw->v.appVarDwDoorSwitch.flag
					&& !p_appDw->v.appVarDwHeater.flagDoneHHP)
			{
				appDwFunc.appDwRelayHeaterSet();
				p_appDw->v.appVarDwHeater.flagStartHHP = APP_TRUE;
			}
			else
			{
				appDwFunc.appDwRelayHeaterReset();
				p_appDw->v.appVarDwHeater.flagStartHHP = APP_TRUE;

			}
		}

		appHeaterFlagInit(&appDw);

	}

}

void appHeaterTimers(appDw_t* p_appDw)
{
	if(p_appDw->v.appVarDwHeater.varHeaterControl.heaterCntFlag)
		++p_appDw->v.appVarDwHeater.varHeaterControl.heaterWaitCnt;
	else
		p_appDw->v.appVarDwHeater.varHeaterControl.heaterWaitCnt = 0;

	if(p_appDw->v.appVarDwHeater.varHeaterControl.heaterWaitCnt
			>= p_appDw->p.appParDwHeater.parHeaterControl.heaterWaitCntLimit)
		p_appDw->v.appVarDwHeater.varHeaterControl.heaterWaitCnt
		= p_appDw->p.appParDwHeater.parHeaterControl.heaterWaitCntLimit;


	if(p_appDw->v.appVarDwHeater.varHeaterControl.heaterOffCntFlag)
		++p_appDw->v.appVarDwHeater.varHeaterControl.heaterOffWaitCnt;
	else
		p_appDw->v.appVarDwHeater.varHeaterControl.heaterOffWaitCnt = 0;

	if(p_appDw->v.appVarDwHeater.varHeaterControl.heaterOffWaitCnt
			>= p_appDw->p.appParDwHeater.parHeaterControl.heaterOffWaitCntLimit)
		p_appDw->v.appVarDwHeater.varHeaterControl.heaterOffWaitCnt
		= p_appDw->p.appParDwHeater.parHeaterControl.heaterOffWaitCntLimit;

	if(p_appDw->v.appVarDwHeater.flagHHP && p_appDw->v.appVarDwHeater.flagStartHHP
			&& !p_appDw->v.appVarDwDoorSwitch.flag && !p_appDw->v.appVarDwHeater.flagDoneHHP)
		++p_appDw->v.appVarDwHeater.counterHHP;
	else
		p_appDw->v.appVarDwHeater.counterHHP = 0;

	if(p_appDw->v.appVarDwHeater.counterHHP >= p_appDw->p.appParDwHeater.counterMaxHHP)
		p_appDw->v.appVarDwHeater.flagDoneHHP = APP_TRUE;

}


void appHotHighPotTest(appDw_t* p_appDw)
{
	if(mcv_rx.heaterFirstCfg & 0x1U)
		p_appDw->v.appVarDwHeater.flagHHP = APP_TRUE;
	else
		p_appDw->v.appVarDwHeater.flagHHP = APP_FALSE;

	if(!p_appDw->v.appVarDwHeater.flagHHP)
	{
		p_appDw->v.appVarDwHeater.flagDoneHHP = APP_FALSE;
		p_appDw->v.appVarDwHeater.flagStartHHP = APP_FALSE;
		p_appDw->v.appVarDwHeater.counterHHP = 0;
	}

}


