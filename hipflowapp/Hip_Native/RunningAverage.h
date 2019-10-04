/*************************************************************************************************
 *
 * Workfile: RunningAverage.h 
 * 05Apr18 - paul
 *
 *************************************************************************************************
* The content of this file is the 
 *     Proprietary and Confidential property of the HART Communication Foundation
 * Copyright (c) 2018, FieldComm Group, Inc., All Rights Reserved 
 *************************************************************************************************
 *
 * Description: Templated running average 
 *		
 *	
 * #include "RunningAverage.h"
 */
#pragma once

#ifndef _RUNNINGAVERAGE_H
#define _RUNNINGAVERAGE_H
#ifdef INC_DEBUG
#pragma message("In RunningAverage.h") 
#endif

#include "CircularBuffer.h"

#ifdef INC_DEBUG
#pragma message("    Finished Includes::RunningAverage.h") 
#endif


/** * *
* Note that class C must be able to hold pRndBuff->maxSize() * maxInputValue or rollover
* ie  running_average<uint16_t> MyAverage(10); 
*     must be restricted to input values from 0 to 6553
*     32 bits with a 16 deep average can take 28 bit values
* * * ***/

template <class C, size_t initialSize>
class running_average 
{
	std::mutex mutex_;
	circular_buffer<C> * pRndBuff;
	C runningTotal = 0;

public:
	running_average() 
	{
		pRndBuff = new circular_buffer<C>(initialSize);
		C local = 0L; //force it to be a numeric or have an operator=...optimized out in Release
	};
	virtual ~running_average() {	std::lock_guard<std::mutex> lock(mutex_); 
									delete pRndBuff;   pRndBuff = NULL;           };

#define DOTHEMATH  ((double)runningTotal/(double)(pRndBuff->size()))
	// get the value
	operator double()  {	return getAvg();      };
	double getAvg(void){ 	std::lock_guard<std::mutex> lock(mutex_);
							return DOTHEMATH;      };

	// add one to get a new average
	double add2Avg( C& newVal )// returns new average
	{
		std::lock_guard<std::mutex> lock(mutex_);
		if (pRndBuff->is_full())
		{
			C local = pRndBuff->get(); // get the oldest, removed from buffer(no longer full)
			runningTotal -= local;
		}
		// else just add it
		pRndBuff->put( newVal );
		runningTotal += newVal;

		return DOTHEMATH;// can't use getAvg because it uses the same mutex
	};

	void resize( size_t newSize ) //resize the length of the running average
	{
		std::lock_guard<std::mutex> lock(mutex_);
		C local;

		if (newSize == 0 || newSize == pRndBuff->maxsize() )
		{// do nothing
			return;
		}
		// else
		// make a new buffer(pRndBuff)..that will copy contents over
		circular_buffer<C> *pNewBuff = new circular_buffer<C>(newSize);

		// add up all the new contents to runningTotal
		size_t cnt = min(newSize, pRndBuff->size() );
		runningTotal = 0;// start a new average
		for ( size_t i = 0; i < cnt; i++ )
		{
			local = pRndBuff->get();
			runningTotal += local;
			pNewBuff->put(local);
		}//next

		delete pRndBuff;
		pRndBuff = pNewBuff;
		return;
	};
};


#endif // _RUNNINGAVERAGE_H