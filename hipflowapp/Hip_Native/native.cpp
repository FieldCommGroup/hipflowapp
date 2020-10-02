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
 *****************************************************************/

#include "native.h"
#include "debug.h" // holds dbg logs
#include "errval.h"
#include "sems.h"
#include "cDataRaw.h"
#include "hartPhy.h" 
#include <string.h>
#include "configuration_Default.h"

#include "hartdefs.h"
#include "appconnector.h"
#include "apppdu.h"
#include "PhysicalAlarms.h"


extern int init_ProcessVariables(void);/*  instead of all PV.h  */

extern void initBurstStack(); // in burst.cpp
extern errVal_t FillDefaultValues(); // in FillDefaultValues.cpp
/*
 * There should only be one TP Master Stack object per executable
 */
NativeData::NativeData():myAddr( {0,0,0,0,0} )
{
// nothing else required	
}

NativeData::~NativeData()
{
	// empty
}

errVal_t NativeData::commandline(int argc, char *argv[])  // -p <port>
{
	errVal_t retval = NO_ERROR;
	// native has no command line variables at this time
	return retval;
}
extern void testTime();
// read a file and/or set static data items
/* stevev --- set variables from file...or default values..burst coomand included */
errVal_t NativeData::configure()
{
	//testTime();
	struct stat sb;
	errVal_t retval = NO_ERROR;

	int ret = Open_File((char*)CONFIG_FILENAME, sizeof(CONFIG_FILENAME));

	if ( CheckFile((char*)CONFIG_FILENAME) && ! ret)// it exists, has data, and is closed
	{
		retval = getData();
		handleDataDependencies();
	}
	else
	if( CheckFile((char *)CONFIG_FILENAME) && ret > 0 )// it exists, is empty and is closed
	{
		retval = fileFillDefault();// generate and save default values to new file
		handleDataDependencies();
	}
	else
	{
		retval = FATAL_ERROR;// utter and complete failure
	}

	return retval;
}

void NativeData::handleDataDependencies()
{
	GenerateMyAddress();
	initBurstStack();
}

void NativeData::GenerateMyAddress()
{
	myAddr[0] = ItemValue(mfrID, uint8_t) & 0x3F; // top two bits set elsewhere
	myAddr[1] = ItemValue(devType, uint8_t);
	myAddr[2] = ((uint8_t *)devID.pRaw)[0];
	myAddr[3] = ((uint8_t *)devID.pRaw)[1];
	myAddr[4] = ((uint8_t *)devID.pRaw)[2];
}

// programatically set data, semaphores, spin threads, etc
errVal_t NativeData::initialize()
{
	errVal_t ret = NO_ERROR;
	int r = 0;
	//no-op
	return ret;
}


// stop threads, delete semaphores and allocated memory
//static 
errVal_t NativeData::cleanup()
{
	errVal_t retval = NO_ERROR;
	int32_t errVal;
	
	dbgp_sem("Total %d semaphores exist\n", get_sem_count());
	if (get_sem_count())
	{
		sem_post(p_semStopMain);
	}

	delete_semaphores();

	return retval;
}

void NativeData::setUniqueID(uint8_t *pLowMACaddr) //we will use first 3 bytes here
{
	NONvolatileData.devID[0] = pLowMACaddr[0];
	NONvolatileData.devID[1] = pLowMACaddr[1];
	NONvolatileData.devID[2] = pLowMACaddr[2];
}

// fill all non volatile with default values and save 'em to the open file
// close the file on exit
errVal_t NativeData::fileFillDefault()
{
	errVal_t retval = FillDefaultValues();

	putData();// write to disk

	return retval;
}