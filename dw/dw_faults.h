/*
 * dw_faults.h
 *
 *  Created on: 8 Ara 2025
 *      Author: 26098107
 */

#ifndef DW_FAULTS_H_
#define DW_FAULTS_H_

void appCrcCheck(appDw_t* p_appDw);
void appHeaterCheck(appDw_t* p_appDw);
void appDiverterCheck(appDw_t* p_appDw);
void appFaultControl(appDw_t* p_appDw);
void appCrcCheckInit(appDw_t* p_appDw);

#endif /* DW_FAULTS_H_ */
