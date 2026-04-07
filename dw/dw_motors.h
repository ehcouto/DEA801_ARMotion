/*
 * dw_motors.h
 *
 *  Created on: 5 Ara 2025
 *      Author: 26098107
 */

#ifndef DW_MOTORS_H_
#define DW_MOTORS_H_

void appResetMotors(void);
void appAddMotors(void);
void appGetCircType(appDw_t* p_appDw);
void appSelectCircMotor(appDw_t* p_appDw);
void appGetDrainType(appDw_t* p_appDw);
void appSelectDrainMotor(appDw_t* p_appDw);



#endif /* DW_MOTORS_H_ */
