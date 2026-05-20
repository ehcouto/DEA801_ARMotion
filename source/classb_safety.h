/*
 * classb_safety.h
 *
 *  Created on: 24 Eki 2025
 *      Author: 26098107
 */

#ifndef SAFETY_CLASSB_SAFETY_H_
#define SAFETY_CLASSB_SAFETY_H_

#include "iec60730b_core.h"
#include "safety_cm33_mcx.h"

#include "iec60730b.h"
#include "drv_config.h"
#include "iec60730b_types.h"

#define SAFETY_ADC  				ADC0
#define SAFETY_ADC_RES 				12
#define SAFETY_ADC_REF				3.3
#define SAFETY_ADC_DEV_PERCENT		10
#define SAFETY_ADC_MAX				(1 << (SAFETY_ADC_RES) - 1)
#define SAFETY_ADC_MIN_LIM(val) 	(uint16_t)(((val) * (100 - SAFETY_ADC_DEV_PERCENT)) / 100)
#define SAFETY_ADC_MAX_LIM(val) 	(uint16_t)(((val) * (100 + SAFETY_ADC_DEV_PERCENT)) / 100)


#define BACKUP_ENABLE 				1
#define LOGICAL_ONE		 			1
#define LOGICAL_ZERO				0
#define DIO_SHORT_TO_GND_TEST 		1
#define DIO_SHORT_TO_VDD_TEST 		1

#endif /* SAFETY_CLASSB_SAFETY_H_ */
