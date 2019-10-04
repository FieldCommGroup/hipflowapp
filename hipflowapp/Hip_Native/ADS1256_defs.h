/*************************************************************************************************
 *
 * Workfile: ADS1256_defs.h 
 * 20Mar18 - paul
 *
 *************************************************************************************************
 * The content of this file was moved from ads1256_test.cpp to be shared
 *************************************************************************************************
 *
 * Description:
 *		3/20/18	pdu	created 
 *	
 * #include "ADS1256_defs.h"
 */
#pragma once

#ifndef _1256_DEFS_H
#define _1256_DEFS_H
#ifdef INC_DEBUG
#pragma message("In ADS1256_defs.h") 
#endif

#include "bcm2835.h"

#ifdef INC_DEBUG
#pragma message("    Finished Includes::ADS1256_defs.h") 
#endif


/* * * * defines in ads1256_test.cpp * * * */
#define DRDY    RPI_GPIO_P1_11  //P0
#define  RST    RPI_GPIO_P1_12  //P1
#define	SPICS	RPI_GPIO_P1_15	//P3


/* Sampling speed choice*/
/* 
	11110000 = 30,000SPS (default)
	11100000 = 15,000SPS
	11010000 = 7,500SPS
	11000000 = 3,750SPS
	10110000 = 2,000SPS
	10100001 = 1,000SPS
	10010010 = 500SPS
	10000010 = 100SPS
	01110010 = 60SPS
	01100011 = 50SPS
	01010011 = 30SPS
	01000011 = 25SPS
	00110011 = 15SPS
	00100011 = 10SPS
	00010011 = 5SPS
	00000011 = 2.5SPS
*/
typedef enum
{
	ADS1256_30000SPS = 0,
	ADS1256_15000SPS,
	ADS1256_7500SPS,
	ADS1256_3750SPS,
	ADS1256_2000SPS,
	ADS1256_1000SPS,
	ADS1256_500SPS,
	ADS1256_100SPS,
	ADS1256_60SPS,
	ADS1256_50SPS,
	ADS1256_30SPS,
	ADS1256_25SPS,
	ADS1256_15SPS,
	ADS1256_10SPS,
	ADS1256_5SPS,
	ADS1256_2d5SPS,

	ADS1256_DRATE_MAX
}ADS1256_DRATE_E;


/* gain channelî */
typedef enum
{
	ADS1256_GAIN_1			= (0),	/* GAIN   1 */
	ADS1256_GAIN_2			= (1),	/*GAIN   2 */
	ADS1256_GAIN_4			= (2),	/*GAIN   4 */
	ADS1256_GAIN_8			= (3),	/*GAIN   8 */
	ADS1256_GAIN_16			= (4),	/* GAIN  16 */
	ADS1256_GAIN_32			= (5),	/*GAIN    32 */
	ADS1256_GAIN_64			= (6),	/*GAIN    64 */
}ADS1256_GAIN_E;

extern void   delay( unsigned mSdelay );
extern void   bsp_DelayUS(uint64_t micros);
extern void    ADS1256_StartScan(uint8_t _ucScanMode);
extern uint8_t ADS1256_ReadChipID(void);
extern void    ADS1256_CfgADC(ADS1256_GAIN_E _gain, ADS1256_DRATE_E _drate);
extern uint8_t ADS1256_Scan(void);
extern int32_t ADS1256_GetAdc(uint8_t _ch);

#endif // _1256_DEFS_H