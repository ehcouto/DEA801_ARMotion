/**
*
*
*/

#ifndef SYSTEM_H
#define SYSTEM_H

#include "brd.h"
#include "mc.h"
#include "app.h"
#ifdef ENABLE_AIRCON_APPLICATION
#include "pfc.h"
#endif

typedef struct{
    unsigned counterFL;
    unsigned counterSL;
    unsigned counterML;
    
    unsigned frequencyFL;
    unsigned frequencySL;
} os_t;


/******************************************************************************/
/* function declerations */

void osInitSys(void);
void osHandlerML(void);

/* function declerations */
/******************************************************************************/

extern mcInit_t mcInit;
extern mcMpvInit_t mcMpvInit1;
extern mcMpvInit_t mcMpvInit2;
#endif
