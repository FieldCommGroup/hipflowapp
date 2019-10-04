/*************************************************************************************************
 *
 * Workfile: physical.cpp
 * 28Mar18 - paul
 *
 *************************************************************************************************
 * The content of this file is the 
 *     Proprietary and Confidential property of the HART Communication Foundation
 * Copyright (c) 2018, Fieldcomm Group Inc., All Rights Reserved 
 *************************************************************************************************
 *
 * Description:
 *		This handles the physical interface for the device.
 *		This also handles the siginit issues for cleanup.
 *
 */

 
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>

#include <semaphore.h>

#include <signal.h>
//#include <cstdio.h>
#include <pthread.h>
#include <pwd.h>

#include "bcm2835.h"
#include "memory_glbl.h" /* includes "shared.h" */
#include "RunningAverage.h"
#include "PV.h"

extern void* run_io(void * pArgs); // phy thread in IOthread.cpp
       void* run_data(void * pArgs);// forward declaration
//extern char updatePrimaryVariable(double rawValue);// in PV.Cpp
//extern char updateSecondaryVariable(double rawValue);// in PV.Cpp
extern char initPressure(void);// in PV.Cpp

extern unsigned debuggingBurstCnt;
extern float pressure,current;

 /*  the only two functions shared outside
 instead of a .h file, copy and paste these

 extern int init_phy(void);
 extern int kill_phy(void);

 ***/
 /*  definitions */
#define chCnt       3 /* added pressure sensor 21may2018 */
#define MAXWAIT   100 /* times 6 mS - 6/10 Sec*/
#define DEFLTSZ    2 /* at 30mS per sample,10 is  ~1/3 sec */

#ifdef _PRESSUREDEVICE
  #define PV_SLOT  2
  #define SV_SLOT 0
#else
    #ifdef _FLOWDEVICE
      #define PV_SLOT  0
      #define SV_SLOT  1
	#else
      #define PV_SLOT  0
      #define SV_SLOT  1
	#endif
#endif
 
volatile int32_t adc[chCnt];   // holds raw ADC data, shared across thread boundries
volatile int32_t updateNo = 0; // incremented on update
#ifdef _DEBUG
volatile int32_t ScanCount = 0;
#endif
int r = 0; // we need to return a value from a thread...&r

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
running_average<uint32_t, DEFLTSZ> RunningAvg[chCnt];// we only have two channels

sharedArgs_s phyArgs; // these have to stick around as long as the sub thread is running


int init_phy(void)
{
	int  r = 0;
#ifdef _DEBUG
	volatile int32_t *pHoldUpdate = &updateNo;
#endif
	// stevev 21mar19 initPressure function now in initialize 
	//if ( initPressure() || (r = sem_init(&dataSemaphore,    0, 1)) == -1)
	if (  (r = sem_init(&dataSemaphore,    0, 1)) == -1 )
	{	
		printf("Error, physical system failed at startup.\n");
	}
	else
	{
		phyArgs.padc       = adc;			// passed on down
		phyArgs.upCnt      = &updateNo;		//volatile here, passed on down
		phyArgs.pSemaPhore = &dataSemaphore;// ony one semiphore..passed down
		phyArgs.pDieCmd    = &dieDataThDie;	// replaced on the way down
#ifdef _DEBUG
		phyArgs.pScanCount = &ScanCount;	// passed on down
#endif
		r = pthread_create(&Datathread, NULL, &run_data, &phyArgs);
		if ( r )// is an error
		{
			printf("Failed to create Data thread.\n");
			// it has killed the IOthread and itself, just exit
			phyArgs.pSemaPhore = NULL;
			sem_destroy(&dataSemaphore);
		}
		else
		{
			printf("Started phy thread 'run_data'\n");
		}
	}
	return r;
}


int kill_phy(void)
{
printf("Kill the physical interface.\n");
	dieDataThDie = true; // it will set back to false right before it exits
	if ( Datathread ) // is zero if not started
	{
		while (dieDataThDie) usleep( 10 );//  for DataThread to kill IOthread and finish
	}
	sem_destroy(&dataSemaphore);
printf("The physical interface is dead.\n");
	return 0;
}

double rt(void)
{
	double val;
	struct timespec rt;
	clock_gettime(CLOCK_REALTIME, &rt);
	val = rt.tv_sec + (rt.tv_nsec / 1000000000.0);
	return val;
}


// for initial calibration
//#include <float.h>
//double minValue[2] = {DBL_MAX, DBL_MAX};
//double maxValue[2] = { 0.0, 0.0 };

extern double avgBurst;
/* runs the IO, smoothing & data pump */
void* run_data(void * pArgs)
{
	r = 0;
	sharedArgs_s *pargs = (sharedArgs_s*)pArgs;
	volatile bool * pMyDie    = pargs->pDieCmd;

	// set up the shared data to be passed in
	myArgs.padc       = pargs->padc;			// global here
	myArgs.upCnt      = pargs->upCnt;		//volatile here
	myArgs.pSemaPhore = pargs->pSemaPhore;
#ifdef _DEBUG
	myArgs.pScanCount = pargs->pScanCount;
#endif

	myArgs.pDieCmd    = &dieThreadDie; // global

	r = pthread_create(&IOthread, NULL, &run_io, &myArgs);
	if ( r )// is an error
	{
		printf("Failed to create IOthread.\n");
		pthread_exit(&r);// kills this thread
		return NULL;     // should not get here
	}
	// else start the work



	int i, waitCnt;
	double volt[8];
	int32_t iTemp;
	int32_t lastUpdt, updtCnt = 0, lasttime = 0, thistime;
	double  lstTime=0.0, thsTime=0.0;
	//u_int8_t buf[3];

#ifdef xxxxDoNotUse_DEBUG
	uid_t  myuid = getuid(), euid = geteuid();
	struct passwd *pPasswd = getpwuid(myuid);
	if (myuid<0 || myuid!=euid) 
	{
    /* We might have elevated privileges beyond that of the user who invoked
     * the program, due to suid bit. Be very careful about trusting any data! */
		if ( ((int)pPasswd) > 100 )
		{
		printf("Debugging as %s. uid (%d); euid(%d) \r\n",pPasswd->pw_name,myuid,euid);
		}
		else
		{
		printf("getpwuid failed as %d. uid (%d); euid(%d) \r\n",(int)pPasswd,myuid,euid);
		}
	} 
	else 
	{		
		if ( ((int)pPasswd) > 100 )
		{
		printf("Debugging as %s.\r\n",pPasswd->pw_name);
		}
		else
		{
		printf("getpwuid failed.%d \r\n",(int)pPasswd);
		}
	}
#endif

#ifdef _DEBUG
	volatile int32_t *pDataUpdate = pargs->upCnt;
#endif	
	lastUpdt = *(pargs->upCnt);

	while( ! (* pMyDie) ) 
	{
		waitCnt = 0;
		while (*(pargs->upCnt) == lastUpdt)
		{
			if ( waitCnt <= MAXWAIT )
			{
				delay( 1 );//mS
				waitCnt++;
			}
			else // repeat the same data
			{
				break; // out of while
			}
		}// wend on update
		lastUpdt = *(pargs->upCnt);
		thistime = time1_32mS;// capture
#ifdef _DEBUG
		thsTime = rt();
		//printf(" Update %12d   %12d   %12d (%d waits %f avg Sec) [%f] w/%d bursts\n",
		//*(pargs->upCnt), updtCnt, *(myArgs.pScanCount), waitCnt,(avgBurst), (thsTime-lstTime),debuggingBurstCnt );
		lstTime = thsTime;
#endif
		lasttime = thistime;
		for (i = 0; i < chCnt; i++)
		{				
				sem_wait(pargs->pSemaPhore);
			uint32_t t = (uint32_t)(adc[i]);
			RunningAvg[i].add2Avg(t);
			volt[i] = ((double)RunningAvg[i] * 100) / 167;
				sem_post(pargs->pSemaPhore);

			//printf(" Raw value 0x%06x      ",adc[i]);
			//if ( updtCnt > 50 )// let the min average out a bit
			//{
			//	minValue[i] = min(minValue[i],(double)RunningAvg[i]);
			//	maxValue[i] = max(maxValue[i],(double)RunningAvg[i]);
			//}
			if (i == PV_SLOT ) //( ! i )// [0]
			{
				updatePrimaryVariable((double)RunningAvg[i]);
				#ifdef turnOff_DEBUG
				if ( updtCnt % 10 )
					printf("  Chan%d: 0x%06x  %12.7f\n", PV_SLOT, t, (double)(RunningAvg[i]));
				//	printf(" Flow: %f     ",pressure);
				#endif
			}
			
			if ( i == SV_SLOT )
			{
				updateSecondaryVariable((double)RunningAvg[1]);
			#ifdef xxx_DEBUG
				printf("  Chan1: 0x%06x  %12.7f\n", t, (double)RunningAvg[i]);
			//	printf(" Current: %f     ",current);
			#endif
			}
			//buf[0] = ((u_int32_t)adc[i] >> 16) & 0xFF;
			//buf[1] = ((u_int32_t)adc[i] >> 8) & 0xFF;
			//buf[2] = ((u_int32_t)adc[i] >> 0) & 0xFF;
/*			printf("%d=%02X%02X%02X, %8ld", (int)i, (int)buf[0], 
					(int)buf[1], (int)buf[2], (long)adc[i]);     */           

			iTemp = (uint32_t)volt[i];	/* uV  */
			if (iTemp < 0)
			{
				iTemp = -iTemp;
//				printf(" (-%ld.%03ld %03ld V) \r\n", iTemp /1000000, (iTemp%1000000)/1000, iTemp%1000);
			}
			else
			{
//				printf(" ( %ld.%03ld %03ld V) \r\n", iTemp /1000000, (iTemp%1000000)/1000, iTemp%1000);                    
			}					
		} // next channel
//		printf("\33[%dA", (int)chCnt+1);  
		updtCnt++;
//		printf("\n");
	//	printf("Avg: %f    Avg:  %f\n",(double)RunningAvg[0], (double)RunningAvg[1]);
	}// next repaint...forever

	if (*pMyDie && IOthread )// non error exit and sub thread is alive
	{
printf("Data trying to kill the IO.\n");
		// kill our sub thread
		dieThreadDie = true;
		while (dieThreadDie) usleep( 10 );//  for IOthread to finish
printf("Data killed the IO.\n");
	}
	else
	{
printf("Data exiting without IO death.\n");
	}
	(*pMyDie) = false;

	return NULL;
}