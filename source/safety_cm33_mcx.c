/*
 * Copyright 2025 NXP.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "safety_cm33_mcx.h"
#include "fsl_clock.h"
#include "MCXA166.h"
#include "fsl_ctimer.h"
#include "drv.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define CRC_POLY	0x00001021

/* RAM test macros */
#define RAM_TEST_BACKUP_SIZE  	0x20
#define RAM_TEST_BLOCK_SIZE 	0x20

/* Clock test macros */
#define REF_TIMER_USED            CTIMER1
#define USED_CLOCK_SOURCE         kFRO_HF_to_CTIMER1
#define CLOCK_DIVIDER             kCLOCK_DivCTIMER1
#define CLOCK_DIVIDE_VALUE        4u
#define REF_TIMER_CLOCK_FREQUENCY 45e06
#define SYSTICK_RELOAD_VALUE      180000
#define ISR_FREQUENCY             1000 /* Hz */
#define CLOCK_TEST_TOLERANCE      100 //20  /* % */

#define PC_TEST_PATTERN 0x2001DFF0 /* Test address for Program counter test */



/*******************************************************************************
* Variables
******************************************************************************/

/* Symbols from linker */
extern uint32_t _stackTestPreBaseEnd;
extern uint32_t _stackTestPreBaseStart;
/* Put symbols content into constant */
const uint32_t c_stackTestFirstAddress = (uint32_t)&_stackTestPreBaseEnd;
const uint32_t c_stackTestSecondAddress = (uint32_t)&_stackTestPreBaseStart;
/* Define pattern */
const uint32_t c_stackPattern = 0xABCDABCD;
/* Symbol from linker */
extern uint32_t _StackTestSize;
/* Put symbol content into constant */
const uint32_t c_stack_test_block_size = (uint32_t)&_StackTestSize;

safety_common_t sFsCommon;
uint32_t ui32FsGlobalFault;
uint32_t ui32FsFailCnt;
uint32_t ui32FsResetType;


extern uint32_t __flash_test_start; /* symbol from Linker command file */
extern uint32_t __flash_test_end; /* symbol from Linker command file */

/* The safety-related FLASH CRC value. */
fs_crc_t c_sfsCRC __attribute__((used, section(".flshcrc"))) =
{
    .ui16StartSig    = 0xA55AU,
    .ui32StartAddr   = (uint32_t) &__flash_test_start,
    .ui32EndAddr     = (uint32_t) &__flash_test_end,
    .ui32OfflineCRC  = (uint32_t) 0xABCDABCD,
    .ui16EndSig      = 0x5AA5U
};

fs_flash_runtime_params_t sFlashCrc;
fs_flash_cfg_params_t sFlashConfig;
uint16_t ui16CodeCrcResult;
uint16_t ui16RtCodeCrcResult;
uint32_t ui32FlashTestRtCnt;

/******************RAM TEST VARIABLES*********************************************/
/* Symbol from Linker configuration file */
extern uint32_t m_safety_ram_start;
uint32_t ui32FsRamStartAddr = (uint32_t)&m_safety_ram_start;

/* Symbol from Linker configuration file */
extern uint32_t m_safety_ram_end;
uint32_t ui32FsRamEndAddr = (uint32_t)&m_safety_ram_end;

/* Symbol from Linker configuration file */
extern uint32_t _vStackBase;
extern uint32_t _vStackTop;

const uint32_t ui32FsStackEndAdr = (uint32_t)&_vStackTop;
const uint32_t ui32FsStackStartAdr = (uint32_t)&_vStackBase;

fs_ram_test_t 	sRamTest;
fs_ram_test_t 	sRamStackTest;
volatile uint32_t testVarArray[RAM_TEST_BACKUP_SIZE] 	FS_RAM_BACKUP_LOC;
uint32_t ui32RamRuntimeDoneCnt;


fs_clock_test_t sFsClockTest;
uint32_t ui32ClockCheckDoneCnt;

uint32_t programCounterTestFlag;
#define PC_TEST_FLAG ((uint32_t *) &programCounterTestFlag)


fs_wdog_test_t	sFsWdog FS_NOINIT_RAM_LOC;

wd_test_t 		sFsWdTest;
/*******************************************************************************
 * Code
 ******************************************************************************/


/**
 * @brief Performs post-reset CPU self-tests to verify core register integrity.
 *
 * This function is typically called immediately after a system reset to validate
 * the integrity of various CPU components, including:
 * - Stacked and non-stacked CPU registers
 * - PRIMASK register (Secure)
 * - Stack pointers (main and process, including limits)
 * - CONTROL register
 * - Special CPU features (e.g., priority levels)
 * - Floating-point unit (FPU) registers
 *
 * If any test fails, the corresponding error flag is set in the `sSafetyCommon.ui32SafetyErrors`
 * structure, and the system transitions to a safe state via `FsSafeState()`.
 *
 *
 * @return void
 */
void FsAfterResetCpuTests(void)
{
	FsPcTest(&sFsCommon, PC_TEST_PATTERN);

    /* stacked CPU registers */
    sFsCommon.ui32CpuRegTestResult = FS_CM33_CPU_Register();
    if (sFsCommon.ui32CpuRegTestResult == FS_FAIL_CPU_REGISTER)
    {
    	sFsCommon.ui32SafetyErrors |= CPU_REGISTERS_ERROR;
        FsSafeState();
    }

    /* non-stacked CPU registers */
    sFsCommon.ui32CpuNonStackedTestResult = FS_CM33_CPU_NonStackedRegister();
    if (sFsCommon.ui32CpuNonStackedTestResult == FS_FAIL_CPU_NONSTACKED_REGISTER)
    {
    	sFsCommon.ui32SafetyErrors |= CPU_NONSTACKED_ERROR;
        FsSafeState();
    }

    /* PRIMASK Secure */
    sFsCommon.ui32CpuPrimaskStestResult = FS_CM33_CPU_Primask_S();
    if (sFsCommon.ui32CpuPrimaskStestResult == FS_FAIL_CPU_PRIMASK)
    {
    	sFsCommon.ui32SafetyErrors |= CPU_PRIMASK_ERROR;
        FsSafeState();
    }

    /* SP main Secure */
    FS_CM33_CPU_SPmain_S();

    /* SP main limit Secure */
    FS_CM33_CPU_SPmain_Limit_S();

    /* SP process Secure */
    FS_CM33_CPU_SPprocess_S();

    /* SP process limit Secure */
    FS_CM33_CPU_SPprocess_Limit_S();

    sFsCommon.ui32CpuControlStestResult = FS_CM33_CPU_Control();
    if (sFsCommon.ui32CpuControlStestResult == FS_FAIL_CPU_CONTROL)
    {
    	sFsCommon.ui32SafetyErrors |= CPU_CONTROL_ERROR;
    	FsSafeState();
    }

    /* Special Secure */
    sFsCommon.ui32CpuSpecialStestResult = FS_CM33_CPU_Special8PriorityLevels_S();
    if (sFsCommon.ui32CpuSpecialStestResult == FS_FAIL_CPU_SPECIAL)
    {
        sFsCommon.ui32SafetyErrors |= CPU_SPECIAL_ERROR;
        FsSafeState();
    }

    sFsCommon.ui32CpuFloat1testResult = FS_CM33_CPU_Float1();
    if (sFsCommon.ui32CpuFloat1testResult == FS_FAIL_CPU_FLOAT_1)
    {
        sFsCommon.ui32SafetyErrors |= CPU_FLOAT_1_ERROR;
        FsSafeState();
    }

    sFsCommon.ui32CpuFloat2testResult = FS_CM33_CPU_Float2();
    if (sFsCommon.ui32CpuFloat2testResult == FS_FAIL_CPU_FLOAT_2)
    {
        sFsCommon.ui32SafetyErrors |= CPU_FLOAT_2_ERROR;
        FsSafeState();
    }
}

/**
 * @brief Puts the system into a safe state in case of a fault.
 *
 * This function sets the global fault flag and increments the fault counter.
 * If PWM is enabled, it should mask the PWM output and simulate a fault to stop the motor
 * It also sets the safety relay into a safe state.
 * If the SAFE_STATE_LOOP_ENABLE macro is defined, the function enters an infinite loop
 * to halt further execution and wait for watchdog reset.
 *
 * @note This function is part of a fault-handling system designed to
 * maintain functional safety and minimize risk during runtime errors.
 */
void FsSafeState(void)
{
	ui32FsGlobalFault = FS_GLOBAL_FAULT;
	ui32FsFailCnt++;

	/* In case of enabled PWM, then mask PWM out to stop the motor and set simulated fault */


	/* Set safety relay into safe state */

	__disable_irq();
#if SAFE_STATE_LOOP_ENABLE
	while(1)
	{
		__disable_irq();
		drvPumpReset();
		drvDispenserReset();
		drvRegenReset();
		drvH2OReset();
		drvDiverterReset();
		drvRelayHeaterReset();
		drvSwitchHeaterReset();
		drvPwmDisable();
	}
#endif
}

/*!
 * @brief  Checks the cause of the last system reset and handles safety-related reset conditions.
 *
 * This function reads the system reset source (SRS) register to determine the
 * type of reset that occurred. If the reset matches any safety-critical condition
 * defined by the composite FS_SAFETY_RESET mask, it:
 * - Sets the RESET_ERROR flag in the safety error structure.
 * - Invokes the FsSafeState() function to transition the system into a safe mode.
 *
 * @note This function clears previous safety errors before performing the check.
 *
 * @return None.
 */
void FsResetCheck(void)
{
	/* Init, clear errors */
	sFsCommon.ui32SafetyErrors = 0;
	/* Read reset type */
	ui32FsResetType = CMC->SRS;

	if (ui32FsResetType & FS_SAFETY_RESET)
	{
		sFsCommon.ui32SafetyErrors |= RESET_ERROR;
		FsSafeState();
	}
}

/**
 * @brief Checks for a global fault condition and transitions to a safe state if detected.
 *
 * This function inspects the value of `ui32FsGlobalFault` to determine if a fault
 * has occurred. If the value differs from `FS_GLOBAL_NOFAULT`, it assumes a fault
 * is present and initiates a transition into a predefined safe system state by calling
 * `FsSafeState()`.
 *
 * @note This function is part of a fault-handling system designed to
 * maintain functional safety and minimize risk during runtime errors.
 */
void FsCheckGlobalFault(void)
{
	/* If any value except FALSE */
	if(FS_GLOBAL_NOFAULT != ui32FsGlobalFault)
	{
		FsSafeState();
	}
}

/*!
 * @brief   Initialization of Safety Flash test.
 *
 *          Enable clock for HW CRC module.
 *          Inits the Flash test variables
 *
 * @param   psFlashCrc    - The pointer of the Flash CRC structure.
 * @param   psFlashConfig - The pointer of the Safety Flash test configuration structure.
 *
 * @return  None
 */
void FsFlashTestInit(fs_flash_runtime_params_t *psFlashCrc, fs_flash_cfg_params_t *psFlashConfig, fs_crc_t *psOfflineCrc)
{
    /* Enable clock to CRC module */
	CLOCK_EnableClock(kCLOCK_Crc0);
    RESET_ReleasePeripheralReset(kCRC0_RST_SHIFT_RSTn);
    CRC0->GPOLY = CRC_POLY;

    psFlashConfig->ui32StartAddress = psOfflineCrc->ui32StartAddr;
    psFlashConfig->ui32EndAddress 	= psOfflineCrc->ui32EndAddr;
    psFlashConfig->ui32Checksum 	= psOfflineCrc->ui32OfflineCRC;

    psFlashConfig->ui32Size               = psFlashConfig->ui32EndAddress - psFlashConfig->ui32StartAddress;
    psFlashConfig->ui32BlockSize          = FLASH_TEST_BLOCK_SIZE;
    psFlashConfig->ui32StartConditionSeed = (uint32_t)FLASH_TEST_CONDITION_SEED;

    psFlashCrc->ui32ActualAddress = psFlashConfig->ui32StartAddress;    /* start address */
    psFlashCrc->ui32PartCrc       = psFlashConfig->ui32StartConditionSeed;    /* initial seed */
    psFlashCrc->ui32BlockSize     = (psFlashConfig->ui32Size < psFlashConfig->ui32BlockSize) ? psFlashConfig->ui32Size : psFlashConfig->ui32BlockSize;
}

/*!
 * @brief   After-reset Flash test.
 *
 *          This function calls the flash test function from IEC60730 library.
 *          Safety-related part of the flash is tested at once.
 *          In case of incorrect flash test result, it updates the safetyErrors variable accordingly.
 *
 * @param   psSafetyCommon - The pointer of the Common Safety structure
 * @param   psFlashConfig  - The pointer of the Safety Flash test configuration structure.
 *
 * @return  None
 */
void FsFlashTestAr(safety_common_t *psSafetyCommon, fs_flash_cfg_params_t *psFlashConfig, uint16_t *pCrcResult)
{
	/* Copy start condition seed */
	*pCrcResult = (uint16_t)psFlashConfig->ui32StartConditionSeed;

    psSafetyCommon->ui32FlashTestResult =
    FS_FLASH_C_HW16_K(psFlashConfig->ui32StartAddress, psFlashConfig->ui32Size, (FS_CRC_Type* )CRC0_BASE, pCrcResult);

    if (*pCrcResult != psFlashConfig->ui32Checksum)
    {
        psSafetyCommon->ui32SafetyErrors |= FLASH_TEST_ERROR;
        FsSafeState();
    }

    if (FS_PASS != psSafetyCommon->ui32FlashTestResult)
    {
        psSafetyCommon->ui32SafetyErrors |= FLASH_TEST_ERROR;
        FsSafeState();
    }
}

/*!
 * @brief   Initialization of Safety RAM test.
 *
 *          Inits the RAM test variables
 *
 * @param   psSafetyRamTest - The pointer of the RAM test structure.
 * @param   pSafetyRamStart - The pointer of the RAM test start address.
 * @param   pSafetyRamEnd   - The pointer of the RAM test end address.
 *
 * @return  None
 */

void FsRamTestInit(fs_ram_test_t *psSafetyRamTest, uint32_t *pSafetyRamStart, uint32_t *pSafetyRamEnd, uint32_t *pSafetyRamTestBackup)
{
    psSafetyRamTest->ui32RamTestStartAddress = (uint32_t)pSafetyRamStart;
    psSafetyRamTest->ui32RamTestEndAddress   = (uint32_t)pSafetyRamEnd;
    psSafetyRamTest->ui32DefaultBlockSize 	 = RAM_TEST_BACKUP_SIZE;
    psSafetyRamTest->ui32BlockSize 			 = RAM_TEST_BLOCK_SIZE;
    psSafetyRamTest->ui32ActualAddress 		 = psSafetyRamTest->ui32RamTestStartAddress;
    psSafetyRamTest->ui32BackupAddress 		 = (uint32_t)pSafetyRamTestBackup;
}

/*!
 * @brief   After-reset RAM test.
 *
 *          This function calls the RAM test function from IEC60730 library.
 *          Safety-related part of the RAM is tested at once.
 *          In case of incorrect RAM test result, it updates the safetyErrors variable accordingly.
 *
 * @param   psSafetyCommon  - The pointer of the Common Safety structure
 * @param   psSafetyRamTest - The pointer of the Safety RAM test structure.
 *
 * @return  None
 */
void FsRamTestAr(safety_common_t *psSafetyCommon, fs_ram_test_t *psSafetyRamTest)
{
    psSafetyCommon->ui32RamTestArResult = FS_CM33_RAM_AfterReset(psSafetyRamTest->ui32RamTestStartAddress, \
                                                             psSafetyRamTest->ui32RamTestEndAddress,   \
                                                             psSafetyRamTest->ui32DefaultBlockSize,    \
                                                             psSafetyRamTest->ui32BackupAddress,       \
                                                             FS_CM33_RAM_SegmentMarchC);

    if (psSafetyCommon->ui32RamTestArResult == FS_FAIL_RAM)
    {
        psSafetyCommon->ui32SafetyErrors |= RAM_TEST_ERROR;
        FsSafeState();
    }
}


void FsAfterResetMemTests(void)
{
	/* STACK TEST */
	FS_CM33_STACK_Init(c_stackPattern, c_stackTestFirstAddress, c_stackTestSecondAddress, c_stack_test_block_size);
	sFsCommon.ui32StackTestResult = FS_CM33_STACK_Test(c_stackPattern, c_stackTestFirstAddress, c_stackTestSecondAddress, c_stack_test_block_size);
    if (sFsCommon.ui32StackTestResult == FS_FAIL_STACK)
    {
        sFsCommon.ui32SafetyErrors |= STACK_TEST_ERROR;
        FsSafeState();
    }

#if AR_FLASH_TEST_ENABLE
	FsFlashTestInit(&sFlashCrc, &sFlashConfig, &c_sfsCRC);
	FsFlashTestAr(&sFsCommon, &sFlashConfig, &ui16CodeCrcResult);
#endif

#if AR_RAM_TEST_ENABLE
	FsRamTestInit(&sRamTest, (uint32_t *)ui32FsRamStartAddr, (uint32_t *)ui32FsRamEndAddr, (uint32_t *)testVarArray);
	FsRamTestAr(&sFsCommon, &sRamTest);
#endif

#if AR_RAM_STACK_TEST_ENABLE
	FsRamTestInit(&sRamStackTest, (uint32_t *)ui32FsStackStartAdr, (uint32_t *)ui32FsStackEndAdr, (uint32_t *)testVarArray);
	FsRamTestAr(&sFsCommon, &sRamStackTest);
#endif

}

/*!
 * @brief   Initialization of Safety clock test.
 *
 *          Complete Initialization of the clock test.
 *          Function calculates limit values.
 *          Cals clock test init function from the IEC60730B library.
 *
 * @param   sFsCommon    - The pointer of the Common Safety structure
 * @param   psSafetyClockTest - The pointer of the Safety Clock test structure
 * @param   peClockFreq       - The pointer of the clock name enumeration
 *
 * @return  None
 */
void FsClockTestInit(safety_common_t *psSafetyCommon, fs_clock_test_t *psSafetyClockTest)
{
    psSafetyCommon->ui32RefClkFreq            = REF_TIMER_CLOCK_FREQUENCY;
    psSafetyClockTest->ui32ClockTestExpected  = psSafetyCommon->ui32RefClkFreq / (uint32_t)ISR_FREQUENCY;
    psSafetyClockTest->ui32ClockTestTolerance = (psSafetyClockTest->ui32ClockTestExpected * (uint32_t)CLOCK_TEST_TOLERANCE) / (uint32_t)100;
    psSafetyClockTest->ui32ClockTestLimitHigh = psSafetyClockTest->ui32ClockTestExpected + psSafetyClockTest->ui32ClockTestTolerance;
    psSafetyClockTest->ui32ClockTestLimitLow  = psSafetyClockTest->ui32ClockTestExpected - psSafetyClockTest->ui32ClockTestTolerance;
    psSafetyClockTest->ui32ClockTestStart     = 0; /* clock test result will be processed after the first interrupt occurs */

    FS_CLK_Init((uint32_t *)&psSafetyClockTest->ui32ClockTestContext);

    /* Initialization of timer */
    /* Use FRO HF clock for some of the Ctimers */
    CLOCK_SetClockDiv(CLOCK_DIVIDER, CLOCK_DIVIDE_VALUE);
    CLOCK_AttachClk(USED_CLOCK_SOURCE);

    ctimer_config_t config;
    CTIMER_GetDefaultConfig(&config);
    CTIMER_Init(REF_TIMER_USED, &config);
    CTIMER_StartTimer(REF_TIMER_USED);
}

/*!
 * @brief   Clock test function, called from interrupt.
 *
 *          This function calls clock test function from the IEC60730B library and enable the test evaluation.
 *          It must be called in the Systick interrupt to catch the value of LPTMR counter.
 *
 * @param   psSafetyClockTest - The pointer of the Safety Clock test structure
 *
 * @return  None
 */
void FsClockTestIsr(fs_clock_test_t *psSafetyClockTest)
{
    FS_CLK_CTIMER((fs_ctimer_t *)REF_TIMER_USED, (uint32_t *)&psSafetyClockTest->ui32ClockTestContext);

    if (psSafetyClockTest->ui32ClockTestStart != 2)
    {
        psSafetyClockTest->ui32ClockTestStart += 1; /* to prevent checking of result before execution */
    }
}

/*!
 * @brief   Clock test check function.
 *
 *          This function can be called from any place of application.
 *          It calls the FS_CLK_Check function from the IEC60730 library
 *          In case of incorrect clock test result, it updates the safetyErrors variable accordingly.
 *          A node of program flow check is placed here.
 *
 * @param   sFsCommon           - The pointer of the Common Safety structure
 * @param   psSafetyClockTest       - The pointer of the Safety Clock test structure
 * @param   psSafetyProgramFlowTest - The pointer of the Program flow test structure
 *
 * @return  None
 */
void FsClockTestCheck(safety_common_t *psSafetyCommon, fs_clock_test_t *psSafetyClockTest)
{
    if (psSafetyClockTest->ui32ClockTestStart) /* condition is valid after the first Systick interrupt */
    {
        psSafetyCommon->ui32ClockTestResult =
            FS_CLK_Check(psSafetyClockTest->ui32ClockTestContext, psSafetyClockTest->ui32ClockTestLimitLow,
                         psSafetyClockTest->ui32ClockTestLimitHigh);
        if (psSafetyCommon->ui32ClockTestResult == FS_FAIL_CLK)
        {
            psSafetyCommon->ui32SafetyErrors |= CLOCK_TEST_ERROR;
            FsSafeState();
        }

        ui32ClockCheckDoneCnt++;
    }
}

/**
 * @brief Prepare runtime flash test structures
 *
 * @return None
 */
void FsFlashTestRtInit(void)
{
	sFlashCrc.ui32PartCrc       = sFlashConfig.ui32StartConditionSeed; 	/* set start seed as input for CRC calculation */
	sFlashCrc.ui32ActualAddress = sFlashConfig.ui32StartAddress; 		/* set start address */
	sFlashCrc.ui32BlockSize     = sFlashConfig.ui32BlockSize;    		/* size of block for CRC testing */
}


/**
 * @brief Performs a runtime flash memory test using CRC verification.
 *
 * This function calculates the CRC of a block of flash memory and compares it
 * against a predefined checksum to verify flash integrity. If the CRC does not match,
 * it sets a safety error flag and transitions the system to a safe state.
 * If the CRC matches, it resets the CRC calculation parameters for the next test cycle.
 *
 * The function is designed to be called periodically during runtime to ensure
 * ongoing flash memory integrity.
 *
 * @note This function modifies global structures: `sFlashCrc`, `sFlashConfig`, `sFsCommon`,
 *       and updates `ui16RtCodeCrcResult` and `ui32FlashTestRtCnt`.
 *
 * @return None
 */
void FsFlashTestRt(void)
{
	FS_FLASH_C_HW16_K(sFlashCrc.ui32ActualAddress, sFlashCrc.ui32BlockSize, (FS_CRC_Type* )CRC0_BASE, (uint16_t*)&(sFlashCrc.ui32PartCrc));
	sFlashCrc.ui32ActualAddress += sFlashCrc.ui32BlockSize;
    if (sFlashCrc.ui32ActualAddress >= sFlashConfig.ui32EndAddress) /* if all the addresses were tested... */
    {
        if (sFlashCrc.ui32PartCrc != sFlashConfig.ui32Checksum) 			/* checksum must be same as calculated in linker */
        {
        	/* Flash test not OK */
        	sFsCommon.ui32SafetyErrors |= FLASH_TEST_ERROR;
            FsSafeState();
        }
        else
        {
        	/* Flash test done */
        	ui16RtCodeCrcResult 		= sFlashCrc.ui32PartCrc;
        	ui32FlashTestRtCnt++;
        	sFlashCrc.ui32PartCrc       = sFlashConfig.ui32StartConditionSeed; 	/* set start seed as input for CRC calculation */
        	sFlashCrc.ui32ActualAddress = sFlashConfig.ui32StartAddress; 		/* set start address */
        	sFlashCrc.ui32BlockSize     = sFlashConfig.ui32BlockSize;    		/* size of block for CRC testing */
        }
    }
    else
    {
    	/* Calc rest of blocksize if needed */
    	if((sFlashConfig.ui32EndAddress - sFlashCrc.ui32ActualAddress) < sFlashCrc.ui32BlockSize)
    	{
    		sFlashCrc.ui32BlockSize = sFlashConfig.ui32EndAddress - sFlashCrc.ui32ActualAddress;
    	}
    }
}

/**
 * @brief Prepare clock test resources
 *
 * @return None
 */
void FsAfterResetClockTest()
{
	/* Init resources for clock test */
	FsClockTestInit(&sFsCommon, &sFsClockTest);
	SysTick_Config(SYSTICK_RELOAD_VALUE);
}
/**
 * @brief Perform clock test
 *
 * @return None
 */
void FsRuntimeClockTest()
{
	 FsClockTestCheck(&sFsCommon, &sFsClockTest);
}
/**
 * @brief Perform clock test update
 *
 * @return None
 */
void FsRuntimeClockUpdate()
{
	FsClockTestIsr(&sFsClockTest);
}

/*!
 * @brief   Runtime RAM test.
 *
 *          This function calls the RAM test function from IEC60730 library.
 *          Safety-related part of the RAM is tested in sequence.
 *          Calls SafetyFlashTestHandling function.
 *          In case of incorrect RAM test result, it updates the safetyErrors variable accordingly.
 *
 * @param   psSafetyCommon  - The pointer of the Common Safety structure
 * @param   psSafetyRamTest - The pointer of the Safety RAM test structure.
 *
 * @return  None
 */
void FsRamRuntimeTest(safety_common_t *psSafetyCommon, fs_ram_test_t *psSafetyRamTest)
{
    psSafetyCommon->ui32RamTestRtResult =
    FS_CM33_RAM_Runtime(psSafetyRamTest->ui32RamTestStartAddress, psSafetyRamTest->ui32RamTestEndAddress,
                       (uint32_t *)&psSafetyRamTest->ui32ActualAddress, psSafetyRamTest->ui32BlockSize,
                        psSafetyRamTest->ui32BackupAddress, FS_CM33_RAM_SegmentMarchX);

    if (psSafetyCommon->ui32RamTestRtResult == FS_FAIL_RAM)
    {
        psSafetyCommon->ui32SafetyErrors |= RAM_TEST_ERROR;
        FsSafeState();
    }

    /* Test if whole RAM was tested, only for example validation */
    if (psSafetyRamTest->ui32RamTestStartAddress == psSafetyRamTest->ui32ActualAddress)
    {
    	ui32RamRuntimeDoneCnt++;
    }

}

/*!
 * @brief   Interruptible test of CPU registers.
 *
 *          This function calls the CPU test functions from IEC60730 library.
 *          The function can be called from the background loop.
 *          In case of incorrect flash test result, it updates the safetyErrors variable accordingly.
 *          See IEC60730 library documentation for CPU errors handling !
 *
 * @param   psSafetyCommon - The pointer of the Common Safety structure
 *
 * @return  None
 */
void FsCpuBackgroundTest()
{
    /* stacked CPU registers */
	sFsCommon.ui32CpuRegTestResult = FS_CM33_CPU_Register();
    if (sFsCommon.ui32CpuRegTestResult == FS_FAIL_CPU_REGISTER)
    {
    	sFsCommon.ui32SafetyErrors |= CPU_REGISTERS_ERROR;
        FsSafeState();
    }

    sFsCommon.ui32CpuNonStackedTestResult = FS_CM33_CPU_NonStackedRegister();
    if (sFsCommon.ui32CpuNonStackedTestResult == FS_FAIL_CPU_NONSTACKED_REGISTER)
    {
    	sFsCommon.ui32SafetyErrors |= CPU_NONSTACKED_ERROR;
        FsSafeState();
    }

    /* safety test of CPU CONTROL register, it cannot be placed in interrupt, thus interrupts must be disabled for a
     * while */
    /* - see IEC60730 library documentation for CPU errors handling ! */
    __asm("CPSID i");
    sFsCommon.ui32CpuControlStestResult = FS_CM33_CPU_Control();
    __asm("CPSIE i");
    if (sFsCommon.ui32CpuControlStestResult == FS_FAIL_CPU_CONTROL)
    {
    	sFsCommon.ui32SafetyErrors |= CPU_CONTROL_ERROR;
    	FsSafeState();
    }

    /* safety test of CPU SP_PROCESS register, it cannot be placed in interrupt, thus interrupts must be disabled
     * for a while */
    /* - see IEC60730 library documentation for CPU errors handling ! */
    __asm("CPSID i");
    FS_CM33_CPU_SPprocess_S();
    __asm("CPSIE i");

}

/*!
 * @brief   Handling of the safety functions that must be called in interrupt routine.
 *
 *          This function switches between safety functions that are called in interrupt
 *
 * @param   psSafetyCommon       - The pointer of the Common Safety structure
 * @param   psSafetyRamTest      - The pointer of the Safety RAM test structure.
 * @param   psSafetyRamStackTest - The pointer of the Safety RAM test structure for Stack area.
 *
 * @return  None
 */
void FsIsrTests()
{
	sFsCommon.ui32StackTestResult = FS_CM33_STACK_Test(c_stackPattern, c_stackTestFirstAddress, c_stackTestSecondAddress, c_stack_test_block_size);
    if (sFsCommon.ui32StackTestResult == FS_FAIL_STACK)
    {
        sFsCommon.ui32SafetyErrors |= STACK_TEST_ERROR;
        FsSafeState();
    }

    switch (sFsCommon.ui32FastIsrSafetySwitch)
    {
        case 0: /* CPU registers test that cannot be interrupted */
            FsCpuIsrTest(&sFsCommon);
            break;
        case 1: /* Program counter test */
            FsPcTest(&sFsCommon, PC_TEST_PATTERN);
            break;
        case 2: /* RAM March test for safety related RAM space */
            FsRamRuntimeTest(&sFsCommon, &sRamTest);
            break;
        case 3: /* RAM March test for memory occupied by the Stack */
            FsRamRuntimeTest(&sFsCommon, &sRamStackTest);
            break;
        default:
            __asm("nop");
            break;
    }

    sFsCommon.ui32FastIsrSafetySwitch++;
    if (sFsCommon.ui32FastIsrSafetySwitch >= 4)
    {
        sFsCommon.ui32FastIsrSafetySwitch = 0;
    }
}

/*!
 * @brief   Uninterruptible test of CPU registers.
 *
 *          This function calls the CPU test functions from IEC60730 library.
 *          The function must be called from an interrupt with highest priority.
 *          In case of incorrect flash test result, it updates the safetyErrors variable accordingly.
 *          See IEC60730 library documentation for CPU errors handling !
 *
 * @param   psSafetyCommon - The pointer of the Common Safety structure
 *
 * @return  None
 */
void FsCpuIsrTest(safety_common_t *psSafetyCommon)
{
    /* PRIMASK Secure */
    psSafetyCommon->ui32CpuPrimaskStestResult = FS_CM33_CPU_Primask_S();
    if (psSafetyCommon->ui32CpuPrimaskStestResult == FS_FAIL_CPU_PRIMASK)
    {
        psSafetyCommon->ui32SafetyErrors |= CPU_PRIMASK_ERROR;
        FsSafeState();
    }

    /* Special Secure */
    psSafetyCommon->ui32CpuSpecialStestResult = FS_CM33_CPU_Special8PriorityLevels_S();
    if (psSafetyCommon->ui32CpuSpecialStestResult == FS_FAIL_CPU_SPECIAL)
    {
        psSafetyCommon->ui32SafetyErrors |= CPU_SPECIAL_ERROR;
        FsSafeState();
    }

    /* SP main Secure */
    FS_CM33_CPU_SPmain_S();

    /* SP main limit Secure */
    FS_CM33_CPU_SPmain_Limit_S();
}

void FsAfterResetVarInit()
{
    /* Clear the variable that records safety error codes */
    sFsCommon.ui32SafetyErrors        = 0;
    sFsCommon.ui32FastIsrSafetySwitch = 0;
    ui32FsGlobalFault = FS_GLOBAL_NOFAULT;
}

/*!
 * @brief   Program counter test.
 *
 *          This function uses two addresses: first is defined in linker file (iec60730b_cm33_pc_object.o),
 *          second address comes as function argument (must be RAM address).
 *          Both addresses must be defined by the developer and suitable to test all of the possible PC bits.
 *          This test cannot be interrupted.
 *          In case of incorrect PC test result, it updates the safetyErrors variable accordingly.
 *
 * @param   psSafetyCommon - The pointer of the Common Safety structure
 * @param   pattern        - RAM address, it can vary with multiple function calls
 *
 * @return  None
 */
void FsPcTest(safety_common_t *psSafetyCommon, uint32_t pattern)
{
    psSafetyCommon->ui32PcTestResult = FS_CM33_PC_Test(pattern, FS_PC_Object, (uint32_t *)PC_TEST_FLAG);

    if (psSafetyCommon->ui32PcTestResult == FS_FAIL_PC)
    {
        psSafetyCommon->ui32SafetyErrors |= PC_TEST_ERROR;
        FsSafeState();
    }
}

void FsWdTestInit(safety_common_t *psSafetyCommon, wd_test_t *psSafetyWdTest)
{
    /* CTIMER initialization */
    WdCtimerInit();
    /* Calculate counter limit values */
    psSafetyWdTest->ui64WdTestTemp1     = ((uint64_t)WD_TIMEOUT_VALUE * (uint64_t)WD_REF_TIMER_CLOCK_FREQUENCY);
    psSafetyWdTest->ui32WdTestExpected  = psSafetyWdTest->ui64WdTestTemp1 / (uint32_t)WD_CLOCK;
    psSafetyWdTest->ui32WdTestTolerance = (psSafetyWdTest->ui32WdTestExpected * (uint32_t)WD_TEST_TOLERANCE) / (uint32_t)100;
    psSafetyWdTest->ui32WdTestLimitHigh = psSafetyWdTest->ui32WdTestExpected + psSafetyWdTest->ui32WdTestTolerance;
    psSafetyWdTest->ui32WdTestLimitLow  = psSafetyWdTest->ui32WdTestExpected - psSafetyWdTest->ui32WdTestTolerance;
    //counterLimitHigh                = psSafetyWdTest->wdTestLimitHigh;
    //counterLimitLow                 = psSafetyWdTest->wdTestLimitLow;

    /* Safety library structure initialization in ram noinit area */
    sFsWdog.RefTimerBase         = (uint32_t)REF_TIMER_USED;
    sFsWdog.WdogBase             = (uint32_t)WD_USED;
    sFsWdog.pResetDetectRegister = (uint32_t)&(CMC->SRS);
    sFsWdog.ResetDetectMask      = (uint32_t)CMC_SRS_WWDT0_MASK;
}


/*!
 * @brief   Safety watchdog test.
 *
 *          This function is used to test the Watchdog.
 *          Sets up LPTMR for the test.
 *          Calculates limit values for watchdog timeout.
 *          Performs the watchdog test.
 *
 * @param   psSafetyCommon - The pointer of the Common Safety structure
 * @param   psSafetyWdTest - The pointer of the Safety Watchdog test structure
 * @param   peClockFreq     - The pointer of the clock name enumeration
 *
 * @return  None
 */
void FsWdTest(safety_common_t *psSafetyCommon, wd_test_t *psSafetyWdTest)
{
	/* If non WD reset --- because of debugging--- in real it must be only after POR reset */
	if(ui32FsResetType&CMC_SRS_POR_MASK)
    {
        //*SAFETY_ERROR_CODE = 0; /* clean the safety error code flag */
        FS_WDOG_Setup_WWDT_CTIMER(&sFsWdog);

    }
	/* Expected WD reset */
    if(ui32FsResetType&CMC_SRS_WWDT0_MASK)
    {
        psSafetyCommon->ui32WdTestResult = FS_WDOG_Check_WWDT_MCX(psSafetyWdTest->ui32WdTestLimitHigh,
        		                                                  psSafetyWdTest->ui32WdTestLimitLow,
																  WD_RESETS_LIMIT,
																  WD_ENDLESS_LOOP_ENABLE,
																  &sFsWdog);
        if (psSafetyCommon->ui32WdTestResult != FS_PASS) /* WDOG can return more error messages */
        {
            psSafetyCommon->ui32SafetyErrors |= WDOG_TEST_ERROR;
            FsSafeState();
        }
    }

    psSafetyWdTest->ui32WdResets       = sFsWdog.resets;
    psSafetyWdTest->ui32WdTimeoutCheck = sFsWdog.counter;
    psSafetyWdTest->ui32WdRefreshRatio = 0U;

}

void WdCtimerInit(void)
{
    /* Use FRO HF clock for some of the Ctimers */
    CLOCK_SetClockDiv(CLOCK_DIVIDER, CLOCK_DIVIDE_VALUE);
    CLOCK_AttachClk(USED_CLOCK_SOURCE);
    ctimer_config_t config;
    CTIMER_GetDefaultConfig(&config);
    CTIMER_Init(REF_TIMER_USED, &config);
    CTIMER_StartTimer(REF_TIMER_USED);
}

void FsWd()
{
#if WD_ENABLE
	FsWdInit(WD_TIMEOUT_VALUE);
#endif
	FsWdTestInit(&sFsCommon, &sFsWdTest);
#if WD_ENABLE
	FsWdTest(&sFsCommon, &sFsWdTest);
#endif
}

void FsWdInit(uint32_t wdSetupvalue)
{
    uint32_t reg_offset         = CLK_GATE_REG_OFFSET(kCLOCK_GateWWDT0);
    uint32_t bit_shift          = CLK_GATE_BIT_SHIFT(kCLOCK_GateWWDT0);
    volatile uint32_t *pClkCtrl = (volatile uint32_t *)((uint32_t)(&(MRCC0->MRCC_GLB_CC0_SET)) + reg_offset);

    /* Unlock clock configuration */
    SYSCON->CLKUNLOCK &= ~SYSCON_CLKUNLOCK_UNLOCK_MASK;

    *pClkCtrl = (1UL << bit_shift);

    /* Freeze clock configuration */
    SYSCON->CLKUNLOCK |= SYSCON_CLKUNLOCK_UNLOCK_MASK;

    WD_USED->TC     = WWDT_TC_COUNT(wdSetupvalue); /* refresh value */
    WD_USED->MOD    = WWDT_MOD_WDRESET(1) | WWDT_MOD_WDEN(1);
    WD_USED->WINDOW = 0xFFFFFF; /* Disable Window mode */

    __asm("CPSID i");
    WD_USED->FEED = 0xAA; /* Start WDOG */
    WD_USED->FEED = 0x55;
    __asm("CPSIE i");
}
