/**
* @file app.c
* @author Serkan BUYUKABALI
* @date 07.09.2020
* @brief File includes related definitions for application layer
* 
* This file is created to provide a source code for the application specific definition to application layer.
* The other way provided source codes for the motor control applications can be all included in this file and 
* desired application source code can be selected with preprocessor statements.
* 
* @see https://rd-repo.arcelik.com/PE_WG-MotorTech/MCP
*/

#include "app.h"

#ifdef ENABLE_REFRIGERATOR_APPLICATION
#include "ref_app.h"
#elif defined(ENABLE_WM_APPLICATION)
#include "wm_app.h"
#elif defined(ENABLE_DRYER_APPLICATION)
#include "dryer_app.h"
#elif defined(ENABLE_AIRCON_APPLICATION)
#include "rac_app.h"
#elif defined(ENABLE_DISHWASHER_APPLICATION)
#include "dw_app.h"
#endif
#ifdef MKV42F16
#include "freemaster.h"
#endif

app_t app;

/**
* @brief Application layer initialization function
* 
* Function will load all initial values of parameters and variables about application layer. All related 
* sublayers or process related to application will be initialized in this function body.* 
*/
void appInitApp(void)
{
    app.counterFL = app.counterSL = app.counterML = 0;  
    
#ifdef MKV42F16
    FMSTR_Init();
#endif

#ifdef ENABLE_WM_APPLICATION
    appWmInitApp();
#elif defined(ENABLE_DRYER_APPLICATION)
    appDryerInitApp();
#elif defined(ENABLE_AIRCON_APPLICATION)
    appRacInit();
#elif defined(ENABLE_REFRIGERATOR_APPLICATION)
    appRefrigeratorInitApp(0);
#elif defined(ENABLE_DISHWASHER_APPLICATION)
    appDwInit();
#endif
}

/**
* @brief Application layer fast loop handler function
* 
* Function will handle application related functions in fast loop (Highest frequency handler)
*/
void appHandlerFL(void)
{
    app.counterFL++;
    
#ifdef ENABLE_WM_APPLICATION
    appWmHandlerFL();
#elif defined(ENABLE_DRYER_APPLICATION)
    appDryerHandlerFL();
#elif defined(ENABLE_REFRIGERATOR_APPLICATION)
    appRefrigeratorHandlerFL();
#endif

#ifdef MKV42F16
    FMSTR_Recorder();
#endif
}

/**
* @brief Application layer slow loop handler function
* 
* Function will handle application related functions in slow loop (Highest frequency handler)
*/
void appHandlerSL(void)
{
    app.counterSL++;
    
    sAwakeTimeHandler_ms(&sAwakeTime);
    
#ifdef ENABLE_WM_APPLICATION
    appWmHandlerSL();
#elif defined(ENABLE_DRYER_APPLICATION)
    appDryerHandlerSL();
#elif defined(ENABLE_AIRCON_APPLICATION)
    appRacHandlerSL();
#elif defined(ENABLE_REFRIGERATOR_APPLICATION)
    appRefrigeratorHandlerSL();
#elif defined(ENABLE_DISHWASHER_APPLICATION)
    appDwHandlerSL();
#endif    
}

/**
* @brief Application layer main loop handler function
* 
* Function will handle application related functions in main loop
*/
void appHandlerML(void)
{
    app.counterML++;
    
#ifdef MKV42F16
    FMSTR_Poll();
#endif
    
#ifdef ENABLE_WM_APPLICATION
    appWmHandlerML();
#elif defined(ENABLE_DRYER_APPLICATION)
    appDryerHandlerML();
#elif defined(ENABLE_AIRCON_APPLICATION)
    appRacHandlerML();
#elif defined(ENABLE_REFRIGERATOR_APPLICATION)
    appRefrigeratorHandlerML();
#elif defined(ENABLE_DISHWASHER_APPLICATION)
    appDwHandlerML();
#endif
}

void appHandlerCom(void)
{
#ifdef ENABLE_WM_APPLICATION
    appWmComIntHandler();
#elif defined(ENABLE_DRYER_APPLICATION)
    appDryerComIntHandler();
#elif defined(ENABLE_AIRCON_APPLICATION)
    appRac2msHandler();
#elif defined(ENABLE_REFRIGERATOR_APPLICATION)
    appRefrigeratorComHandler();
#elif defined(ENABLE_DISHWASHER_APPLICATION)
    //appDwComHandler();
#endif
}


