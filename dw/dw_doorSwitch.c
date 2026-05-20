/*
 * dw_doorSwitch.c
 *
 *  Created on: 8 Ara 2025
 *      Author: 26098107
 */
#include "dw_globals.h"
#include "drv_config.h"

void appDoorSwitchCheckInit(appDw_t* p_appDw)
{
	p_appDw->v.appVarDwDoorSwitch.faultCntr = 0;
	p_appDw->v.appVarDwDoorSwitch.flag = APP_FALSE;
}

void appDoorSwitchCheck(appDw_t* p_appDw)
{
    if(p_appDw->p.appParDwDoorSwitch.enabled == APP_FALSE)
    {
    	p_appDw->v.appVarDwDoorSwitch.faultCntr = 0u;
        return;
    }
    else
    {
    	if(!(GPIO_PinRead(PIN_IN_12V_DS_GPIO, PIN_IN_12V_DS_GPIO_PIN)))
    	{
            p_appDw->v.appVarDwDoorSwitch.faultCntr++;

            if(p_appDw->v.appVarDwDoorSwitch.faultCntr >
               p_appDw->p.appParDwDoorSwitch.faultCntrMax)
            {

            p_appDw->v.appVarDwDoorSwitch.flag = APP_TRUE;

            //p_appDw->v.generatedFaultsBinary &= 0x00;
            p_appDw->v.generatedFaultsBinary |= AS_BINARY_FLAG_DOOR_SWITCH;

            p_appDw->v.mc_app_state = APP_STATE_FAULT;
            p_appDw->v.appTotalGeneratedFault++;

            p_appDw->v.appFatalFaultFlag = APP_TRUE;
            p_appDw->v.appVarDwDoorSwitch.faultCntr = p_appDw->p.appParDwDoorSwitch.faultCntrMax;

            }
    	}
        else
        {

        	p_appDw->v.appVarDwDoorSwitch.flag = APP_FALSE;
        	//p_appDw->v.appFatalFaultFlag = APP_FALSE;
        	//p_appDw->v.mc_app_state = APP_STATE_NO_FAULT;
        	p_appDw->v.appVarDwDoorSwitch.faultCntr = 0;
        	if(p_appDw->v.generatedFaultsBinary & AS_BINARY_FLAG_DOOR_SWITCH)
        		p_appDw->v.generatedFaultsBinary &= 0x00;

        }
    }
}

