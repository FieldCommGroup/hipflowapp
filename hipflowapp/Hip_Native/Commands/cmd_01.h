/*************************************************************************************************
 * Copyright 2019-2021 FieldComm Group, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
	 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Description: This holds one command
 *
 * #include "cmd_01.h"
 */

#ifndef CMD_01_H_
#define CMD_01_H_

#include "command.h"


class cmd_01 : public cmd_base
{

public: // c.dtor
	cmd_01():cmd_base(01) {	};
	~cmd_01(){};

public: // work	
	virtual uint8_t extractData(uint8_t &ByteCnt, uint8_t *pData);
	virtual uint8_t insert_Data(uint8_t &ByteCnt, uint8_t *pData); 
	virtual bool isTriggered(burstMessage & bMsg);// we're a burstable command

};

// extract ByteCnt bytes from data at pData
// return zero on success, response code on error
uint8_t cmd_01::extractData(uint8_t &ByteCnt, uint8_t *pData)
{
	uint8_t ret = 0;

	// insert extraction here

	return ret;
}


// generate reply
// add bytes from data to pData, filling ByteCnt with the number added (caller adds RC/DS)
// return zero on success, response code (with byte count 0) on error
uint8_t cmd_01::insert_Data(uint8_t &ByteCnt, uint8_t *pData) 
{
	int ret = 0;
	ByteCnt = 0; // just in case
	do // once
	{
	// units followed by float value
		if ( ret = devVar_PV.Units.insertSelf( &pData, &ByteCnt )  ) break;
		if ( ret = convertAndInsert(devVar_PV, &pData, &ByteCnt )  ) break;
	}
	while(false);// execute once

	if ( ret )
	{
		printf( "Data insertion error in cmd %d. ret = %d.\n", number(), ret);
	}

    return ret;
};


bool cmd_01::isTriggered(burstMessage & bMsg)// this is a burstable command
{
	bool R = false;
	float x = ItemValue(devVar_PV.Value, float);// dataitem, type
	int trigMode = ItemValue(bMsg.TrigLvlMode, int8_t);

	// convert it from standard units to desired units
	int U = ItemValue(bMsg.trigLvlUnits, uint8_t);
	float currentValue = devVar_PV.UnitSet[U].frmStandard(x);

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
			if ( currentValue < bMsg.fallingTrigVal)
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
}

#endif // CMD_00_H_