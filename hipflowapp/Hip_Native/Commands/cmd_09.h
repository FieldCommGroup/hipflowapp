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
 * #include "cmd_09.h"

 */

#ifndef CMD_09_H_
#define CMD_09_H_

#include "command.h"
#include "PV.h"
#include <stdio.h>

class cmd_09 : public cmd_base
{
	int requestCount;// number of valid slots
	uint8_t slot[8];// we have to handle if all are slot 0

public: // c.dtor
	cmd_09():cmd_base(0x09) {	};
	~cmd_09(){};

public: // work	
	virtual uint8_t extractData(uint8_t &ByteCnt, uint8_t *pData);
	virtual uint8_t insert_Data(uint8_t &ByteCnt, uint8_t *pData);
	virtual bool isTriggered(burstMessage & bMsg);// we're a burstable command
	
	int insertUnsupported( uint8_t slotNum, uint8_t **ppData, uint8_t *pInserCnt );
	int insertSlot       ( uint8_t slotNum, uint8_t **ppData, uint8_t *pInserCnt );

	void setIndexes(dataItem indexList[]);
};

int debughitcount = 0;

// extract ByteCnt bytes from data at pData
// return zero on success, response code on error
uint8_t cmd_09::extractData(uint8_t &ByteCnt, uint8_t *pData)
{
	uint8_t  ret = RC_SUCCESS;
	requestCount = ByteCnt;
	if (requestCount > 8)
		requestCount = 8;
	if (requestCount <= 0 )
		return RC_TOO_FEW;

	// insert extraction here, there is at least 1
	int slotNumber = 0;
	while ( (ByteCnt > 0) && (ret == RC_SUCCESS) )
	{
		ret = extract( slot[slotNumber++], &pData, ByteCnt);
	}
//printf(">>>> Exit 9 Extract.\n");
	return ret;
};

// generate reply
// add bytes from data to pData, filling ByteCnt with the number added (caller adds RC/DS)
// return zero on success, response code (with byte count 0) on error
uint8_t cmd_09::insert_Data(uint8_t &ByteCnt, uint8_t *pData) 
{
	bool istruncated = false;
	int ret = RC_SUCCESS, i;
	if (requestCount > 4)
	{// wap confirmed this was correct 08apr2019
		istruncated = true;
		requestCount = 4;
	}
	ByteCnt = 0; // just in case
	do // once
	{
		if ( (ret = extended_fld_dev_status.insertSelf( &pData, &ByteCnt ))  ) break;
		for ( i = 0; i < requestCount && ret == RC_SUCCESS; i++)// for each requested code
		{
		// latest balloted spec - that isn't published at this time..now has 3 responses:
		// OK - we support it,  "Invalid Selection" response code for 251 and greater, and Not supported
			if (slot[i] > 250)
			{
				return RC_INVALID;
			}
			else
			if (slot[i] == 3)// force 3 to be unsupported here
			{
//fprintf(stderr, ">>>> Cmd 9: Insert %d slot %d as unsupported.\n", i, slot[i]);
				ret = insertUnsupported(slot[i], &pData, &ByteCnt);
			}
			else
			if (deviceVar::isDevVar(slot[i]))//slot[i] > 3 )
			{
//fprintf(stderr, ">>>> Cmd 9: Insert %d slot %d\n", i, slot[i]);
				ret = insertSlot( slot[i], &pData, &ByteCnt);
			}
			else // it's unsigned so it has to be 0 to 3
			{
//fprintf(stderr, ">>>> Cmd 9: Insert %d slot %d as unsupported.\n", i, slot[i]);
				ret = insertUnsupported( slot[i], &pData, &ByteCnt);
			}
		}
		if (ret == RC_SUCCESS)
		{
//printf(">>>> Insert timestamp and leave\n");
			updateTimeStamp();
			ret = dateTimeStamp.insertSelf( &pData, &ByteCnt );
		}
	}
	while(false);// execute once

	if ( ret ) // has an error
	{
		printf( "Data insertion error in cmd %d. ret = %d slot 0x02X.\n", number(), ret, slot[i]);
	}
	else // insertion went fine
	if (istruncated)
	{
		ret = RC_TRUNCATED;// 30 (Warning) ResponseTruncated  
	}

    return ret;
};



int cmd_09::insertUnsupported( uint8_t slotNum, uint8_t **ppData, uint8_t *pInserCnt )
{
	int ret = RC_SUCCESS;

	do // once
	{
		if( (ret = insert(                 slotNum, ppData, *pInserCnt )) != RC_SUCCESS)  break;
		if( (ret = insert(         HART_NOT_CLASFD, ppData, *pInserCnt )) != RC_SUCCESS)  break;
		if( (ret = insert(  HART_UNITCODE_NOT_USED, ppData, *pInserCnt )) != RC_SUCCESS)  break;
		if( (ret = insert(               NaN_value, ppData, *pInserCnt )) != RC_SUCCESS)  break;
		if( (ret = insert(HART_DEVICESTAT_NOT_USED, ppData, *pInserCnt )) != RC_SUCCESS)  break;	 
	}
	while(false);// execute once
	return ret;
};

int cmd_09::insertSlot( uint8_t slotNum, uint8_t **ppData, uint8_t *pInserCnt )
{// assume slot number has been verified as 0 or 1
	int ret = RC_SUCCESS;

	deviceVar* pDV = deviceVar::devVarPtr(slotNum);
	if (pDV == NULL)
	{
		return RC_INVALID;
	}

	do // once
	{
		double locDbl = RTdata.getDevVar(slotNum);
		// unit conversion if required goes here
		pDV->Value.setValue(locDbl);// bad slot numbers get set to NaN
		if( (ret = insert( slotNum, ppData, *pInserCnt )) != RC_SUCCESS)  break;
		if( (ret = pDV->classification.insertSelf( ppData, pInserCnt )) != RC_SUCCESS)  break;
		if( (ret = pDV->Units         .insertSelf( ppData, pInserCnt )) != RC_SUCCESS)  break;
		if( (ret =          convertAndInsert(*pDV, ppData, pInserCnt )) != RC_SUCCESS)  break;
// don't know how this ever compiled...		if( (ret = devVarArray[slotNum].df_status     .insertSelf( ppData, pInserCnt )) != RC_SUCCESS)  break;		 
		if ((ret = pDV->devVar_status .insertSelf( ppData, pInserCnt )) != RC_SUCCESS)  break;
	}
	while(false);// execute once
//}
	return ret;
};




void cmd_09::setIndexes(dataItem indexList[])
{
	uint8_t idx;
	requestCount = 0;
	
	for (int i = 0; i < MAX_CMD_9_RESP; i++)
	{
		idx = ItemValue(indexList[i],uint8_t);
		if (idx != 0xFA)
		{
			slot[requestCount++] = idx;
		}
		else
			break; // at the first 250 index
	}
};


bool cmd_09::isTriggered(burstMessage & bMsg)// this is a burstable command
{
	bool R = false;
	
	uint8_t slotNumber = ItemValue(bMsg.indexList[0],uint8_t);
	deviceVar* pDV = deviceVar::devVarPtr(slotNumber);//first one is data item

	float x = ItemValue(pDV->Value, float);// dataitem, type
	int trigMode = ItemValue(bMsg.TrigLvlMode, int8_t);

	// convert it from standard units to desired units
	int U = ItemValue(bMsg.trigLvlUnits, uint8_t);
	float currentValue = pDV->UnitSet[U].frmStandard(x);// //245 - loop current

	//printf("        Cmd 9 curr=%6.2f Hi=%6.2f Lo=%6.2f Mode = %d\n", currentValue, bMsg.risingTrigVal, bMsg.fallingTrigVal, trigMode);

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
#endif // CMD_09_H_