#include "system.h"
#include "freemaster.h"
#include "safety_cm33_mcx.h"


void main(void)
{

	osInitSys();

    while (1)
    {
	    osHandlerML();
	    //FMSTR_Poll();

    	//FsRuntimeClockTest();
    	//FsCpuBackgroundTest();
	}
}
