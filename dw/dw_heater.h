/*
 * dw_heater.h
 *
 *  Created on: 4 Ara 2025
 *      Author: 26098107
 */

#ifndef DW_HEATER_H_
#define DW_HEATER_H_

void appHeaterInit(appDw_t* p_appDw);
void appHeaterFlagInit(appDw_t* p_appDw);
void appHeaterControl(appDw_t* p_appDw);
void appHeaterTimers(appDw_t* p_appDw);
void appHotHighPotTest(appDw_t* p_appDw);

#endif /* DW_HEATER_H_ */
