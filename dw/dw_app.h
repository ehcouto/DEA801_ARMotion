/*
 * dw_app.h
 *
 *  Created on: 10 Şub 2025
 *      Author: 26098107
 */


#include "app_config.h"

#ifdef ENABLE_DISHWASHER_APPLICATION

#ifndef SOURCE_APP_DW_APP_H_
#define SOURCE_APP_DW_APP_H_

#include "dw_types.h"
#include "dw_app.h"
#include "stdint.h"

void appDwInit(void);
void appDwHandlerFL(void);
void appDwHandlerSL(void);
void appDwHandlerML(void);
void appDwComHandler();
void appResetErrorCountersFlags(appDw_t* p_appDw);
void appResetAllFaultCounters(appDw_t* p_appDw);
void appValveControl(appDw_t* p_appDw);


#endif

#endif /* SOURCE_APP_DW_APP_H_ */
