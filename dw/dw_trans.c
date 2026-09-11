/*
 * dw_trans.c
 *
 *  Created on: 8 Ara 2025
 *      Author: 26098107
 */
#include "dw_globals.h"

void appMotorInit(appDw_t* p_appDw)
{
	p_appDw->v.activeState = CIRCULATION;
	p_appDw->v.drainReadyFlag = APP_TRUE;
	p_appDw->v.drainWaitTime = 0;
	p_appDw->v.drainWorkTime = 0;
	p_appDw->v.drainWindowFlag = APP_FALSE;
	p_appDw->v.drainWindowTime = 0;
	p_appDw->v.drainOverRunFlag = APP_FALSE;
	p_appDw->v.prevRefSpeedCirc = 0;
	p_appDw->v.prevRefSpeedDrain = 0;
	p_appDw->v.refSpeedCirc = 0;
	p_appDw->v.refSpeedDrain = 0;
	p_appDw->v.startFlag = 0;
	p_appDw->v.transFlag = APP_TRUE;
	p_appDw->v.transRelFlag = APP_TRUE;
	p_appDw->v.transRelTime = 0;
	p_appDw->v.transTime = 0;
	appDwFunc.appDwPumpReset();

	refSpeed_M0.mc_motorIndex = CIRCULATION;
	refSpeed_M1.mc_motorIndex = DRAIN;

	refSpeed_M0.mc_sprefmec = 0.0f;
	refSpeed_M1.mc_sprefmec = 0.0f;

	appDw.v.cntrML = 0;
	appDw.v.cntrSL = 0;
	appDw.v.cntrFL = 0;

}

void appAdjustSpeedRamp(void)
{
	if(!mcv_rx.spRefCirc || !(mcv_rx.pilotValves & CIRCULATION_COMMAND_BITMASK))
	{
		refSpeed_M0.mc_rampin = (app_float_t)CIRC_STOP_RAMP;
		refSpeed_M0.mc_ramp_duration =  0.01f;
	}
	else
		refSpeed_M0.mc_rampin = (app_float_t)CIRC_NORMAL_RAMP;

	if(!mcv_rx.spRefDrain || !(mcv_rx.pilotValves & DRAIN_COMMAND_BITMASK))
	{
		refSpeed_M1.mc_rampin = (app_float_t)DRAIN_STOP_RAMP;
		refSpeed_M1.mc_ramp_duration =  0.01f;
	}
	else
	{
		if(mcGetRefSpeed(DRAIN) > (app_float_t)DRAIN_SLOW_RAMP_SPEED)
			refSpeed_M1.mc_rampin = (app_float_t)DRAIN_SLOW_RAMP;
		else
			refSpeed_M1.mc_rampin = (app_float_t)DRAIN_NORMAL_RAMP;

	}
}

void appTransMotor(appDw_t* p_appDw)
{
	if(mcv_rx.pilotValves & CIRCULATION_COMMAND_BITMASK)
	{
		if((mcv_rx.pilotDiverter != p_appDw->v.appVarDwDiverter.divPosReal)
				&& (mcv_rx.spRefCirc>(app_float_t)CIRC_COMMAND_FOR_DIVERTER)
				&& (p_appDw->v.appVarDwDiverter.flag == APP_FALSE))

		{
			mcv_rx.spRefM1Rx = (app_float_t)CIRC_COMMAND_FOR_DIVERTER;
		    refSpeed_M0.mc_rampin = (app_float_t)CIRC_RAMP_FOR_DIVERTER;
		}
		else
		{
			mcv_rx.spRefM1Rx = mcv_rx.spRefCirc;
			mcv_rx.spRefM2Rx = 0;
		}
/*
		if(p_appDw->v.appVarDwDiverter.diverInitFlag
				|| (!p_appDw->v.appVarDwDiverter.divReadyFlag))
		{
			mcv_rx.spRefM1Rx = 0;
			mcv_rx.spRefM2Rx = 0;
		}
		*/
	}
	else if(mcv_rx.pilotValves & DRAIN_COMMAND_BITMASK)
	{
		mcv_rx.spRefM1Rx = 0;
		mcv_rx.spRefM2Rx = mcv_rx.spRefDrain;
/*
		if(p_appDw->v.appVarDwDiverter.diverInitFlag
				|| (!p_appDw->v.appVarDwDiverter.divReadyFlag))
		{
			mcv_rx.spRefM1Rx = 0;
			mcv_rx.spRefM2Rx = 0;
		}
		*/
	}
	else
	{
		mcv_rx.spRefM1Rx = 0;
		mcv_rx.spRefM2Rx = 0;
	}




	p_appDw->v.refSpeedCirc = mcv_rx.spRefM1Rx;
	p_appDw->v.refSpeedDrain = mcv_rx.spRefM2Rx;

	if(!p_appDw->v.startFlag && (p_appDw->v.refSpeedCirc || p_appDw->v.refSpeedDrain))
	{
		p_appDw->v.prevRefSpeedCirc = p_appDw->v.refSpeedCirc;
		p_appDw->v.prevRefSpeedDrain = p_appDw->v.refSpeedDrain;
		p_appDw->v.startFlag = APP_TRUE;
	}
	else
	{
		;
	}

	if((!p_appDw->v.prevRefSpeedCirc != !p_appDw->v.refSpeedCirc) ||
			(!p_appDw->v.prevRefSpeedDrain != !p_appDw->v.refSpeedDrain))
	{
		p_appDw->v.transFlag = APP_TRUE;
		refSpeed_M0.mc_sprefmec = 0;
		refSpeed_M1.mc_sprefmec = 0;
	}
	else if(!p_appDw->v.transFlag && p_appDw->v.refSpeedCirc && !p_appDw->v.refSpeedDrain)
	{
		p_appDw->v.activeState = CIRCULATION;
		if(appDw.v.funcInitReady == true)
		{
			appDwFunc.appDwPumpReset();
		}
		if(p_appDw->v.transRelFlag)
		{
			refSpeed_M0.mc_sprefmec = 0;
			refSpeed_M1.mc_sprefmec = 0;
		}
		else
		{
		refSpeed_M0.mc_sprefmec = p_appDw->v.refSpeedCirc;
		refSpeed_M1.mc_sprefmec = 0;
		}

	}
	else if(!p_appDw->v.transFlag && !p_appDw->v.refSpeedCirc && p_appDw->v.refSpeedDrain)
	{
		p_appDw->v.activeState = DRAIN;
		if(appDw.v.funcInitReady == true)
		{
			appDwFunc.appDwPumpSet();
		}
		if(p_appDw->v.transRelFlag || (p_appDw->v.drainReadyFlag == APP_FALSE))
		{
			refSpeed_M0.mc_sprefmec = 0;
			refSpeed_M1.mc_sprefmec = 0;
		}
		else
		{
			refSpeed_M0.mc_sprefmec = 0;
			refSpeed_M1.mc_sprefmec = p_appDw->v.refSpeedDrain;
		}

	}

	else
	{
		;
	}

	p_appDw->v.prevRefSpeedCirc = p_appDw->v.refSpeedCirc;
	p_appDw->v.prevRefSpeedDrain = p_appDw->v.refSpeedDrain;

	mcRequestSpeed(&refSpeed_M0);
	mcRequestSpeed(&refSpeed_M1);
}

void appTransTimer(appDw_t* p_appDw)
{
	float actSpCirc, actSpDrain;
	actSpCirc = mcGetActSpeed(CIRCULATION);
	actSpDrain = mcGetActSpeed(DRAIN);

    if(p_appDw->v.transFlag)
    	++p_appDw->v.transTime;
    else
    {
    	;
    }

    if((p_appDw->v.transTime >= p_appDw->p.transReqTime) && (actSpCirc == 0) && (actSpDrain == 0))
    {
    	p_appDw->v.transFlag = false;
    	p_appDw->v.transTime = 0;
    	p_appDw->v.transRelFlag = true;
    }
    else
    {
    	;
    }

    if(p_appDw->v.transRelFlag)
    {
    	++p_appDw->v.transRelTime;
    }
    else
    {
    	;
    }

    if(p_appDw->v.transRelTime >= p_appDw->p.transRelReqTime)
    {
    	p_appDw->v.transRelFlag = false;
    	p_appDw->v.transRelTime = 0;
    }
    else
    {
    	;
    }
    /*
    if(p_appDw->v.transFlag || p_appDw->v.transRelFlag || ((refSpeed_M0.mc_sprefmec == 0)
    		&& (refSpeed_M1.mc_sprefmec == 0)))
    {
    	drvPwmDisable();
    }
    else
    {
    	drvPwmEnable();
    }
    */
}
