/**
*
*
*/

#ifndef APP_H
#define APP_H

#include "app_config.h"
#include "app_common.h"

typedef struct{
    unsigned counterFL;
    unsigned counterSL;
    unsigned counterML;
} app_t;


void appInitApp(void);
void appHandlerFL(void);
void appHandlerSL(void);
void appHandlerML(void);
void appHandlerCom(void);

#endif
