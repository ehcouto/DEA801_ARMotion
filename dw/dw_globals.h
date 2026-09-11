/*
 * dw_globals.h
 *
 *  Created on: 10 Şub 2025
 *      Author: 26098107
 */
#include "app_config.h"
#include "dw_types.h"
#include "drv.h"
#include "dw_heater.h"
#include "dw_diverter.h"
#include "dw_motors.h"
#include "dw_trans.h"
#include "dw_drainTimer.h"
#include "dw_uart.h"
#include "dw_doorSwitch.h"
#include "dw_faults.h"
#include "mc.h"
#include "dw_app.h"
#include "dw_relays.h"


#ifdef ENABLE_DISHWASHER_APPLICATION

#ifndef SOURCE_APP_DW_GLOBALS_H_
#define SOURCE_APP_DW_GLOBALS_H_

typedef unsigned char sys_t;
#define CIRCULATION    ((sys_t)0x00U)
#define DRAIN	       ((sys_t)0x01U)

#define APP_FALSE       ((app_bool_t)0x0U)
#define APP_TRUE        ((app_bool_t)0x1U)

#define APP_DISABLED    ((app_bool_t)0x0U)
#define APP_ENABLED     ((app_bool_t)0x1U)

#define UART_PREAMBLE_VALUE						(0x55U)

#define AS_BINARY_FLAG_CRC_FAULT 	   		    (0x00080000U)
//#define AS_BINARY_FLAG_SHORT_CIRCUIT			(0x00100000U)
#define AS_BINARY_FLAG_HEATER					(0x00200000U)
#define AS_BINARY_FLAG_DOOR_SWITCH				(0x00400000U)
#define AS_BINARY_FLAG_DIVERTER					(0x00800000U)
#define AS_BINARY_FLAG_UART_FAULT			    (0x01000000U)
#define AS_BINARY_FLAG_RELAY_STUCK				(0x02000000U)

#define CIRC_STOP_RAMP							(5000.0f)
#define DRAIN_STOP_RAMP							(5000.0f)
#define CIRC_NORMAL_RAMP						(500.0f)
#define DRAIN_NORMAL_RAMP						(1000.0f)
#define DRAIN_SLOW_RAMP							(1000.0f)
#define DRAIN_SLOW_RAMP_SPEED					(2500.0f)
#define CIRC_COMMAND_FOR_DIVERTER				(1800.0f)
#define CIRC_RAMP_FOR_DIVERTER					(3000.0f)

#define DIVER_UNKNOWN_POS						(0x6U)
#define DIVER_INIT_POS							(0x7U)

#define SWITCH_HEATER							(0x0U)
#define RELAY_HEATER							(0x1U)

#define WATER_VALVE_BITMASK    					(0x02U)
#define DISPENSER_VALVE_BITMASK     			(0x80U)
#define REGEN_VALVE_BITMASK    					(0x08U)
#define HEATER_VALVE_BITMASK   					(0x04U)
#define CIRCULATION_COMMAND_BITMASK 			(0x20U)
#define DRAIN_COMMAND_BITMASK   				(0x01U)

#define VERSION_MAJOR 	  						(uint8_t)(1U)
#define VERSION_MINOR 	  						(uint8_t)(25U)
#define VERSION_BUGFIX 	  						(uint8_t)(0U)

#define DRAIN_TYPE_BIT_FIELD 					(0x0U)
#define CIRC_TYPE_BIT_FIELD 					(0x2U)
#define DIVERTER_TYPE_BIT_FIELD 				(0x5U)

#define DIVERTER_TYPE_BIT_AREA					(0x7U)
#define CIRC_TYPE_BIT_AREA						(0x7U)
#define DRAIN_TYPE_BIT_AREA						(0x3U)

#define DIVERTER_3WAY_BITMASK   				(0x0U)
#define DIVERTER_2WAY_BITMASK   				(0x1U)
#define DIVERTERLESS_BITMASK 	  				(0x2U)

#define CIRC_W20007335_BITMASK   				(0x0U)
#define CIRC_W11715301_BITMASK   				(0x1U)
#define CIRC_W20016965_BITMASK 	  				(0x2U)
#define CIRC_W11652801_BITMASK 	  				(0x3U)

#define DRAIN_W11402566_BITMASK					(0x0U)
#define DRAIN_W11186148_BITMASK					(0x1U)
#define DRAIN_W11377410_BITMASK					(0x2U)


extern appDwFp_t appDwFunc;
extern McSpeedReq_t refSpeed_M0;
extern McSpeedReq_t refSpeed_M1;


#endif

#endif /* SOURCE_APP_DW_GLOBALS_H_ */



