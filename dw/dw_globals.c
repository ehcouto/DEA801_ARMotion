/*
 * dw_globals.c
 *
 *  Created on: 10 Şub 2025
 *      Author: 26098107
 */

#include "app_config.h"

#ifdef ENABLE_DISHWASHER_APPLICATION

#include "dw_globals.h"
#include "drv.h"

appDw_t appDw;


const appDwPar_t dwParam = {
		.transReqTime = 2000,	  /* Required time for motor transition in msec */
		.transRelReqTime = 500,  /* Required time for transition relay to open in msec */
		.drainMaxWorkTime = 150000,   /* Maximum permitted operating time of the drain motor in msec */
		.drainMaxWaitTime = 90000,
		.drainMaxWindowTime = 240000,
		.appSafetyReqTime = 5000, /* Required time for wait in application fault in msec */
		.appParDwDiverter = {
				.enabled = APP_TRUE,
				.faultCntrMax = 60000u,
				.divAdcLow = 25000,//27500,
				.divPosNum = 6,
				.div1TurnTime = 2375,//2500,
				.divHighMax = 2400,
				.divHighMin = 1400,
				.divLowMax = 650,
				.divLowMin = 350,
				.divPos0HighMax = 1400,
				.divPos0HighMin = 1000,
				.divPos0LowMax = 1400,
				.divPos0LowMin = 1000,
				.div2WPos0HighMax = 1850,
				.div2WPos0HighMin = 1450,
				.div2WPos0LowMax = 2300,
				.div2WPos0LowMin = 1900,
				.div2WRot0to1 = 3750,
				.div2WRot0to2 = 11250,
				.div2WRot1to2 = 7500,
				.div2WRot1to0 = 11250,
				.div2WRot2to0 = 3750,
				.div2WRot2to1 = 7500,
		},
		.appParDwHeater = {
				.enabled = APP_TRUE,
				.faultCntrMax = 500u,
				.counterMaxHHP = 7000u,
				.parHeaterControl = {
						.heaterOffWaitCntLimit = 1000,
						.heaterWaitCntLimit = 1000,
						.relaySwTime = 200,
				},
		},
		.appParDwCrcCheck = {
				.enabled = APP_TRUE,
				.faultCntrMax = 10u,
		},
		.appParDwDoorSwitch = {
				.enabled = APP_TRUE,
				.faultCntrMax = 50u,
		},
		.appParDwUART = {
				.enabled = APP_TRUE,
				.faultCntrMax = 1000u,
				.faultResMax = 1000u,
		},
		.appParDwRelays = {
				.enabled = APP_TRUE,
				.faultCntrMax = 1000u,
				.adcValSyncMax = 10000u,
		},
		.appParDwDRCurrCheck = {
				.enabled = APP_FALSE,
				.minCurr = 0.00873f,
				.errorCntrMax = 5000u,
				.faultCntrMax = 4u,
		},
		.noFaultCntrMax = 2000,
};

const appDwPar_t *const dwp_px = {
		&dwParam,
};

#endif

