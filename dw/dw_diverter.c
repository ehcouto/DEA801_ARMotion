/*
 * dw_diverter.c
 *
 *  Created on: 4 Ara 2025
 *      Author: 26098107
 */

#include "dw_globals.h"


void appDiverterInit(appDw_t* p_appDw)
{
	p_appDw->v.appVarDwDiverter.posDivCnt = 0;
	p_appDw->v.appVarDwDiverter.divStopCnt = 0;
	p_appDw->v.appVarDwDiverter.divRotCnt = 0;
	p_appDw->v.appVarDwDiverter.divRotTimer = 0;
	p_appDw->v.appVarDwDiverter.divPosReal = DIVER_UNKNOWN_POS;
	p_appDw->v.appVarDwDiverter.divReadyFlag = APP_FALSE;
	p_appDw->v.appVarDwDiverter.diverCommand = APP_FALSE;
	p_appDw->v.appVarDwDiverter.diverInitFlag = APP_FALSE;
	p_appDw->v.appVarDwDiverter.faultCntr = 0;
	p_appDw->v.appVarDwDiverter.diverterType = DIVERTERLESS;
	p_appDw->v.appVarDwDiverter.firstFlag2W = APP_FALSE;
	p_appDw->v.appVarDwDiverter.secondFlag2W = APP_FALSE;
	p_appDw->v.appVarDwDiverter.divSyncFlag = APP_FALSE;
	p_appDw->v.appVarDwDiverter.adcHighPrev = APP_FALSE;
	mcv_rx.commandDiverter = 0;
}

void appDiverterReset(appDw_t* p_appDw)
{
	p_appDw->v.appVarDwDiverter.posDivCnt = 0;
	p_appDw->v.appVarDwDiverter.divStopCnt = 0;
	p_appDw->v.appVarDwDiverter.divRotCnt = 0;
	p_appDw->v.appVarDwDiverter.divRotTimer = 0;
	p_appDw->v.appVarDwDiverter.divPosReal = DIVER_UNKNOWN_POS;
	p_appDw->v.appVarDwDiverter.divReadyFlag = APP_FALSE;
	p_appDw->v.appVarDwDiverter.diverCommand = APP_FALSE;
	p_appDw->v.appVarDwDiverter.diverInitFlag = APP_FALSE;
	p_appDw->v.appVarDwDiverter.firstFlag2W = APP_FALSE;
	p_appDw->v.appVarDwDiverter.secondFlag2W = APP_FALSE;
	p_appDw->v.appVarDwDiverter.divSyncFlag = APP_FALSE;
	p_appDw->v.appVarDwDiverter.adcHighPrev = APP_FALSE;
	mcv_rx.commandDiverter = 0;
}

void appDetectDiverterType(appDw_t* p_appDw)
{
	p_appDw->v.appVarDwDiverter.divDesiredType = (mcv_rx.selectComponent >> DIVERTER_TYPE_BIT_FIELD)
			& DIVERTER_TYPE_BIT_AREA;

	p_appDw->v.appVarDwDiverter.diverterOldType = p_appDw->v.appVarDwDiverter.diverterType;

	if(p_appDw->v.appVarDwDiverter.divDesiredType == DIVERTER_3WAY_BITMASK)
		p_appDw->v.appVarDwDiverter.diverterType = DIVERTER_3WAY;

	else if(p_appDw->v.appVarDwDiverter.divDesiredType == DIVERTER_2WAY_BITMASK)
		p_appDw->v.appVarDwDiverter.diverterType = DIVERTER_2WAY;

	else if(p_appDw->v.appVarDwDiverter.divDesiredType == DIVERTERLESS_BITMASK)
		p_appDw->v.appVarDwDiverter.diverterType = DIVERTERLESS;

	else
	{
		;
	}

}

void appDiverter3Way(appDw_t* p_appDw)
{
	if(!p_appDw->v.appVarDwDiverter.posDivCnt
			&& !p_appDw->v.appVarDwDiverter.divStopCnt
			&& !p_appDw->v.appVarDwDiverter.divRotCnt
			&& (mcv_rx.commandDiverter != DIVER_INIT_POS))
		mcv_rx.pilotDiverter = mcv_rx.commandDiverter;


	if((mcv_rx.commandDiverter == DIVER_INIT_POS) && !p_appDw->v.appVarDwDiverter.diverInitFlag)
	{
		appDiverterInit(&appDw);
		mcv_rx.pilotDiverter = 0;
		mcv_rx.commandDiverter = 0;
		p_appDw->v.appVarDwDiverter.diverInitFlag = APP_TRUE;
	}
	else if((mcv_rx.pilotDiverter != p_appDw->v.appVarDwDiverter.divPosReal)
			&& p_appDw->v.appVarDwDiverter.diverInitFlag)
	{
		mcv_rx.pilotDiverter = 0;
		mcv_rx.commandDiverter = 0;
	}


	if(mcv_rx.pilotDiverter != p_appDw->v.appVarDwDiverter.divPosReal)
	{
		appDwFunc.appDwDiverterSet();
		p_appDw->v.appVarDwDiverter.diverCommand = APP_TRUE;
		p_appDw->v.appVarDwDiverter.divPosRealTx = DIVER_UNKNOWN_POS;
	}

	else
	{
		appDwFunc.appDwDiverterReset();
		p_appDw->v.appVarDwDiverter.diverCommand = APP_FALSE;
		p_appDw->v.appVarDwDiverter.posDivCnt = 0;
		p_appDw->v.appVarDwDiverter.divStopCnt = 0;
		p_appDw->v.appVarDwDiverter.divRotCnt = 0;
		p_appDw->v.appVarDwDiverter.divPosRealTx = p_appDw->v.appVarDwDiverter.divPosReal;
	}


	if(p_appDw->v.appVarDwDiverter.diverCommand && !p_appDw->v.appVarDwDiverter.divReadyFlag)
	{
		uint8_t adcHigh = (adcValDiverter > p_appDw->p.appParDwDiverter.divAdcLow);
		if(!p_appDw->v.appVarDwDiverter.divSyncFlag)
		{
			if(p_appDw->v.appVarDwDiverter.adcHighPrev && !adcHigh)
			{
				p_appDw->v.appVarDwDiverter.divSyncFlag = APP_TRUE;
				p_appDw->v.appVarDwDiverter.posDivCnt   = 0;
				p_appDw->v.appVarDwDiverter.divStopCnt  = 0;
			}
			p_appDw->v.appVarDwDiverter.adcHighPrev = adcHigh;
		}
		else
		{
			uint8_t adcHighNow = (adcValDiverter > p_appDw->p.appParDwDiverter.divAdcLow);

			if(!p_appDw->v.appVarDwDiverter.adcHighPrev && adcHighNow)
				p_appDw->v.appVarDwDiverter.divStopCnt = 0;
			p_appDw->v.appVarDwDiverter.adcHighPrev = adcHighNow;

			if(adcValDiverter > p_appDw->p.appParDwDiverter.divAdcLow)
				++p_appDw->v.appVarDwDiverter.posDivCnt;
			else
			{
				++p_appDw->v.appVarDwDiverter.divStopCnt;
				if((p_appDw->v.appVarDwDiverter.posDivCnt > p_appDw->p.appParDwDiverter.divLowMin)
						&& (p_appDw->v.appVarDwDiverter.posDivCnt < p_appDw->p.appParDwDiverter.divPos0HighMin))
					p_appDw->v.appVarDwDiverter.posDivCnt = 0;
			}


			if(p_appDw->v.appVarDwDiverter.divStopCnt > p_appDw->p.appParDwDiverter.divHighMin
					&& p_appDw->v.appVarDwDiverter.divStopCnt < p_appDw->p.appParDwDiverter.divHighMax
					&& (adcValDiverter > p_appDw->p.appParDwDiverter.divAdcLow))
				p_appDw->v.appVarDwDiverter.divStopCnt = 0;
			else
			{
				;
			}

			if(p_appDw->v.appVarDwDiverter.posDivCnt > p_appDw->p.appParDwDiverter.divLowMin
					&& p_appDw->v.appVarDwDiverter.posDivCnt < p_appDw->p.appParDwDiverter.divLowMax
					&& (adcValDiverter < p_appDw->p.appParDwDiverter.divAdcLow))
				p_appDw->v.appVarDwDiverter.posDivCnt = 0;
			else
			{
				;
			}


			if(p_appDw->v.appVarDwDiverter.posDivCnt>p_appDw->p.appParDwDiverter.divPos0HighMin
					&& p_appDw->v.appVarDwDiverter.posDivCnt<p_appDw->p.appParDwDiverter.divPos0HighMax
					&& p_appDw->v.appVarDwDiverter.divStopCnt>p_appDw->p.appParDwDiverter.divPos0HighMin
					&& p_appDw->v.appVarDwDiverter.divStopCnt<p_appDw->p.appParDwDiverter.divPos0HighMax
					&& (adcValDiverter < p_appDw->p.appParDwDiverter.divAdcLow))
			{
				p_appDw->v.appVarDwDiverter.divPosReal = 0;
				p_appDw->v.appVarDwDiverter.divReadyFlag = APP_TRUE;
				p_appDw->v.appVarDwDiverter.diverInitFlag = APP_FALSE;

			}
			else
			{
				;
			}
		}

	}

	else if(p_appDw->v.appVarDwDiverter.diverCommand && p_appDw->v.appVarDwDiverter.divReadyFlag)
	{
		if((mcv_rx.pilotDiverter - p_appDw->v.appVarDwDiverter.divPosReal) < 0)
			p_appDw->v.appVarDwDiverter.divRotTimer = (mcv_rx.pilotDiverter - p_appDw->v.appVarDwDiverter.divPosReal + p_appDw->p.appParDwDiverter.divPosNum)*p_appDw->p.appParDwDiverter.div1TurnTime;
		else
			p_appDw->v.appVarDwDiverter.divRotTimer = (mcv_rx.pilotDiverter - p_appDw->v.appVarDwDiverter.divPosReal)*p_appDw->p.appParDwDiverter.div1TurnTime;

		++p_appDw->v.appVarDwDiverter.divRotCnt;
		if(p_appDw->v.appVarDwDiverter.divRotCnt >= p_appDw->v.appVarDwDiverter.divRotTimer)
		{
			p_appDw->v.appVarDwDiverter.divPosReal = mcv_rx.pilotDiverter;
			p_appDw->v.appVarDwDiverter.divRotCnt = 0;
		}
		else
		{
			;
		}
	}

	if(((p_appDw->v.appVarDwDiverter.divStopCnt + p_appDw->v.appVarDwDiverter.posDivCnt) > (p_appDw->p.appParDwDiverter.div1TurnTime * (p_appDw->p.appParDwDiverter.divPosNum + 1))) &&
			(mcv_rx.pilotDiverter != p_appDw->v.appVarDwDiverter.divPosReal))
	{
		appDiverterReset(&appDw);
	}

}

void appDiverter2Way(appDw_t* p_appDw)
{
	if(!p_appDw->v.appVarDwDiverter.posDivCnt
			&& !p_appDw->v.appVarDwDiverter.divStopCnt
			&& !p_appDw->v.appVarDwDiverter.divRotCnt
			&& (mcv_rx.commandDiverter != DIVER_INIT_POS))
		mcv_rx.pilotDiverter = mcv_rx.commandDiverter;

	if((mcv_rx.commandDiverter == DIVER_INIT_POS) && !p_appDw->v.appVarDwDiverter.diverInitFlag)
	{
		appDiverterInit(&appDw);
		mcv_rx.pilotDiverter = 0;
		mcv_rx.commandDiverter = 0;
		p_appDw->v.appVarDwDiverter.diverInitFlag = APP_TRUE;
	}
	else if((mcv_rx.pilotDiverter != p_appDw->v.appVarDwDiverter.divPosReal) && p_appDw->v.appVarDwDiverter.diverInitFlag)
	{
		mcv_rx.pilotDiverter = 0;
		mcv_rx.commandDiverter = 0;
	}


	if(mcv_rx.pilotDiverter != p_appDw->v.appVarDwDiverter.divPosReal)
	{
		appDwFunc.appDwDiverterSet();
		p_appDw->v.appVarDwDiverter.diverCommand = APP_TRUE;
		p_appDw->v.appVarDwDiverter.divPosRealTx = DIVER_UNKNOWN_POS;
	}

	else
	{
		appDwFunc.appDwDiverterReset();
		p_appDw->v.appVarDwDiverter.diverCommand = APP_FALSE;
		p_appDw->v.appVarDwDiverter.posDivCnt = 0;
		p_appDw->v.appVarDwDiverter.divStopCnt = 0;
		p_appDw->v.appVarDwDiverter.divRotCnt = 0;
		p_appDw->v.appVarDwDiverter.divPosRealTx = p_appDw->v.appVarDwDiverter.divPosReal;
	}


	if(p_appDw->v.appVarDwDiverter.diverCommand && !p_appDw->v.appVarDwDiverter.divReadyFlag)
	{

		if(p_appDw->v.appVarDwDiverter.posDivCnt >  p_appDw->p.appParDwDiverter.div2WPos0HighMin
				&& p_appDw->v.appVarDwDiverter.posDivCnt <  p_appDw->p.appParDwDiverter.div2WPos0HighMax
				&& !p_appDw->v.appVarDwDiverter.secondFlag2W)
			p_appDw->v.appVarDwDiverter.firstFlag2W = APP_TRUE;


		if(p_appDw->v.appVarDwDiverter.divStopCnt >  p_appDw->p.appParDwDiverter.div2WPos0LowMin
				&& p_appDw->v.appVarDwDiverter.divStopCnt <  p_appDw->p.appParDwDiverter.div2WPos0LowMax
				&& p_appDw->v.appVarDwDiverter.firstFlag2W)
			p_appDw->v.appVarDwDiverter.secondFlag2W = APP_TRUE;


		if(adcValDiverter > p_appDw->p.appParDwDiverter.divAdcLow)
		{
			++p_appDw->v.appVarDwDiverter.posDivCnt;
			p_appDw->v.appVarDwDiverter.divStopCnt = 0;
		}

		else
		{
			++p_appDw->v.appVarDwDiverter.divStopCnt;
			p_appDw->v.appVarDwDiverter.posDivCnt = 0;
		}

		if(p_appDw->v.appVarDwDiverter.firstFlag2W && p_appDw->v.appVarDwDiverter.secondFlag2W)
		{
			p_appDw->v.appVarDwDiverter.divPosReal = 0;
			p_appDw->v.appVarDwDiverter.divReadyFlag = APP_TRUE;
			p_appDw->v.appVarDwDiverter.diverInitFlag = APP_FALSE;
			p_appDw->v.appVarDwDiverter.firstFlag2W = APP_FALSE;
			p_appDw->v.appVarDwDiverter.secondFlag2W = APP_FALSE;

		}
		else
		{
			;
		}
	}

	else if(p_appDw->v.appVarDwDiverter.diverCommand && p_appDw->v.appVarDwDiverter.divReadyFlag)
	{

		if(mcv_rx.pilotDiverter == 1 && p_appDw->v.appVarDwDiverter.divPosReal == 0)
			p_appDw->v.appVarDwDiverter.divRotTimer = p_appDw->p.appParDwDiverter.div2WRot0to1;
		else if(mcv_rx.pilotDiverter == 2 && p_appDw->v.appVarDwDiverter.divPosReal == 0)
			p_appDw->v.appVarDwDiverter.divRotTimer = p_appDw->p.appParDwDiverter.div2WRot0to2;
		else if(mcv_rx.pilotDiverter == 2 && p_appDw->v.appVarDwDiverter.divPosReal == 1)
			p_appDw->v.appVarDwDiverter.divRotTimer = p_appDw->p.appParDwDiverter.div2WRot1to2;
		else if(mcv_rx.pilotDiverter == 0 && p_appDw->v.appVarDwDiverter.divPosReal == 1)
			p_appDw->v.appVarDwDiverter.divRotTimer = p_appDw->p.appParDwDiverter.div2WRot1to0;
		else if(mcv_rx.pilotDiverter == 0 && p_appDw->v.appVarDwDiverter.divPosReal == 2)
			p_appDw->v.appVarDwDiverter.divRotTimer = p_appDw->p.appParDwDiverter.div2WRot2to0;
		else if(mcv_rx.pilotDiverter == 1 && p_appDw->v.appVarDwDiverter.divPosReal == 2)
			p_appDw->v.appVarDwDiverter.divRotTimer = p_appDw->p.appParDwDiverter.div2WRot2to1;


		++p_appDw->v.appVarDwDiverter.divRotCnt;
		if(p_appDw->v.appVarDwDiverter.divRotCnt >= p_appDw->v.appVarDwDiverter.divRotTimer)
		{
			p_appDw->v.appVarDwDiverter.divPosReal = mcv_rx.pilotDiverter;
			p_appDw->v.appVarDwDiverter.divRotCnt = 0;
		}
	}
}

void appDiverterless(appDw_t* p_appDw)
{
	appDiverterInit(&appDw);
	p_appDw->v.appVarDwDiverter.divPosReal = 0;
	p_appDw->v.appVarDwDiverter.divPosRealTx = 0;
	appDwFunc.appDwDiverterReset();
}

void appDiverterNoSelect(appDw_t* p_appDw)
{
	appDiverterInit(&appDw);
	p_appDw->v.appVarDwDiverter.divPosReal = 0;
	p_appDw->v.appVarDwDiverter.divPosRealTx = 0;
	appDwFunc.appDwDiverterReset();
}

void appCheckDiverterType(appDw_t* p_appDw)
{
	if(p_appDw->v.appVarDwDiverter.diverterOldType != p_appDw->v.appVarDwDiverter.diverterType)
		appDiverterReset(&appDw);
}

void appDiverterControl(appDw_t* p_appDw)
{
	appDetectDiverterType(&appDw);
	appCheckDiverterType(&appDw);

	switch(p_appDw->v.appVarDwDiverter.diverterType)
	{
	case DIVERTER_3WAY:
		appDiverter3Way(&appDw);
		break;
	case DIVERTER_2WAY:
		appDiverter2Way(&appDw);
		break;
	case DIVERTERLESS:
		appDiverterless(&appDw);
		break;
	default:
		appDiverterNoSelect(&appDw);

	}
}
