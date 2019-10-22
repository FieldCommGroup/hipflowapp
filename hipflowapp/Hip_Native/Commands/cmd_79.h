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
 * #include "cmd_79.h"
 */

#ifndef CMD_79_H_
#define CMD_79_H_

#include "command.h"


class cmd_79 : public cmd_base
{
friend class cmd_534;

protected:
	uint8_t activeDevVar;
	uint8_t wdvcc;

public: // c.dtor
	cmd_79():cmd_base(79) {	};
	~cmd_79(){};

public: // work	
	virtual uint8_t extractData(uint8_t &ByteCnt, uint8_t *pData);
	virtual uint8_t insert_Data(uint8_t &ByteCnt, uint8_t *pData);

};

// extract ByteCnt bytes from data at pData
// return zero on success, response code on error
uint8_t cmd_79::extractData(uint8_t &ByteCnt, uint8_t *pData)
{
	uint8_t ret = 0, chngCnt = 0;
	deviceVar *pDV = NULL;

	if (ByteCnt < 8)
	{
		activeDevVar = 0xff;
		ret = RC_TOO_FEW;// too few data bytes
	}
	else
	{ 
		activeDevVar = *pData++;ByteCnt--;
		if (deviceVar::devVarPtr(activeDevVar) == NULL)
		{
			ret = RC_BAD_INDEX;// RC_BAD_INDEX 17 invalid device var index is NOT in the command's description.
		}
		else
		{
			pDV = deviceVar::devVarPtr(activeDevVar);
			do // once
			{
				if (ret = pDV->lastDVcmdCode.extractSelf(&pData, &ByteCnt, chngCnt)) break;
				if (ret = pDV->simulateUnits.extractSelf(&pData, &ByteCnt, chngCnt)) break;
				if (ret = pDV->simulateValue.extractSelf(&pData, &ByteCnt, chngCnt)) break;
				if (ret = pDV->simulateStats.extractSelf(&pData, &ByteCnt, chngCnt)) break;
			} while (false);// execute once, allow breaks
			wdvcc = ItemValue(pDV->lastDVcmdCode, uint8_t );
			if (wdvcc != 0 && wdvcc != 1)
			{
				ret = RC_MULTIPLE_10; // Invalid Write Device Variable Command Code
			}
		}
	}

	if (ret)
	{
		printf( "Data extraction error in cmd %d. ret = %d.\n", number(), ret);
	}
	else // there are no warnings in this command
	{
		pDV->Set_Simulation( );
	}

	if (!isError(ret) && chngCnt != 0)// something changed in this extraction
	{
		bumpConfigCnt();
	}
	return ret;
}


// generate reply
// add bytes from data to pData, filling ByteCnt with the number added (caller adds RC/DS)
// return zero on success, response code (with byte count 0) on error
uint8_t cmd_79::insert_Data(uint8_t &ByteCnt, uint8_t *pData) 
{
	int ret = 0;
	ByteCnt = 0; // just in case
	deviceVar *pDV = deviceVar::devVarPtr(activeDevVar);
	do // once
	{
		*pData++ = activeDevVar; ByteCnt++;// activeDevVar
		if (ret = pDV->lastDVcmdCode.insertSelf(&pData, &ByteCnt)) break;
		if (ret = pDV->simulateUnits.insertSelf(&pData, &ByteCnt)) break;
		if (ret = pDV->simulateValue.insertSelf(&pData, &ByteCnt)) break;
		// we have no device family, these are required to be zero
		*pData++ = 0; ByteCnt++;
	} while (false);// execute once

	if (ret)
	{
		printf( "Data insertion error in cmd %d. ret = %d.\n", number(), ret);
	}
    return ret;
};

#endif // CMD_79_H_