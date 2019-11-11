/*************************************************************************************************
 * Copyright 2019 FieldComm Group, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Description: This holds the common functions for raw PV, SV data
 *	
 *	
 * #include "PV.h"
*/
#pragma once

#ifndef _PV_H
#define _PV_H
#ifdef INC_DEBUG
#pragma message("In PV.h") 
#endif

//#include "datatypes.h"
//#include "errval.h"
//#include <sys/stat.h>
//#include <stdio.h>
//#include <semaphore.h>
//#include <string>

#include <semaphore.h>

using namespace std;

#ifdef INC_DEBUG
#pragma message("    Finished Includes::PV.h") 
#endif


#pragma once



#define NO_CLASS     0		// 'Not Classified' 
#define UNITS_MA     39		// mA
#define UNITS_PERCNT 57		// %
#define UNITS_PSI     6
#define UNITS_F      0x21
#define UNITS_KGpH   75		// kg/h
#define UNITS_NONE   250		// unused
#define NO_STATUS      0
#define UNUSED_STATUS 0x30
#define DV_STATUS_GOOD   0xc0	/* bit 7 & 6 */
#define DV_STATUS_POOR   0x40	/* bit  6 */

#define MA_ALARM       100.0  /* alarms above this value  */

//   catch( int e ) {}
#define EXCP_NO_SEMAPHORE		100

// note that the rt data is stored as a double, most uses are for float
// Storing as double prevents the constant conversion to do math (all c math is in double)
// converting to float just to put it in a command is a small price to pay
class rtData
{
	double rtFlow;
	double rtCurrent;
	double rtTotalizer;
	double rtPercentRng;
	
	sem_t    RTdataSema;    // control access to RT data for reading/writing 

public:
	rtData();
	~rtData();
public:
	double getFlow(void);
	double getCurrent(void);
	double getTotal(void);
	double getPercent(void);
	double getDevVar(uint8_t devVarIdx);

public: // for rtData update calculations
	char updateFlow(double raw);
	char updateCurrent(double raw);
	char updateTotal(double raw);


private:
// flow::>
	float zero = 17536.0;
	float span = (float)0.00286701665772516;
			float lowerRawValue = 20000.0;
			float upperRawValue = 8388607.0;
	float lowerTrimPt = 10.0;
			float upperTrimPt = 24000.0;
	float lastValue = 4096.0;


	float b_zero = 1000000.0;// new cal 06nov2019...was::>205000.0;
	float b_span = (float)0.0000308424756779135;
			float c_lowerRawValue = 4096.0;
			float c_upperRawValue = 1048576.0 + 4096.0;
	float b_lowerTrimPt = 1.0;
			float c_upperTrimPt = 150.0;
	float c_lastValue = 4096.0;

};
// these should never be needed
//void   setFlow(double);
//void   setCurrent(double);
//void   setTotal(double);
//void   setPercent(double);

//instantiates and initializes at startup, dtor at exit
extern rtData RTdata;

//int init_ProcessVariables(void); in ctor
#define updatePrimaryVariable( r )   RTdata.updateFlow( r )
#define updateSecondaryVariable( r ) RTdata.updateCurrent( r )

#endif //_PV_H