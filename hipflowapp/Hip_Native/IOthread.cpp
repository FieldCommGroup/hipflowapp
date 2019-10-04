/*************************************************************************************************
 *
 * Workfile: IOthread.cpp
 * 20Mar18 - paul
 *
 *************************************************************************************************
 * The content of this file is the 
 *     Proprietary and Confidential property of the HART Communication Foundation
 * Copyright (c) 2018, Fieldcomm Group Inc., All Rights Reserved 
 *************************************************************************************************
 *
 * Description:
 *		This is the Pi thread that is doing the actual IO.
 *		Reads data from the AD to shared memory
 *	    Checks a kill boolean, sleeps, does it again.
 */
 
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

#include "ADS1256_defs.h"
#include "shared.h"

#define MINUSNULL   ((void*)-1)



void* run_io(void * pArgs)
{
    uint8_t id;
  	// now passed in int32_t adc[8];
	// not used here int32_t volt[8];
	uint8_t i;
	uint8_t ch_num;
	int32_t iTemp;
	// not used here uint8_t buf[3];
	sharedArgs_s *pShared = (sharedArgs_s *) pArgs;

int pass=0;
int y;

#ifdef _DEBUG
	volatile int32_t *pIoUpdate = pShared->upCnt;
	extern volatile uint32_t time1_32mS;
	uint32_t lastTm = 0, thisTm;
#endif	

    if (!bcm2835_init())
	{
        goto runExit;
	}
    bcm2835_spi_begin();
    bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_LSBFIRST );      // The default
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE1);                   // The default
    bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_1024); // The default
    bcm2835_gpio_fsel(SPICS, BCM2835_GPIO_FSEL_OUTP);//
    bcm2835_gpio_write(SPICS, HIGH);
    bcm2835_gpio_fsel(DRDY, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_set_pud(DRDY, BCM2835_GPIO_PUD_UP);    	
    //ADS1256_WriteReg(REG_MUX,0x01);
    //ADS1256_WriteReg(REG_ADCON,0x20);
   // ADS1256_CfgADC(ADS1256_GAIN_1, ADS1256_15SPS);
   id = ADS1256_ReadChipID();
   printf("\r\n");
   printf("ID=   ");  
	if (id != 3)
	{
		printf("Error, ASD1256 Chip ID = 0x%d\r\n", (int)id);	
		while(! *(pShared->pDieCmd) )// for proper shutdown (a little dirty...I know.)
		{
			timespec x; x.tv_sec = 1; x.tv_nsec =0;
			nanosleep(&x, NULL);
		}
        goto runExit;
	}
	else
	{
		printf("Ok, ASD1256 Chip ID = 0x%d\n", (int)id);
	}
#ifdef _PRESSUREDEVICE
  	ADS1256_CfgADC(ADS1256_GAIN_4, ADS1256_15SPS);
#else
  #ifdef _FLOWDEVICE
  	ADS1256_CfgADC(ADS1256_GAIN_1, ADS1256_15SPS);
  #else
  	ADS1256_CfgADC(ADS1256_GAIN_1, ADS1256_15SPS);
  #endif
#endif

    ADS1256_StartScan(0);
	
	*(pShared->upCnt) = 0;
	ch_num = 3;	// we are only interested in the first two

	while(! *(pShared->pDieCmd) )
	{
#ifdef _DEBUG
	thisTm = time1_32mS;
#endif
	    while((ADS1256_Scan() == 0));

		for (i = 0; i < ch_num; i++)
		{
			iTemp  =  ADS1256_GetAdc(i);

				sem_wait(pShared->pSemaPhore);
			(pShared->padc)[i] = iTemp;
				sem_post(pShared->pSemaPhore);
		}
			sem_wait(pShared->pSemaPhore);
		*(pShared->upCnt) += 1;
			sem_post(pShared->pSemaPhore);
		//bsp_DelayUS(100000);	// uS -- 100 mS -- 0.1 Sec...10 samples/sec
		// bsp_DelayUS(20000);	// uS -- 20 mS -- 0.02 Sec...50 samples/sec
		// it takes ~ 28 mS to run
		// bsp_DelayUS(5000);	// uS -- 5 mS + 28mS = 33 mS -- 0.033 Sec...30 samples/sec
		// removing this delay made no change to the run time.
#ifdef _DEBUG
		*(pShared->pScanCount)+=1 ;// = thisTm-lastTm;		lastTm = thisTm;
		struct tm t1, t2;
		if (*(pShared->pScanCount) == 10)
		{
			time_t t = time(NULL);
			t1 = *localtime(&t);
		}
		if (*(pShared->pScanCount) == 110)
		{
			time_t t = time(NULL);
			t2 = *localtime(&t);
			*(pShared->upCnt) -= 1;
		}
#endif
	}	
runExit:
	if ( bcm2835_peripherals != ((uint32_t *)mapFailed) )
	{// onlu un-map 'em if we mapped them to begin with
		bcm2835_spi_end();
		bcm2835_close();
	}
	
	if (*(pShared->pDieCmd))// non error exit
	{
printf("IO exiting with Death command.\n");
		*(pShared->pDieCmd) = false; // handshake with owner to say we be done
		return MINUSNULL;
	}

	return NULL;
}