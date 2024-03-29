/*************************************************************************************************
 * Copyright 2019-2021 FieldComm Group, Inc.
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
 * #include "cmd_534.h"
 */

#ifndef CMD_534_H_
#define CMD_534_H_

#include "command.h"


class cmd_534 : public cmd_base
{
	uint8_t activeDevVar;

public: // c.dtor
	cmd_534():cmd_base(534) {	};
	~cmd_534(){};

public: // work	
	virtual uint8_t extractData(uint8_t &ByteCnt, uint8_t *pData);
	virtual uint8_t insert_Data(uint8_t &ByteCnt, uint8_t *pData);

};

// extract ByteCnt bytes from data at pData
// return zero on success, response code on error
uint8_t cmd_534::extractData(uint8_t &ByteCnt, uint8_t *pData)
{
	uint8_t ret = 0;

	if (ByteCnt < 1)
	{
		activeDevVar = 0xff;
		ret = RC_TOO_FEW;// too few data bytes
	}
	else
	{
		activeDevVar = *pData++; ByteCnt--;
		if (deviceVar::devVarPtr(activeDevVar) == NULL)
		{
			ret = RC_INVALID;
		}
	}

	return ret;
}


// generate reply
// add bytes from data to pData, filling ByteCnt with the number added (caller adds RC/DS)
// return zero on success, response code (with byte count 0) on error
uint8_t cmd_534::insert_Data(uint8_t &ByteCnt, uint8_t *pData) 
{
	int ret = 0;
	ByteCnt = 0; // just in case
	deviceVar *pDV = deviceVar::devVarPtr(activeDevVar);
	uint8_t  wdvcc = ItemValue(pDV->lastDVcmdCode, uint8_t);

	do // once
	{
		if (wdvcc & 0x01)// Fixed (ie in simulation)
		{ 
			*pData++ = activeDevVar; ByteCnt++;// activeDevVar
			if (ret = pDV->lastDVcmdCode.insertSelf(&pData, &ByteCnt)) break;
			if (ret = pDV->simulateUnits.insertSelf(&pData, &ByteCnt)) break;
			if (ret = pDV->simulateValue.insertSelf(&pData, &ByteCnt)) break;
		}
		else
		{
			*pData++ = activeDevVar; ByteCnt++;// activeDevVar
			*pData++ = 0; ByteCnt++;// lastDVcmdCode - Normal
			*pData++ = HART_UNITCODE_NOT_USED; ByteCnt++;// Units
			insert(NaN_value, &pData, ByteCnt);
		}
		// we have no device family, these are required to be zero
		*pData++ = 0; ByteCnt++;
	} while (false);// execute once

	if (ret)
	{
		printf( "Data insertion error in cmd %d. ret = %d.\n", number(), ret);
	}
    return ret;
};

#endif // CMD_534_H_