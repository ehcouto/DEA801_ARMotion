/*
 * dw_uart.h
 *
 *  Created on: 8 Ara 2025
 *      Author: 26098107
 */

#ifndef DW_UART_H_
#define DW_UART_H_

#define UART_TORQUE_SCALE   (65536.0f)

void appUARTInit(void);
app_bool_t appIsStateFault(appDw_t* p_appDw);
void appUARTHandler(appDw_t* p_appDw);
void appGetSwVersion(void);
void appRMSCalc(const float* const input, float* const output, const uint32_t numSample, rmsCalcPar_t* rmsCalcObj);
uint8_t appCalcCRC(const uint8_t *data, uint8_t length);

extern uint8_t crc;
extern volatile uint8_t txIndex;

#endif /* DW_UART_H_ */
