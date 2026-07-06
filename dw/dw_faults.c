/*
 * dw_faults.c
 *
 *  Created on: 8 Ara 2025
 *      Author: 26098107
 */
#include "dw_globals.h"
#include "drv_config.h"
#include "mcglobals.h"

static void UART_Reset(void);

volatile uint8_t enDSFlagPWM=0;
volatile uint8_t enCRCFlagPWM=0;

void appCrcCheckInit(appDw_t* p_appDw)
{
	p_appDw->v.appVarDwCrcCheck.faultCntr = 0;
	p_appDw->v.appVarDwCrcCheck.flag = APP_FALSE;

	p_appDw->v.appVarDwUART.faultRXCntr  = 0u;
	p_appDw->v.appVarDwUART.faultTXCntr  = 0u;
	p_appDw->v.appVarDwUART.faultResCntr = 0u;
}

void appCrcCheck(appDw_t* p_appDw)
{
    if(p_appDw->p.appParDwCrcCheck.enabled == APP_FALSE)
    {
    	p_appDw->v.appVarDwCrcCheck.faultCntr = 0u;
        return;
    }
    else
    {
    	if(mcv_rx.CRCRx != mcv_rx.receivedCRCRx)
    	{
            p_appDw->v.appVarDwCrcCheck.faultCntr++;

            if(p_appDw->v.appVarDwCrcCheck.faultCntr >
               p_appDw->p.appParDwCrcCheck.faultCntrMax)
            {

            p_appDw->v.appVarDwCrcCheck.flag = APP_TRUE;

            //p_appDw->v.generatedFaultsBinary &= 0x00;
            p_appDw->v.generatedFaultsBinary |= AS_BINARY_FLAG_CRC_FAULT;

            p_appDw->v.mc_app_state = APP_STATE_FAULT;
            p_appDw->v.appTotalGeneratedFault++;

            p_appDw->v.appFatalFaultFlag = APP_TRUE;
            p_appDw->v.appVarDwCrcCheck.faultCntr = p_appDw->p.appParDwCrcCheck.faultCntrMax;

            }
    	}
        else
        {
        	p_appDw->v.appVarDwCrcCheck.flag = APP_FALSE;
        	//p_appDw->v.appFatalFaultFlag = APP_FALSE;
        	//p_appDw->v.mc_app_state = APP_STATE_NO_FAULT;
        	p_appDw->v.appVarDwCrcCheck.faultCntr = 0;
        	if(p_appDw->v.generatedFaultsBinary & AS_BINARY_FLAG_CRC_FAULT)
        		p_appDw->v.generatedFaultsBinary &= 0x00;
        }

    }
}

void appHeaterCheck(appDw_t* p_appDw)
{
    if(p_appDw->p.appParDwHeater.enabled == APP_FALSE)
    {
    	p_appDw->v.appVarDwHeater.faultCntr = 0u;
        return;
    }
    else
    {
    	p_appDw->v.appVarDwHeater.varHeaterControl.heaterTestFlag = APP_TRUE;

    	if(p_appDw->v.appVarDwHeater.varHeaterControl.heaterTestDone)
    	{
        	if(GPIO_PinRead(PIN_FB_HEATER_GPIO, PIN_FB_HEATER_GPIO_PIN))
        		p_appDw->v.appVarDwHeater.posHeater = 1;
        	else
        		p_appDw->v.appVarDwHeater.posHeater = 0;

        	if(!(p_appDw->v.appVarDwHeater.posHeater))

        	{
                p_appDw->v.appVarDwHeater.faultCntr++;

                if(p_appDw->v.appVarDwHeater.faultCntr >
                   p_appDw->p.appParDwHeater.faultCntrMax)
                {

                p_appDw->v.appVarDwHeater.flag = APP_TRUE;

                //p_appDw->v.generatedFaultsBinary &= 0x00;
                p_appDw->v.generatedFaultsBinary |= AS_BINARY_FLAG_HEATER;

                p_appDw->v.mc_app_state = APP_STATE_FAULT;
                p_appDw->v.appTotalGeneratedFault++;

                p_appDw->v.appFatalFaultFlag = APP_TRUE;
                p_appDw->v.appVarDwHeater.faultCntr = p_appDw->p.appParDwHeater.faultCntrMax;

                p_appDw->v.appVarDwHeater.varHeaterControl.heaterOnCommand = APP_FALSE;

                }
        	}
        	else
        	{
            	p_appDw->v.appVarDwHeater.flag = APP_FALSE;
            	//p_appDw->v.appFatalFaultFlag = APP_FALSE;
            	//p_appDw->v.mc_app_state = APP_STATE_NO_FAULT;
            	p_appDw->v.appVarDwHeater.faultCntr = 0;
            	if(p_appDw->v.generatedFaultsBinary & AS_BINARY_FLAG_HEATER)
            		p_appDw->v.generatedFaultsBinary &= 0x00;
            	p_appDw->v.appVarDwHeater.varHeaterControl.heaterCheckFlag = APP_TRUE;
            	p_appDw->v.appVarDwHeater.varHeaterControl.heaterOnCommand = APP_TRUE;
        	}
    	}
    }
}

void appDiverterCheck(appDw_t* p_appDw)
{
    if(p_appDw->p.appParDwDiverter.enabled == APP_FALSE)
    {
    	p_appDw->v.appVarDwDiverter.faultCntr = 0u;
        return;
    }
    else
    {

    	if((p_appDw->v.appVarDwDiverter.divPosRealTx == DIVER_UNKNOWN_POS)
    			&& ((p_appDw->v.appVarDwDiverter.diverterType == DIVERTER_3WAY)
				|| (p_appDw->v.appVarDwDiverter.diverterType == DIVERTER_2WAY)))
    	{
            p_appDw->v.appVarDwDiverter.faultCntr++;

            if(p_appDw->v.appVarDwDiverter.faultCntr >
               p_appDw->p.appParDwDiverter.faultCntrMax)
            {

            p_appDw->v.appVarDwDiverter.flag = APP_TRUE;

            //p_appDw->v.generatedFaultsBinary &= 0x00;
            p_appDw->v.generatedFaultsBinary |= AS_BINARY_FLAG_DIVERTER;

            p_appDw->v.mc_app_state = APP_STATE_FAULT;
            p_appDw->v.appTotalGeneratedFault++;

            p_appDw->v.appFatalFaultFlag = APP_TRUE;
            p_appDw->v.appVarDwDiverter.faultCntr = p_appDw->p.appParDwDiverter.faultCntrMax;

            }
    	}
    	else
    	{
        	p_appDw->v.appVarDwDiverter.flag = APP_FALSE;
        	p_appDw->v.appVarDwDiverter.faultCntr = 0;
        	if(p_appDw->v.generatedFaultsBinary & AS_BINARY_FLAG_DIVERTER)
        		p_appDw->v.generatedFaultsBinary &= 0x00;
    	}
    }

}

void appUARTCheck(appDw_t* p_appDw)
{
    if(p_appDw->p.appParDwUART.enabled == APP_FALSE)
    {
    	p_appDw->v.appVarDwUART.faultRXCntr = 0u;
    	p_appDw->v.appVarDwUART.faultTXCntr = 0u;
        return;
    }
    else
    {
        p_appDw->v.appVarDwUART.faultRXCntr++;
        p_appDw->v.appVarDwUART.faultTXCntr++;

        // RX Checking
    	if(p_appDw->v.appVarDwUART.faultRXCntr >
        p_appDw->p.appParDwUART.faultCntrMax)
    	{
    		//Reset the UART Peripheral
    		if(p_appDw->v.appVarDwUART.flag == APP_FALSE)
    		{
    			UART_Reset(); //Reset only for the first time the fault is raised after normal operation
    		}

    		p_appDw->v.appVarDwUART.flag = APP_TRUE;

    		//p_appDw->v.generatedFaultsBinary &= 0x00;
    		p_appDw->v.generatedFaultsBinary |= AS_BINARY_FLAG_UART_FAULT;

    		p_appDw->v.mc_app_state = APP_STATE_FAULT;
    		p_appDw->v.appTotalGeneratedFault++;

    		p_appDw->v.appFatalFaultFlag = APP_TRUE;

    		p_appDw->v.appVarDwUART.faultRXCntr = p_appDw->p.appParDwUART.faultCntrMax;

    		p_appDw->v.appVarDwUART.faultResCntr = 0;
    	}

    	// TX Checking
    	else if(p_appDw->v.appVarDwUART.faultTXCntr >
        		p_appDw->p.appParDwUART.faultCntrMax)
    	{
    		//Reset the UART Peripheral
    		if(p_appDw->v.appVarDwUART.flag == APP_FALSE)
    		{
    			UART_Reset(); // This function is called on the first occurrence of a fault after normal operation.
    		}

    		p_appDw->v.appVarDwUART.flag = APP_TRUE;

    		//p_appDw->v.generatedFaultsBinary &= 0x00;
    		p_appDw->v.generatedFaultsBinary |= AS_BINARY_FLAG_UART_FAULT;

    		p_appDw->v.mc_app_state = APP_STATE_FAULT;
    		p_appDw->v.appTotalGeneratedFault++;

    		p_appDw->v.appFatalFaultFlag = APP_TRUE;

    		p_appDw->v.appVarDwUART.faultTXCntr = p_appDw->p.appParDwUART.faultCntrMax;

    		p_appDw->v.appVarDwUART.faultResCntr = 0;
    	}
    	else
    	{
    		if(p_appDw->v.appVarDwUART.faultResCntr < p_appDw->p.appParDwUART.faultResMax)
    		{
    			p_appDw->v.appVarDwUART.faultResCntr++;
    		}
    		else
    		{
    			p_appDw->v.appVarDwUART.faultResCntr = p_appDw->p.appParDwUART.faultResMax;
    			p_appDw->v.appVarDwUART.flag = APP_FALSE;
    			if(p_appDw->v.generatedFaultsBinary & AS_BINARY_FLAG_UART_FAULT)
    				p_appDw->v.generatedFaultsBinary &= 0x00;
    		}
    	}
    }
}

void appDrainCurrentCheck(appDw_t* p_appDw)
{
	float _iamp2;

    if(p_appDw->p.appParDwDRCurrCheck.enabled == APP_FALSE)
    {
    	p_appDw->v.appVarDwDRCurrCheck.faultCntr = 0u;
        return;
    }
    else
    {
    	_iamp2 = mpv[DRAIN].v.iamp2;

    	if(_iamp2 <= p_appDw->p.appParDwDRCurrCheck.minCurr)
    	{
    		p_appDw->v.appVarDwDRCurrCheck.errorCntr++;
    		if(p_appDw->v.appVarDwDRCurrCheck.errorCntr >= p_appDw->p.appParDwDRCurrCheck.errorCntrMax)
    		{
    			p_appDw->v.appVarDwDRCurrCheck.flag = APP_TRUE;

                p_appDw->v.generatedFaultsBinary |= AS_BINARY_FLAG_RELAY_STUCK;

                p_appDw->v.mc_app_state = APP_STATE_FAULT;
                p_appDw->v.appTotalGeneratedFault++;
                p_appDw->v.appVarDwDRCurrCheck.faultCntr++;

                if(p_appDw->v.appVarDwDRCurrCheck.faultCntr >= p_appDw->p.appParDwDRCurrCheck.faultCntrMax)
                {
                	p_appDw->v.appFatalFaultFlag = APP_TRUE;
                }
                p_appDw->v.appVarDwDRCurrCheck.faultCntr = p_appDw->p.appParDwDRCurrCheck.faultCntrMax;
    		}
    	}
        else
        {
        	if(p_appDw->v.appVarDwDRCurrCheck.faultCntr)
        	{
        		p_appDw->v.appVarDwDRCurrCheck.faultCntr--;
        	}
        }

    }
}


void appFaultControl(appDw_t* p_appDw)
{
	appUARTCheck(&appDw);
	appDiverterCheck(&appDw);
	appDoorSwitchCheck(&appDw);
	appRelaysCheck(&appDw);
	if(p_appDw->v.activeState == DRAIN)
		appDrainCurrentCheck(&appDw);


	if((mcv_rx.pilotValves & HEATER_VALVE_BITMASK)
			&& !p_appDw->v.appVarDwHeater.varHeaterControl.heaterCheckFlag
			&& !p_appDw->v.appVarDwHeater.varHeaterControl.heaterOnFlag)
		appHeaterCheck(&appDw);
	else
	{
		/*
    	if(p_appDw->v.generatedFaultsBinary & AS_BINARY_FLAG_HEATER)
    		p_appDw->v.generatedFaultsBinary &= 0x00;
    	*/
	}


	if(p_appDw->v.appVarDwCrcCheck.flag == APP_TRUE)
	{
		mcv_rx.spRefM1Rx = 0;
		mcv_rx.spRefM2Rx = 0;
		mcv_rx.pilotValves = 0;
		appDwFunc.appDwDiverterReset();
	}
	else
	{
		;
	}

	if(p_appDw->v.appVarDwUART.flag == APP_TRUE)
	{
		mcv_rx.spRefM1Rx = 0;
		mcv_rx.spRefM2Rx = 0;
		mcv_rx.pilotValves = 0;
		appDwFunc.appDwDiverterReset();
	}
	else
	{
		;
	}

	if(p_appDw->v.appVarDwDoorSwitch.flag == APP_TRUE)
	{
		mcv_rx.spRefM1Rx = 0;
		mcv_rx.pilotValves = DRAIN_COMMAND_BITMASK;
	}
	else
	{
		;
	}

	if(p_appDw->v.appVarDwRelays.flag == APP_TRUE)
	{
		mcv_rx.spRefM1Rx = 0;
		mcv_rx.spRefM2Rx = 0;
		mcv_rx.pilotValves = 0;
		appDwFunc.appDwDiverterReset();
	}
	else
	{
		;
	}
	if(p_appDw->v.appVarDwDiverter.flag == APP_TRUE)
	{
		appDwFunc.appDwDiverterReset();
		p_appDw->v.appVarDwDiverter.diverCommand = APP_FALSE;
		p_appDw->v.appVarDwDiverter.posDivCnt = 0;
		p_appDw->v.appVarDwDiverter.divStopCnt = 0;
		p_appDw->v.appVarDwDiverter.divRotCnt = 0;
		p_appDw->v.appVarDwDiverter.divPosRealTx = p_appDw->v.appVarDwDiverter.divPosReal;

	}
	else
	{
		;
	}

	if(p_appDw->v.appVarDwDRCurrCheck.flag == APP_TRUE)
	{
		mcv_rx.spRefM1Rx = 0;
		mcv_rx.spRefM2Rx = 0;
		mcv_rx.pilotValves = 0;
		appDwFunc.appDwDiverterReset();
	}
	else
	{
		;
	}

}



void UART_Reset(void)
{
	// Disable Receiver & Transmitter
	LPUART4->CTRL &= ~(LPUART_CTRL_TE_MASK | LPUART_CTRL_RE_MASK);

	// Before Resetting the module, disable the interrupts
	LPUART_DisableInterrupts(LPUART4, kLPUART_TxDataRegEmptyInterruptEnable);
	LPUART_DisableInterrupts(LPUART4, kLPUART_RxDataRegFullInterruptEnable);

	// Reset LPUART4
	LPUART_SoftwareReset(LPUART4);

	// Re-initialize UART
	InitUART();

	//Reset Variables
	appUARTInit();
}
