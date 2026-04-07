#include "app_common.h"

sAwakeTime_t sAwakeTime;

void sAwakeTimeHandler_ms(sAwakeTime_t *l_sAwakeTime)
{
    l_sAwakeTime->u32TimeCounter1ms++;
    if(l_sAwakeTime->u32TimeCounter1ms >= MS_IN_SEC)
    {
        l_sAwakeTime->u32TimeCounter1ms = 0;
        l_sAwakeTime->u32TimeCounter1sec++;
        
        if(l_sAwakeTime->u32TimeCounter1sec >= SEC_IN_MIN)
        {
            l_sAwakeTime->u32TimeCounter1sec = 0;
            l_sAwakeTime->u32TimeCounter1min++;
            
            if(l_sAwakeTime->u32TimeCounter1min >= MIN_IN_HOUR)
            {
                l_sAwakeTime->u32TimeCounter1min = 0;
                l_sAwakeTime->u32TimeCounter1hour++;
            }
            else
            {
                ;       /* do nothing */
            }
        }
        else
        {
            ;       /* do nothing */
        }
    }
    else
    {
        ;       /* do nothing */
    }
}
