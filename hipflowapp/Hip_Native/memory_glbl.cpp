/*************************************************************************************************
 *
 * Workfile: memory_glbl.cpp
 * 28Mar18 - paul
 *
 *************************************************************************************************
 * The content of this file is the 
 *     Proprietary and Confidential property of the HART Communication Foundation
 * Copyright (c) 2018, Fieldcomm Group Inc., All Rights Reserved 
 *************************************************************************************************
 *
 * Description:
 *		This handles memory that needs to be accesses by many functions/threads.
 *
 */

 #include "memory_glbl.h"
 
 /* * * global SHARED memory * * * */
sem_t            dataSemaphore; // protects data in adc (see physical)
volatile bool    dieThreadDie = false; // cmd true to kill the phy thread, thread sets it false at exit
volatile bool    dieDataThDie = false; // cmd true to kill Datathread, thread sets it false at exit
volatile bool    dieBursthDie = false; // cmd true to kill the burst thread, sets it false at exit
sharedArgs_s     myArgs; // must stay around as long as the thread is running
volatile uint32_t time1_32mS = 0;

bool             queuesRopen = false;
bool             isBIGendian = false;// starts out little-endian (Intel)

/* * * the threads involved (other than main()  * * * */
pthread_t IOthread   = 0;
pthread_t Datathread = 0;
pthread_t Burstthread= 0;


#ifdef _DEBUG
double rt(void);
unsigned debuggingBurstCnt;
double avgBurst;
double lastT, thisT;
#endif
