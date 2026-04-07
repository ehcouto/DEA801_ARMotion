/*
 * dw_relays.c
 *
 *  Created on: 18 Oca 2026
 *      Author: 26098107
 */

#include "dw_globals.h"
#include "drv_config.h"

void appRelaysCheckInit(appDw_t* p_appDw)
{
	p_appDw->v.appVarDwRelays.faultCntr = 0;
	p_appDw->v.appVarDwRelays.flag = APP_FALSE;
}

void appRelaysCheck(appDw_t* p_appDw)
{
    if(p_appDw->p.appParDwRelays.enabled == APP_FALSE)
    {
    	p_appDw->v.appVarDwRelays.faultCntr = 0u;
        return;
    }
    else
    {
    	if((p_appDw->v.activeState == CIRCULATION) && (adcValSyncPin < p_appDw->p.appParDwRelays.adcValSyncMax)
    			|| (p_appDw->v.activeState == DRAIN) && (adcValSyncPin > p_appDw->p.appParDwRelays.adcValSyncMax))
    	{
            p_appDw->v.appVarDwRelays.faultCntr++;

            if(p_appDw->v.appVarDwRelays.faultCntr >
               p_appDw->p.appParDwRelays.faultCntrMax)
            {

            p_appDw->v.appVarDwRelays.flag = APP_TRUE;

            //p_appDw->v.generatedFaultsBinary &= 0x00;
            p_appDw->v.generatedFaultsBinary |= AS_BINARY_FLAG_RELAY_STUCK;

            p_appDw->v.mc_app_state = APP_STATE_FAULT;
            p_appDw->v.appTotalGeneratedFault++;

            p_appDw->v.appFatalFaultFlag = APP_TRUE;
            p_appDw->v.appVarDwRelays.faultCntr = p_appDw->p.appParDwRelays.faultCntrMax;

            }
    	}
        else
        {

        	p_appDw->v.appVarDwRelays.flag = APP_FALSE;
        	//p_appDw->v.appFatalFaultFlag = APP_FALSE;
        	//p_appDw->v.mc_app_state = APP_STATE_NO_FAULT;
        	p_appDw->v.appVarDwRelays.faultCntr = 0;
        	if(p_appDw->v.generatedFaultsBinary & AS_BINARY_FLAG_RELAY_STUCK)
        		p_appDw->v.generatedFaultsBinary &= 0x00;

        }
    }
}


