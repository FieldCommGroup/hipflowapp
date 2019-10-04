/**********************************************************
 * Copyright (C) 2019, FieldComm Group, Inc.
 **********************************************************/

 /**********************************************************
  *
  * File Name:
  *   timeStamp.cpp
  * File Description:
  *   Functions use rt clock to generate a HART time value
  *
  **********************************************************/

#include <time.h>
#include <string.h>
#include <stdio.h>

#include "cDatabase.h"

/************
 *  Globals
 ************/

/************************************
 *  Private variables for this file
 ************************************/

/**********************************************
 *  Private function prototypes for this file
 **********************************************/


/***********************************************
 *  Functions
 ***********************************************/

void testTime()
{
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);

	printf("now: %d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
}


// "must be monotonic and roll over every 24 hours"
void    updateTimeStamp(void)// date+time to 1/32 mS in a uint32
{
	struct timespec spec;
	struct tm       tm;
	time_t          s;
	unsigned        cnt;

	clock_gettime(CLOCK_REALTIME, &spec);
	s  = spec.tv_sec;
	tm = *localtime(&s);

	cnt = (int) ((spec.tv_nsec/31250.0)+.5);// nS /  nS/cnt rounded then truncated
	cnt += tm.tm_sec * 32000;               // sec * cnt/sec
	cnt += tm.tm_min * 1920000;             // min * cnt/min
	cnt += tm.tm_hour* 115200000;			// hr  * cnt/hr

	dateTimeStamp.setValue(cnt);
}