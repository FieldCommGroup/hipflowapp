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
 * #include "cmd_109.h"
 */

#ifndef CMD_109_H_
#define CMD_109_H_

#include "command.h"


class cmd_109 : public cmd_base
{
	uint8_t  burstMsgNumber;
	bool     isLegacy;

public: // c.dtor
	cmd_109():cmd_base(109) {	};
	~cmd_109(){};

public: // work	
	virtual uint8_t extractData(uint8_t &ByteCnt, uint8_t *pData);
	virtual uint8_t insert_Data(uint8_t &ByteCnt, uint8_t *pData);

};

// extract ByteCnt bytes from data at pData
// return zero on success, response code on error
uint8_t cmd_109::extractData(uint8_t &ByteCnt, uint8_t *pData)
{
	uint8_t ret = 0, bmcc;
	if (ByteCnt == 1)
	{
		isLegacy = true;
		burstMsgNumber = 0;
		bmcc = *pData++; ByteCnt--;
		if (bmcc > 1)// legacy only has 0 and 1 // off and on
		{
			ret = RC_TOO_FEW; // Too few data bytes			
		}
	}
	else
	if (ByteCnt >= 2)
	{
		isLegacy = false;
		bmcc = *pData++; ByteCnt--;
		burstMsgNumber = *pData++; ByteCnt--;
	}
	else // bytecnt = 0
	{
		ret = RC_TOO_FEW; // Too few data bytes
	}

	if (ret)
	{
		printf( "Data extraction error in cmd %d. ret = %d.\n", number(), ret);
	}
	else
	{
		if (burstMsgNumber >= MAX_BURSTMSGS)// it's unsigned
		{
			return RC_INVALID;//Invalid Burst Message
		}
		if (bmcc > 4)// unsigned so >=0 & < 5 is OK
		{
			return RC_INVALID; // invalid burst mode
		}
		// else install it

		burstMessage * pBM = &(burstMsgArray[burstMsgNumber]);
		bool ischanged = false;
		if (ItemValue(pBM->burstModeCtrl, uint8_t) != bmcc)
			ischanged = true;
			
		pBM->burstModeCtrl.setValue(bmcc);

		if (ischanged)
		{
			bumpConfigCnt();
			writeNeeded = true; // actual write done in NativeApp::handle_device_message
			pBM->updateBurstOperation();
		}
	}

	return ret;
}


// generate reply
// add bytes from data to pData, filling ByteCnt with the number added (caller adds RC/DS)
// return zero on success, response code (with byte count 0) on error
uint8_t cmd_109::insert_Data(uint8_t &ByteCnt, uint8_t *pData) 
{
	int ret = 0;
	ByteCnt = 0; // just in case
	burstMessage * pBM = &(burstMsgArray[burstMsgNumber]);
	ret = pBM->burstModeCtrl.insertSelf(&pData, &ByteCnt);
	if (!ret && !isLegacy)
	{
		ret = insert(burstMsgNumber, &pData, ByteCnt);
	}

	if (ret)
	{
		printf( "Data insertion error in cmd %d. ret = %d.\n", number(), ret);
	}

    return ret;
};

#endif // CMD_109_H_