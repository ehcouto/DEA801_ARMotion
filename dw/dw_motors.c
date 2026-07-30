/*
 * dw_motors.c
 *
 *  Created on: 5 Ara 2025
 *      Author: 26098107
 */
#include "system.h"
#include "mcglobals.h"
#include "mc.h"
#include "dw_globals.h"

void appResetMotors(void)
{
	mcDisableMotorControl(CIRCULATION);
	mcDisableMotorControl(DRAIN);

	mcInitMotorControl(&mcInit);
}

void appAddMotors(void)
{
	mcAddMotor(&mcMpvInit1);
	mcAddMotor(&mcMpvInit2);

	//Reset MC State Machine
	mpv[CIRCULATION].v.state = JUST_POWERED;
	mpv[DRAIN].v.state       = JUST_POWERED;

	mcEnableMotorControl(CIRCULATION);
	mcEnableMotorControl(DRAIN);
}

void appGetCircType(appDw_t* p_appDw)
{
	p_appDw->v.circOldType = p_appDw->v.circType;

	p_appDw->v.circDesiredType = (mcv_rx.selectComponent >> CIRC_TYPE_BIT_FIELD) & CIRC_TYPE_BIT_AREA ;

	if(p_appDw->v.circDesiredType == CIRC_W20007335_BITMASK)
		p_appDw->v.circType = CIRC_W20007335;

	else if(p_appDw->v.circDesiredType == CIRC_W11715301_BITMASK)
		p_appDw->v.circType = CIRC_W11715301;

	else if(p_appDw->v.circDesiredType == CIRC_W20016965_BITMASK)
		p_appDw->v.circType = CIRC_W20016965;

	else if(p_appDw->v.circDesiredType == CIRC_W11652801_BITMASK)
		p_appDw->v.circType = CIRC_W11652801;

	else
	{
		;
	}

}

void appSelectCircMotor(appDw_t* p_appDw)
{
	if((p_appDw->v.circOldType != p_appDw->v.circType)
			&& !mcGetActSpeed(CIRCULATION))
	{
		appResetMotors();

		switch(p_appDw->v.circType)
		{
		case CIRC_W20007335:
			mcp_px[CIRCULATION] = &mcp_dw_circ_W20007335;
			fsp_px[CIRCULATION] = &fsp_dw_circ_W20007335;
			break;

		case CIRC_W11715301:
			mcp_px[CIRCULATION] = &mcp_dw_circ_W11715301;
			fsp_px[CIRCULATION] = &fsp_dw_circ_W20007335;
			break;

		case CIRC_W20016965:
			mcp_px[CIRCULATION] = &mcp_dw_circ_W20016965;
			fsp_px[CIRCULATION] = &fsp_dw_circ_W20007335;
			break;

		case CIRC_W11652801:
			mcp_px[CIRCULATION] = &mcp_dw_circ_W11652801;
			fsp_px[CIRCULATION] = &fsp_dw_circ_W11652801;
			break;

		default:
			mcp_px[CIRCULATION] = &mcp_dw_circ_W20007335;
			fsp_px[CIRCULATION] = &fsp_dw_circ_W20007335;

		}
		appAddMotors();

	}
	else
	{
		;
	}

}

void appGetDrainType(appDw_t* p_appDw)
{
	p_appDw->v.drainOldType = p_appDw->v.drainType;

	p_appDw->v.drainDesiredType = (mcv_rx.selectComponent >> DRAIN_TYPE_BIT_FIELD) & DRAIN_TYPE_BIT_AREA ;

	if(p_appDw->v.drainDesiredType == DRAIN_W11402566_BITMASK)
		p_appDw->v.drainType = DRAIN_W11402566;

	else if(p_appDw->v.drainDesiredType == DRAIN_W11186148_BITMASK)
		p_appDw->v.drainType = DRAIN_W11186148;

	else if(p_appDw->v.drainDesiredType == DRAIN_W11377410_BITMASK)
		p_appDw->v.drainType = DRAIN_W11377410;

	else
	{
		;
	}
}

void appSelectDrainMotor(appDw_t* p_appDw)
{
	if((p_appDw->v.drainOldType != p_appDw->v.drainType)
			&& !mcGetActSpeed(DRAIN))
	{
		appResetMotors();

		switch(p_appDw->v.drainType)
		{
		case DRAIN_W11402566:
			mcp_px[DRAIN] = &mcp_dw_drain_W11402566;
			fsp_px[DRAIN] = &fsp_dw_drain_W11402566;
			break;

		case DRAIN_W11186148:
			mcp_px[DRAIN] = &mcp_dw_drain_W11186148;
			fsp_px[DRAIN] = &fsp_dw_drain_W11402566;
			break;

		case DRAIN_W11377410:
			mcp_px[DRAIN] = &mcp_dw_drain_W11377410;
			fsp_px[DRAIN] = &fsp_dw_drain_W11377410;
			break;

		default:
			mcp_px[DRAIN] = &mcp_dw_drain_W11402566;
			fsp_px[DRAIN] = &fsp_dw_drain_W11402566;

		}
		appAddMotors();

	}
	else
	{
		;
	}

}

