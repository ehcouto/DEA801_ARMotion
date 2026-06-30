/*
 * dw_uart.c
 *
 *  Created on: 8 Ara 2025
 *      Author: 26098107
 */

#include "dw_globals.h"
#include "drv_config.h"
#include "math.h"

mcv_rx_uart mcv_rx;
mcv_tx_uart mcv_tx;

uint8_t crc = 0x00;
volatile uint8_t txIndex = 0;
volatile uint8_t rxIdxAcu = 0;


void appUARTInit(void)
{
	mcv_rx.CRCRx = 0;
	mcv_rx.pilotDiverter = 0;
	mcv_rx.commandDiverter = 0;
	mcv_rx.pilotValves = 0;
	mcv_rx.preambleByteRx = 0;
	mcv_rx.receivedCRCRx = 0;
	mcv_rx.rxComplete = false;
	mcv_rx.rxIndex = 0;
	mcv_rx.spRefCirc = 0;
	mcv_rx.spRefDrain = 0;
	mcv_rx.spRefM1Rx = 0;
	mcv_rx.spRefM2Rx = 0;
	mcv_rx.pilotValvesRx = 0;

	mcv_tx.CRCTx = 0;
	mcv_tx.diverterFbTx = DIVER_UNKNOWN_POS;
	mcv_tx.faultTx = 0;
	mcv_tx.preambleByteTx = (app_uint8_t)UART_PREAMBLE_VALUE;
	mcv_tx.spEstM1Tx = 0;
	mcv_tx.spEstM2Tx = 0;
	mcv_tx.txReady = true;
	mcv_tx.resultHHPTest = 0;
	for(uint8_t i; i<4; i++)
	{
		mcv_tx.torqueVal[i] = 0;
	}

	drvUart_Reset();
}

app_bool_t appIsStateFault(appDw_t* p_appDw)
{
	if((p_appDw->v.appVarDwDoorSwitch.flag == APP_TRUE) ||
			(p_appDw->v.appVarDwHeater.flag == APP_TRUE) ||
			(p_appDw->v.appVarDwCrcCheck.flag == APP_TRUE) ||
			(p_appDw->v.appVarDwDiverter.flag == APP_TRUE) ||
			(p_appDw->v.appVarDwUART.flag == APP_TRUE) ||
			(p_appDw->v.appVarDwRelays.flag == APP_TRUE) ||
			(p_appDw->v.appVarDwDRCurrCheck.flag == APP_TRUE))
	{
		return APP_TRUE;
	}
    else
    {
        return APP_FALSE;
    }

}

void appUARTHandler(appDw_t* p_appDw)
{
	uint32_t fault = 0;

	//Load TX Buffer
	if (mcv_tx.txReady)
	{
		mcv_tx.txReady = false;

		if((mcv_rx.pilotValves & CIRCULATION_COMMAND_BITMASK) && mcv_rx.spRefM1Rx)
		{
			for(uint8_t i = 0; i<4; i++)
			{
				mcv_tx.torqueVal[i] = (uint16_t)(torqueCirc[i]*UART_TORQUE_SCALE);
			}
		}
		else if((mcv_rx.pilotValves & DRAIN_COMMAND_BITMASK)  && mcv_rx.spRefM2Rx)
		{
			for(uint8_t i = 0; i<4; i++)
			{
				mcv_tx.torqueVal[i] = (uint16_t)(torqueDrain[i]*UART_TORQUE_SCALE);
			}
		}
		else
		{
			for(uint8_t i = 0; i<4; i++)
			{
				mcv_tx.torqueVal[i] = 0;
			}
		}

		for(uint8_t i = 0; i<4; i++)
		{
			if(mcv_tx.torqueVal[i] >= 65535)
			{
				mcv_tx.torqueVal[i] = 65535;
			}
		}


		p_appDw->v.fsCircFaultFlag = (app_bool_t)mcIsStateFault(CIRCULATION);
		p_appDw->v.fsDrainFaultFlag = (app_bool_t)mcIsStateFault(DRAIN);
		p_appDw->v.appFaultFlag = (app_bool_t)appIsStateFault(p_appDw);

		if((p_appDw->v.fsCircFaultFlag) &&
		   (p_appDw->v.activeState == CIRCULATION))
		{
			fault |= mcGetGeneratedFaultsBinary(CIRCULATION);
		}
		if((p_appDw->v.fsDrainFaultFlag) &&
		   (p_appDw->v.activeState == DRAIN))
		{
			fault |= mcGetGeneratedFaultsBinary(DRAIN);
		}
		if(p_appDw->v.appFaultFlag)
		{
			fault |= p_appDw->v.generatedFaultsBinary;
		}

		mcv_tx.faultTx = fault;

		mcv_tx.diverterFbTx = p_appDw->v.appVarDwDiverter.divPosRealTx;

		mcv_tx.spEstM1Tx = (uint16_t)mcGetActSpeed(CIRCULATION);

		mcv_tx.spEstM2Tx = (uint16_t)mcGetActSpeed(DRAIN);

		mcv_tx.preambleByteTx = (app_uint8_t)UART_PREAMBLE_VALUE;

		appGetSwVersion();

		if(p_appDw->v.appVarDwHeater.flagDoneHHP)
			mcv_tx.resultHHPTest = 1;
		else
			mcv_tx.resultHHPTest = 0;

		txBuffer[0] = mcv_tx.preambleByteTx;
		txBuffer[1] = mcv_tx.faultTx & 0xFF;
		txBuffer[2] = mcv_tx.faultTx >> 8;
		txBuffer[3] = mcv_tx.faultTx >> 16;
		txBuffer[4] = mcv_tx.faultTx >> 24;
		txBuffer[5] = mcv_tx.diverterFbTx;
		txBuffer[6] = mcv_tx.spEstM1Tx & 0xFF;
		txBuffer[7] = mcv_tx.spEstM1Tx >> 8;
		txBuffer[8] = mcv_tx.spEstM2Tx & 0xFF;
		txBuffer[9] = mcv_tx.spEstM2Tx >> 8;
		txBuffer[10] = mcv_tx.torqueVal[0] & 0xFF;
		txBuffer[11] = mcv_tx.torqueVal[0] >> 8;
		txBuffer[12] = mcv_tx.torqueVal[1] & 0xFF;
		txBuffer[13] = mcv_tx.torqueVal[1] >> 8;
		txBuffer[14] = mcv_tx.torqueVal[2] & 0xFF;
		txBuffer[15] = mcv_tx.torqueVal[2] >> 8;
		txBuffer[16] = mcv_tx.torqueVal[3] & 0xFF;
		txBuffer[17] = mcv_tx.torqueVal[3] >> 8;
		txBuffer[18] = mcv_tx.packedVersion & 0xFF;
		txBuffer[19] = (mcv_tx.packedVersion >> 8) & 0xFF;
		txBuffer[20] = mcv_tx.resultHHPTest;
		++rxIdxAcu;
		if(rxIdxAcu >= 255)
			rxIdxAcu = 0;
		txBuffer[21] = rxIdxAcu;

		mcv_tx.CRCTx = appCalcCRC(txBuffer, (TX_BUFFER_SIZE-1)*sizeof(uint8_t));
		txBuffer[22] = mcv_tx.CRCTx;

		txIndex = 0;
		LPUART_EnableInterrupts(LPUART4, kLPUART_TxDataRegEmptyInterruptEnable);
	}

	//Receive RX buffer
	if (mcv_rx.rxComplete)
	{
		mcv_rx.receivedCRCRx = rxBuffer[11];
		mcv_rx.CRCRx = appCalcCRC(rxBuffer,(RX_BUFFER_SIZE-1)*sizeof(uint8_t));

		if(mcv_rx.receivedCRCRx == mcv_rx.CRCRx)
		{
			mcv_rx.preambleByteRx = rxBuffer[0];
			mcv_rx.commandDiverter = rxBuffer[1];
			mcv_rx.pilotValvesRx = rxBuffer[2];
			mcv_rx.spRefCirc = (rxBuffer[4]<<8) | (rxBuffer[3]);
			mcv_rx.spRefDrain = (rxBuffer[6]<<8) | (rxBuffer[5]);
			mcv_rx.selectComponent = rxBuffer[7];
			mcv_rx.heaterFirstCfg = rxBuffer[8];
			mcv_rx.rsvByte1 = rxBuffer[9];
			mcv_rx.rsvByte2 = rxBuffer[10];
			mcv_rx.pilotValves = mcv_rx.pilotValvesRx;
		}

		mcv_rx.rxComplete = false;
		appCrcCheck(&appDw);
	}
}

void appGetSwVersion(void)
{
	mcv_tx.packedVersion &= (0x00);

	mcv_tx.packedVersion |= (VERSION_MAJOR & 0x1F);
	mcv_tx.packedVersion |= ((VERSION_MINOR & 0x1F) << 5);
	mcv_tx.packedVersion |= ((VERSION_BUGFIX & 0x3F) << 10);

}

void appRMSCalc(const float* const input, float* const output, const uint32_t numSample, rmsCalcPar_t* rmsCalcObj)
{
	++rmsCalcObj->rmsCounter;
	rmsCalcObj->current2 = (*input)*(*input);
	rmsCalcObj->sum2 += rmsCalcObj->current2;

    if((rmsCalcObj->rmsCounter) >= numSample)
    {
    	rmsCalcObj->sum2 /= (rmsCalcObj->rmsCounter);
    	*output = sqrtf(rmsCalcObj->sum2);
    	rmsCalcObj->sum2 = 0.0f;
    	rmsCalcObj->rmsCounter = 0;
    }
}

uint8_t appCalcCRC(const uint8_t *data, uint8_t length)
{
    crc = 0x00;

    for (uint8_t i = 0; i < length; i++) {
        crc = crcTable[crc ^ data[i]];
    }
    return crc;
}
