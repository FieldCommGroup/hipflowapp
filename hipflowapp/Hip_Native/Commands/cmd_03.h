/*************************************************************************************************
 * Copyright 2019 FieldComm Group, Inc.
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
 * #include "cmd_03.h"
 */

#ifndef CMD_03_H_
#define CMD_03_H_

#include "command.h"


class cmd_03 : public cmd_base
{

public: // c.dtor
	cmd_03():cmd_base(03) {	};
	~cmd_03(){};

public: // work	
	virtual uint8_t extractData(uint8_t &ByteCnt, uint8_t *pData);
	virtual uint8_t insert_Data(uint8_t &ByteCnt, uint8_t *pData);
	virtual bool isTriggered(burstMessage & bMsg);// we're a burstable command

	//virtual uint8_t burstThisCmd(dataItem indexList[], AppPdu *pPDU);

};

// extract ByteCnt bytes from data at pData
// return zero on success, response code on error
uint8_t cmd_03::extractData(uint8_t &ByteCnt, uint8_t *pData)
{
	uint8_t ret = 0;

	// cmd 3 has no sent variables

	return ret;
}


// generate reply
// add bytes from data to pData, filling ByteCnt with the number added (caller adds RC/DS)
// return zero on success, response code (with byte count 0) on error
uint8_t cmd_03::insert_Data(uint8_t &ByteCnt, uint8_t *pData) 
{
	int ret = 0;
	ByteCnt = 0; // just in case
	do // once
	{
	// loop current  followed by PV Units, PV value, SV Units, SV Value
		if ( ret = loopCurrent.    insertSelf( &pData, &ByteCnt )  ) break;
		if ( ret = devVar_PV.Units.insertSelf( &pData, &ByteCnt )  ) break;
		if ( ret = convertAndInsert(devVar_PV, &pData, &ByteCnt )  ) break;
		if ( ret = devVar_SV.Units.insertSelf( &pData, &ByteCnt )  ) break;
		if ( ret = convertAndInsert(devVar_SV, &pData, &ByteCnt )  ) break;
		if ( ret = devVar_TV.Units.insertSelf( &pData, &ByteCnt )  ) break;
		if ( ret = convertAndInsert(devVar_TV, &pData, &ByteCnt )  ) break;
	}
	while(false);// execute once

	if ( ret )
	{
		printf( "Data insertion error in cmd %d. ret = %d.\n", number(), ret);
	}

    return ret;
};

// fill the reply data, address has been filled
// use the default base class version
//void cmd_03::burstThisCmd(dataItem indexList[], AppPdu *pPDU)
//{
//	uint8_t dbgReply = insert_Data(uint8_t &ByteCnt, uint8_t *pData)
//}

bool cmd_03::isTriggered(burstMessage & bMsg)// this is a burstable command
{
	bool R = false;
	float x = ItemValue(loopCurrent, float);// dataitem, type
	deviceVar* pDV = deviceVar::devVarPtr(245);// loopcurrent
	int trigMode = ItemValue(bMsg.TrigLvlMode, int8_t);

	// convert it from standard units to desired units
	int U = ItemValue(bMsg.trigLvlUnits, uint8_t);
	float currentValue = pDV->UnitSet[U].frmStandard(x);// //245 - loop current

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
}

#endif // CMD_03_H_