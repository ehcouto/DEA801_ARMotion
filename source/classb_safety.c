/*
 * classb_safety.c
 *
 *  Created on: 24 Eki 2025
 *      Author: 26098107
 */

/*
#include "iec60730b_core.h"
#include "safety_cm33_mcx.h"
#include "classb_safety.h"
#include "iec60730b.h"
#include "drv_config.h"
#include "iec60730b_types.h"

FS_RESULT classB_ADC_Val;
FS_RESULT dio_short_to_adj_test_result;
FS_RESULT dio_short_to_gnd_test_result;
FS_RESULT dio_short_to_vdd_test_result;

#define FLASH_CRC	(0x12345678)
__attribute__((section(".flshcrc"))) const uint32_t flash_crc_value = FLASH_CRC;

fs_aio_test_a1_t aio_safety_test_item_VH =
{
		.AdcChannel = ADC0CH_3V3_MEAS,
		.Limits.low = (uint32_t)SAFETY_ADC_MIN_LIM(SAFETY_ADC_MAX),
		.Limits.high = (uint32_t)SAFETY_ADC_MAX_LIM(SAFETY_ADC_MAX),
		.state = FS_AIO_INIT
};


fs_dio_test_t dio_safety_test_item_0 =
{
		.gpio = GPIO0_BASE,
		.pcr = PORT0_BASE,
		.pinNum = 1,
		.pinDir = kGPIO_DigitalInput,
};

fs_dio_test_t dio_safety_test_item_1 =
{
		.gpio = GPIO1_BASE,
		.pcr = PORT1_BASE,
		.pinNum = 2,
		.pinDir = kGPIO_DigitalOutput,
};

fs_dio_test_t *dio_safety_test_items[] = { &dio_safety_test_item_0, &dio_safety_test_item_1, 0 };

void FsADCTest(void)
{
	classB_ADC_Val = FS_AIO_LimitCheck(aio_safety_test_item_VH.RawResult,
			&(aio_safety_test_item_VH.Limits), &(aio_safety_test_item_VH.state));

	switch (classB_ADC_Val)
	{
	case FS_FAIL_AIO:
        FsSafeState();
	break;

	case FS_AIO_INIT:
	FS_AIO_InputSet_A1(&aio_safety_test_item_VH, (fs_aio_a1_t*)SAFETY_ADC);
	break;

	case FS_AIO_PROGRESS:
	FS_AIO_ReadResult_A1(&aio_safety_test_item_VH, (fs_aio_a1_t*)SAFETY_ADC);
	break;

	case FS_PASS: // successful execution of test, call the trigger function again
		aio_safety_test_item_VH.state = FS_AIO_INIT;
	break;
	default:
	__asm("NOP");
	break;
	}
}

void FsDIOTest(void)
{
	dio_short_to_adj_test_result = FS_DIO_ShortToAdjSet_MCX(&dio_safety_test_item_0, &dio_safety_test_item_1,
			LOGICAL_ONE, BACKUP_ENABLE);
	dio_short_to_adj_test_result = FS_DIO_InputExt_MCX(&dio_safety_test_item_0, &dio_safety_test_item_1,
			LOGICAL_ONE, BACKUP_ENABLE);

	dio_short_to_gnd_test_result = FS_DIO_ShortToSupplySet_MCX(&dio_safety_test_item_0,
			DIO_SHORT_TO_GND_TEST, BACKUP_ENABLE);
	dio_short_to_gnd_test_result = FS_DIO_InputExt_MCX(&dio_safety_test_item_0, &dio_safety_test_item_0,
			DIO_SHORT_TO_GND_TEST, BACKUP_ENABLE);
	dio_short_to_vdd_test_result = FS_DIO_ShortToSupplySet_MCX(&dio_safety_test_item_0,
			DIO_SHORT_TO_VDD_TEST, BACKUP_ENABLE);
	dio_short_to_vdd_test_result = FS_DIO_InputExt_MCX(&dio_safety_test_item_0, &dio_safety_test_item_0,
			DIO_SHORT_TO_VDD_TEST, BACKUP_ENABLE);

}
*/


