#ifndef APP_COMMON_H
#define APP_COMMON_H

#include "stdint.h"

#define MS_IN_SEC       1000
#define SEC_IN_MIN      60
#define MIN_IN_HOUR     60


typedef enum
{
    STATE_PASSIVE = 0,
    STATE_ACTIVE
} State_t;

typedef struct
{
    uint32_t u32TimeCounter1ms;
    uint32_t u32TimeCounter1sec;
    uint32_t u32TimeCounter1min;
    uint32_t u32TimeCounter1hour;
} sAwakeTime_t;

extern sAwakeTime_t sAwakeTime;

void sAwakeTimeHandler_ms(sAwakeTime_t *l_sAwakeTime);

#endif
