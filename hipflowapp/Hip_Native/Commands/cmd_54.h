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
 * Description: This holds one command
 *
 * #include "cmd_54.h"
 */

#ifndef CMD_54_H_
#define CMD_54_H_

#include "command.h"


class cmd_54 : public cmd_base
{
	uint8_t activeDevVar;

public: // c.dtor
	cmd_54():cmd_base(54) {	};
	~cmd_54(){};

public: // work	
	virtual uint8_t extractData(uint8_t &ByteCnt, uint8_t *pData);
	virtual uint8_t insert_Data(uint8_t &ByteCnt, uint8_t *pData);

};

// extract ByteCnt bytes from data at pData
// return zero on success, response code on error
uint8_t cmd_54::extractData(uint8_t &ByteCnt, uint8_t *pData)
{
	if (ByteCnt < 1)
	{
		activeDevVar = 0xff;
		return RC_TOO_FEW;// too few data bytes
	}
	activeDevVar = *pData;
	if (deviceVar::devVarPtr(activeDevVar) == NULL)
	{
		return RC_INVALID;// invalid selection..RC_BAD_INDEX 17 invalid device var index is NOT in the command's description.
	}
	// ret is zero
	return RC_SUCCESS;
}


// generate reply
// add bytes from data to pData, filling ByteCnt with the number added (caller adds RC/DS)
// return zero on success, response code (with byte count 0) on error
uint8_t cmd_54::insert_Data(uint8_t &ByteCnt, uint8_t *pData) 
{
	int ret = 0;
	ByteCnt = 0; // just in case
	deviceVar * pDV = deviceVar::devVarPtr(activeDevVar);
	do // once
	{
		*pData++ = activeDevVar; ByteCnt++;// activeDevVar
		if (ret = pDV->sensorSerialNumber.insertSelf(&pData, &ByteCnt)) break;// serial number
		if (ret = pDV->Units.insertSelf(&pData, &ByteCnt)) break;//limits units code
		if (ret = convertAndInsert(pDV->upperSensorLimit, devVar_PV, &pData, &ByteCnt)) break;//upper sensor limit
		if (ret = convertAndInsert(pDV->lowerSensorLimit, devVar_PV, &pData, &ByteCnt)) break;//lower sensor limit
		if (ret = pDV->dampingValue.insertSelf(&pData, &ByteCnt)) break;//damping value
		if (ret = convertAndInsert(pDV->minSpan, devVar_PV, &pData, &ByteCnt)) break;// min span
		if (ret = pDV->classification.insertSelf(&pData, &ByteCnt)) break;// classification
		if (ret = pDV->deviceFamily.insertSelf(&pData, &ByteCnt)) break;//dv family
		if (ret = pDV->updatePeriod.insertSelf(&pData, &ByteCnt)) break;//acq period time:4 bytes
		if (ret = pDV->properties.insertSelf(&pData, &ByteCnt)) break;//bits-table 65
	}
	while(false);// execute once

	if ( ret )
	{
		printf( "Data insertion error in cmd %d. ret = %d.\n", number(), ret);
	}

    return ret;
};

#endif // CMD_54_H_