/*
 * dw_diverter.h
 *
 *  Created on: 4 Ara 2025
 *      Author: 26098107
 */

#ifndef DW_DIVERTER_H_
#define DW_DIVERTER_H_

void appDiverterInit(appDw_t* p_appDw);
void appDiverterReset(appDw_t* p_appDw);
void appDetectDiverterType(appDw_t* p_appDw);
void appDiverter3Way(appDw_t* p_appDw);
void appDiverter2Way(appDw_t* p_appDw);
void appDiverterless(appDw_t* p_appDw);
void appDiverterNoSelect(appDw_t* p_appDw);
void appCheckDiverterType(appDw_t* p_appDw);
void appDiverterControl(appDw_t* p_appDw);

#endif /* DW_DIVERTER_H_ */
