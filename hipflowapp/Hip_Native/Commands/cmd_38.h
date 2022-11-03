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
 * #include "cmd_38.h"
 */

#ifndef CMD_38_H_
#define CMD_38_H_

#include "command.h"


class cmd_38 : public cmd_base
{
	bool isVer5Host;

public: // c.dtor
	cmd_38():cmd_base(38) {	};
	~cmd_38(){};

public: // work	
	virtual uint8_t extractData(uint8_t &ByteCnt, uint8_t *pData);
	virtual uint8_t insert_Data(uint8_t &ByteCnt, uint8_t *pData);
	virtual bool isTriggered(burstMessage & bMsg);// we're a burstable command

};

// extract ByteCnt bytes from data at pData
// return zero on success, response code on error
uint8_t cmd_38::extractData(uint8_t &ByteCnt, uint8_t *pData)
{
	uint8_t ret = 0, chngCnt = 0;
	bool OK2Clear = false;

	// insert extraction here
	if ( ByteCnt == 0 ) // version 5 master
	{
		OK2Clear  = true;
		isVer5Host= true;
	}
	else
	{
		isVer5Host = false;
		ret = tempconfigChangeCnt.extractSelf( &pData, &ByteCnt, chngCnt);
		if ( ret == 0 &&   *((uint16_t*)(tempconfigChangeCnt.pRaw)) == *((uint16_t*)(configChangeCnt.pRaw))  )
		{
			OK2Clear = true;
		}
		else
		if ( ret == 0)
		{
			OK2Clear = false;
			ret = RC_CFGMISMATCH;
		}
		// else too few data bytes error
	}
	if (OK2Clear)
	{
		if (  fromPrimary || isVer5Host)
		{
			configChangedBit_Pri = false;// clear primary flag
		}
		// has to be secondary
		if ( (!fromPrimary) || isVer5Host)
		{
			configChangedBit_Sec = false; // clear secondary flag
#ifdef _DEBUG
fprintf(stderr, "        SecCCB clr %s.\n",(isVer5Host)?"Version5":"NotV5"); fflush(stderr);
#endif
		}
	}// else do nothing, leave everything as found

	return ret;
}


// generate reply
// add bytes from data to pData, filling ByteCnt with the number added (caller adds RC/DS)
// return zero on success, response code (with byte count 0) on error
uint8_t cmd_38::insert_Data(uint8_t &ByteCnt, uint8_t *pData) 
{
	int ret = 0;
	ByteCnt = 0; // just in case
	// latest spec says return the config counter regardlwss of host type
	ret = configChangeCnt.insertSelf( &pData, &ByteCnt ) ;

	if ( ret )
	{
		printf( "Data insertion error in cmd %d. ret = %d.\n", number(), ret);
	}

    return ret;
};


bool cmd_38::isTriggered(burstMessage & bMsg)// this is a burstable command
{
	bool R = false;
	float x = ItemValue(devVar_PV.Value, float);// dataitem, type
	int trigMode = ItemValue(bMsg.TrigLvlMode, int8_t);

	float currentValue = *((uint16_t*)(configChangeCnt.pRaw));

	switch (trigMode)
	{
	case 0: // continuous
	{// should never come this way
		R = true;
	}
	break;
	case 1: // 1=Window,
	{
		if (currentValue > bMsg.risingTrigVal || currentValue < bMsg.fallingTrigVal)
		{
			R = true;
		}// else leave it false
	}
	break;
	case 2: // 2=Rising,
	{
		if (currentValue > bMsg.risingTrigVal)
		{
			R = true;
		}// else leave it false
	}
	break;
	case 3: // 3=Falling,
	{
		if (currentValue < bMsg.fallingTrigVal)
		{
			R = true;
		}// else leave it false
	}
	break;
	case 4: // 4=AnyChangeInMsgVars
	{// not implemented
		R = false;
	}
	break;
	default:
	{// error
		R = false;
	}
	break;
	}//endswitch
	return R;
};
#endif // CMD_38_H_