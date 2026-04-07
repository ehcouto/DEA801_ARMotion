/*
 * dw_drainTimer.c
 *
 *  Created on: 8 Ara 2025
 *      Author: 26098107
 */

#include "dw_globals.h"
uint32_t actSpDrain;

void appDrainTimer(appDw_t* p_appDw)
{
	if(refSpeed_M1.mc_sprefmec)
	{
		++p_appDw->v.drainWorkTime;
		p_appDw->v.drainWindowFlag = APP_TRUE;
	}
	else if(p_appDw->v.drainWindowFlag)
		++p_appDw->v.drainWaitTime;


	if(p_appDw->v.drainWorkTime >= p_appDw->p.drainMaxWorkTime)
	{
		p_appDw->v.drainReadyFlag = APP_FALSE;
		if(p_appDw->v.drainWaitTime >= p_appDw->p.drainMaxWaitTime)
		{
			p_appDw->v.drainReadyFlag = APP_TRUE;
			p_appDw->v.drainWorkTime = 0;
			p_appDw->v.drainWaitTime = 0;
		}
	}

	if(p_appDw->v.drainWindowFlag)
		++p_appDw->v.drainWindowTime;

	if(p_appDw->v.drainWindowTime > p_appDw->p.drainMaxWindowTime)

	{
		p_appDw->v.drainWaitTime = 0;
		p_appDw->v.drainWorkTime = 0;
		p_appDw->v.drainWindowTime = 0;

	}

}
