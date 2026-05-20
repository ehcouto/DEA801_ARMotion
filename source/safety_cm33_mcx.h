/*
 * Copyright 2025 NXP.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _SAFETY_CM33_MCX_H_
#define _SAFETY_CM33_MCX_H_

#include "iec60730b.h"
#include "iec60730b_core.h"

#include "MCXA166.h"
#include "MCXA166_COMMON.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define AR_FLASH_TEST_ENABLE		1//1
#define AR_RAM_TEST_ENABLE			1//1
#define AR_RAM_STACK_TEST_ENABLE	1//1
#define SAFE_STATE_LOOP_ENABLE		1//0
#define WD_ENABLE					1//0

#define WD_ENDLESS_LOOP_ENABLE 1 /* set 1 or 0 */
#define WD_USED            	   WWDT0
#define WD_RESETS_LIMIT        1000
#define WD_REFRESH_RATIO       1
#define WD_TIMEOUT_VALUE       1000000 /* 40ms refresh period (1000 / 250kHz) */
#define WD_REF_TIMER_CLOCK_FREQUENCY 45e06
#define WD_CLOCK               250000
#define WD_TEST_TOLERANCE      20 /* % */

#if WD_ENABLE
	#define WD_REFRESH \
		WWDT0->FEED = 0xAA;   \
		WWDT0->FEED = 0x55
#else
	#define WD_REFRESH
#endif


#define FS_NOINIT_RAM_LOC __attribute__((section(".noinit")))
#define FS_RAM_BACKUP_LOC __attribute__((section(".ramBackup")))

#define CPU_TEST_ERROR           0x1               /* CPU test fault flag */
#define FLASH_TEST_ERROR         0x2               /* Flash test fault flag */
#define RAM_TEST_ERROR           0x4               /* RAM test fault flag */
#define STACK_TEST_ERROR         0x8               /* Stack test fault flag */
#define CLOCK_TEST_ERROR         0x10              /* Clock test fault flag */
#define PC_TEST_ERROR            0x20              /* Program counter test fault flag */
#define CPU_PRIMASK_ERROR        0x40              /* PRIMASK test fault flag */
#define CPU_REGISTERS_ERROR      0x80              /* CPU registers test fault flag */
#define CPU_NONSTACKED_ERROR     0x100             /* non-stacked CPU test fault flag */
#define CPU_FLOAT_1_ERROR        0x200             /* floating point registers test fault flag */
#define CPU_FLOAT_2_ERROR        0x400             /* floating point registers test fault flag */
#define CPU_CONTROL_ERROR        0x800             /* CONTROL register test fault flag */
#define CPU_SPECIAL_ERROR        0x1000            /* special CPU registers test fault flag */
#define DIO_TEST_ERROR           0x2000            /* DIO test fault flag */
#define AIO_TEST_ERROR           0x4000            /* AIO test fault flag */
#define WDOG_TEST_ERROR          0x8000            /* WDOG test fault flag */
#define RESET_ERROR           	 0x10000

#define FS_FLASH_PASS     0x0        /* Flash test pass return */
#define FS_FLASH_FAIL     0x00000301 /* Flash test fail return */
#define FS_FLASH_PROGRESS 0x00000302 /* Flash test in progress  */

#define FS_GLOBAL_FAULT				0xAAAAAAAA
#define FS_GLOBAL_NOFAULT			0x55555555

#define FS_SAFETY_RESET				(CMC_SRS_RSTACK_MASK|\
									 CMC_SRS_SCG_MASK   |\
									 CMC_SRS_LOCKUP_MASK|\
									 CMC_SRS_CDOG0_MASK)


/* FLASH TEST CFG */
#define FLASH_TEST_BLOCK_SIZE     0x20
#define FLASH_TEST_CONDITION_SEED 0x0000 /* 0xFFFFFFFF CRC32, 0x0000 CRC16 */
/*! @brief Safety tests */
typedef struct _safety_common
{
	uint32_t ui32RefClkFreq;
	uint32_t ui32ClockTestResult;
    uint32_t ui32FastIsrSafetySwitch;
    uint32_t ui32SafetyErrors;
    uint32_t ui32ClkTestResult;
    uint32_t ui32FlashTestResult;
    uint32_t ui32RamTestArResult;
    uint32_t ui32RamTestRtResult;
    uint32_t ui32CpuPrimaskStestResult;
    uint32_t ui32CpuSpecialStestResult;;
    uint32_t ui32CpuRegTestResult;
    uint32_t ui32CpuNonStackedTestResult;
    uint32_t ui32CpuControlStestResult;
    uint32_t ui32CpuControlNStestResult;
    uint32_t ui32CpuFloat1testResult;
    uint32_t ui32CpuFloat2testResult;
    uint32_t ui32StackTestResult;
    uint32_t ui32WdTestResult;
    uint32_t ui32WdPreTestResult;
    uint32_t ui32FuSaRunTimeExecError;
    uint16_t ui32AfterResetCrc;
    uint16_t ui32FatalReset;
    uint16_t ui32PcTestResult;
} safety_common_t;




extern uint32_t ui32FsGlobalFault;
extern uint32_t ui32FsFailCnt;
extern uint32_t ui32ResetType;

/* Safety Flash test runtime */
typedef struct _flash_runtime_test_parameters
{
    uint32_t ui32BlockSize;         /* size of tested block */
    uint32_t ui32ActualAddress;     /* actual start address for crc module */
    uint32_t ui32PartCrc;           /* seed in begin, particular crc result in process, crc result in final*/
} fs_flash_runtime_params_t;

/* Safety Flash test parameters */
typedef struct _flash_configuration_parameters
{
    uint32_t ui32StartConditionSeed;
    uint32_t ui32StartAddress;
    uint32_t ui32EndAddress;
    uint32_t ui32Size;
    uint32_t ui32BlockSize;
    uint32_t ui32Checksum;
} fs_flash_cfg_params_t;


/* CRC structure containing information for the offline CRC calculation. */
typedef struct _fs_crc
{
    uint16_t ui16StartSig;								/* Start marker for the CRC information table */
    uint32_t ui32StartAddr 	__attribute__((packed));	/* Start address for the CRC calculation */
    uint32_t ui32EndAddr   	__attribute__((packed));	/* End address for the CRC calculation */
    uint32_t ui32OfflineCRC __attribute__((packed));	/* Seed value */
    uint16_t ui16EndSig     __attribute__((packed));	/* End marker for the CRC information table */
} fs_crc_t;


/*! @brief Safety RAM test */
typedef struct _ram_test
{
    uint32_t ui32RamTestStartAddress;
    uint32_t ui32RamTestEndAddress;
    uint32_t ui32BlockSize;
    uint32_t ui32ActualAddress;
    uint32_t ui32DefaultBlockSize;
    uint32_t ui32BackupAddress;
} fs_ram_test_t;

/*! @brief Safety Clock test */
typedef struct _clock_test
{
    uint32_t ui32ClockTestContext;
    uint32_t ui32ClockTestTolerance;
    uint32_t ui32ClockTestExpected;
    uint32_t ui32ClockTestLimitHigh;
    uint32_t ui32ClockTestLimitLow;
    uint32_t ui32SystickReloadValue;
    uint16_t ui32ClockTestStart;
} fs_clock_test_t;

/*! @brief Safety Watchdog test */
typedef struct _wd_test
{
    uint64_t ui64WdTestTemp1;
    uint32_t ui32WdTestExpected;
    uint32_t ui32WdTestTolerance;
    uint32_t ui32WdTestLptmrCnt;
    uint32_t ui32WdTestLimitHigh;
    uint32_t ui32WdTestLimitLow;
    uint32_t ui32WdResets;
    uint32_t ui32WdTimeoutCheck;
    uint16_t ui32WdRefreshRatio;
} wd_test_t;


#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @name Safety Class B tests handling functions
 * @{
 */

/*******************************************************************************
 * API
 ******************************************************************************/

void FsAfterResetVarInit(void);
void FsAfterResetCpuTests(void);
void FsAfterResetMemTests(void);
void FsAfterResetClockTest(void);
void FsRuntimeClockTest(void);
void FsRuntimeClockUpdate(void);
void FsSafeState(void);
void FsResetCheck(void);
void FsCheckGlobalFault(void);
void FsFlashTestInit(fs_flash_runtime_params_t *psFlashCrc, fs_flash_cfg_params_t *psFlashConfig, fs_crc_t *psOfflineCrc);
void FsFlashTestAr(safety_common_t *psSafetyCommon, fs_flash_cfg_params_t *psFlashConfig, uint16_t *pCrcResult);
void FsClockTestInit(safety_common_t *psSafetyCommon, fs_clock_test_t *psSafetyClockTest);
void FsClockTestIsr(fs_clock_test_t *psSafetyClockTest);
void FsClockTestCheck(safety_common_t *psSafetyCommon, fs_clock_test_t *psSafetyClockTest);
void FsFlashTestRtInit(void);
void FsFlashTestRt(void);
void FsCpuIsrTest(safety_common_t *psSafetyCommon);
void FsIsrTests(void);
void FsRamRuntimeTest(safety_common_t *psSafetyCommon, fs_ram_test_t *psSafetyRamTest);
void FsPcTest(safety_common_t *psSafetyCommon, uint32_t pattern);
void FsWdTestInit(safety_common_t *psSafetyCommon, wd_test_t *psSafetyWdTest);
void FsWdTest(safety_common_t *psSafetyCommon, wd_test_t *psSafetyWdTest);
void FsCpuBackgroundTest();
void FsWd(void);
void WdCtimerInit(void);
void FsWdInit(uint32_t wdSetupvalue);
#ifdef __cplusplus
}
#endif

#endif /* _SAFETY_CM33_MCX_H_ */
