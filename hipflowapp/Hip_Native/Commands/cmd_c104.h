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
 * #include "cmd_c104.h"
 */

#ifndef CMD_104_H_
#define CMD_104_H_

#include "command.h"
#include "cDatabase.h"
#include "nativeapp.h" // this needs intimate knowledge of commands

#include "cmd_01.h" 
#include "cmd_02.h" 
#include "cmd_03.h" 
#include "cmd_09.h" 
#include "cmd_48.h"


class cmd_104 : public cmd_base
{
	uint8_t burstMsgNumber;
public: // c.dtor
	cmd_104():cmd_base(104) {	};
	~cmd_104(){};

public: // work	
	virtual uint8_t extractData(uint8_t &ByteCnt, uint8_t *pData);
	virtual uint8_t insert_Data(uint8_t &ByteCnt, uint8_t *pData);

};

// extract ByteCnt bytes from data at pData
// return zero on success, response code on error
uint8_t cmd_104::extractData(uint8_t &ByteCnt, uint8_t *pData)
{
	uint8_t ret = 0, chngCnt = 0;
	bool isContinue = false;
	bool valChanged = false;// in continuous mode we are required to force values
	if (ByteCnt < 8)
	{
		ByteCnt = 0;
		return RC_TOO_FEW;// too few data bytes
	}
	burstMsgNumber = *(pData++); ByteCnt--;
	if (burstMsgNumber >= MAX_BURSTMSGS)// it's unsigned so zero to max
	{
		return RC_MULTIPLE_9;//Invalid Burst Message
	}
	burstMessage * pBM = &(burstMsgArray[burstMsgNumber]);// final destination
	tempBurstMsg = burstMsgArray[burstMsgNumber];// preset so non-changed won't change
	uint8_t FF= burstMsgNumber;// don't know why this was set 0xff;
	tempBurstMsg.message_number.setValue(FF);

	do // once
	{
		if (ret = tempBurstMsg.TrigLvlMode.extractSelf (&pData, &ByteCnt, chngCnt)) break;
		if (ret = tempBurstMsg.trigLvlClass.extractSelf(&pData, &ByteCnt, chngCnt)) break;
		if (ret = tempBurstMsg.trigLvlUnits.extractSelf(&pData, &ByteCnt, chngCnt)) break;
		if (ret = tempBurstMsg.trigLvlValue.extractSelf(&pData, &ByteCnt, chngCnt)) break;
	} while (false);// execute once, allow breaks
	/*
	cmd 1 : trigger PV
	cmd 2 : class must be zero, Units %(x37)
	cmd 3 : trigger PV
	cmd 9 : trigger DV in slot 0
	// not implemented  cmd 33: trigger DV in slot 0
	*/
	uint8_t mod = ItemValue(tempBurstMsg.TrigLvlMode,  uint8_t),
	        cls = ItemValue(tempBurstMsg.trigLvlClass, uint8_t),
	        unt = ItemValue(tempBurstMsg.trigLvlUnits, uint8_t);
	float   val = ItemValue(tempBurstMsg.trigLvlValue, float);

	if (mod == 0)
	{
		isContinue = true;
	}
	// get the trigger Device Variable
	uint16_t cmdNum = ItemValue(pBM->command_number, uint16_t);
	//SUPPORTED[]{ 1,2,3,9,48 };

	cmd_base * pCmd = pGlobalApp->cmd(cmdNum); 
	if (pCmd == NULL)
	{
		printf(" Error; BurstMsg %d's command # %d could not be found.\n", burstMsgNumber, cmdNum);
	}
	deviceVar *pDV = NULL;

	if (pCmd != NULL)
	{
//printf("    DEBUG: cmdNum:%d\n",pCmd->number());
		switch (cmdNum)
		{
		case 1:// PV
			{
				pDV = &(devVar_PV);
//printf("    DEBUG: Cmd#1 DevVar#: %d\n", pDV->devVarNumber());
			}
			break;
		case 2://percent range...loop current is first but % is spec'd	
			{
				pDV = deviceVar::devVarPtr(SLOT_244_PERCENT_RANGE_IDX);// percent range devvar
//printf("    DEBUG: Cmd#2 DevVar#: %d\n", pDV->devVarNumber());
			}
			break;
		case 3://first in list - PV
			{
				cmd_03* pCmdThree = (cmd_03*)pCmd;
				pDV = &(devVar_PV);
//printf("    DEBUG: Cmd#3 DevVar#: %d\n", pDV->devVarNumber());
			}
			break;
		case 9://first in list
			{
				pDV = deviceVar::devVarPtr(ItemValue(pBM->indexList[0], uint8_t));
				if ( pDV == NULL && !isContinue)
					return RC_MULTIPLE_12;// invalid units code
//printf("    DEBUG: Cmd#9 DevVar#: %d\n", pDV->devVarNumber());
			}
			break;
		case 38:
		case 48:
			{// only mode 0 or 4
				if (mod != 0 && mod != 4)
				{
//printf("    DEBUG: Cmd#x8 mod = %d..exit 13\n", mod);
					return RC_MULTIPLE_13;// Invalid burst mode (for this vcommand)
				}
//printf("    DEBUG: Cmd#x8 DevVar#: not.\n");
			}
			break;
		default:
			{
				printf(" Burst command %d is not supported.\n", pCmd->number());
				pDV = NULL;
				return RC_INVALID; // command not supported
			}
			break;
		}//endswitch
	}
	if (pDV == NULL)// proly 38 or 48 or unknown UNIT CHANGE!!!!!
	{
//printf("    DEBUG:  DevVar Ptr is NULL.\n");
		if (unt != HART_UNITCODE_NOT_USED && !isContinue)
		{
			printf("Null Device Variable Ptr: expected UNUSED got %d  [ bm# %d DevVar# %d ].\n", unt, burstMsgNumber, ItemValue(burstMsgArray[burstMsgNumber].indexList[0], uint8_t));
			return RC_MULTIPLE_12;// invalid unit
		}
	}

	if ( mod != 0 && mod != 4 ) // then test for range when mode not continuopus or on change
	{
		if (pDV != NULL)
		{
//printf("    DEBUG:  DevVar Ptr is good with mode = %d & unit = 0x%02x.\n",mod, unt);
			if (!pDV->UnitSet.isaUnit(unt))
			{
	printf("Unit mismatch: looking for %d  [ bm# %d DevVar# %d ].\n", unt, burstMsgNumber, ItemValue(burstMsgArray[burstMsgNumber].indexList[0], uint8_t));
//printf("    DEBUG:  Out of the following list:\n");
//for (UnitList::iterator iT = pDV->UnitSet.begin(); iT != pDV->UnitSet.end(); ++iT)
//{
//printf("    DEBUG:        Val: %d  (%s)\n", iT->first, (iT->second).Name().c_str());
//}
				return RC_MULTIPLE_12;// invalid trigger unit
			}
			val = pDV->UnitSet[unt].to_Standard(val);
			// val is now in standard units 
			tempBurstMsg.trigLvlValue.setValue(val);// so we store it later in standard units


			/*	0	Continuous. The Burst Message is published continuously at (worst case) the Minimum Update Period.
				1	Window. The Burst Message is triggered when the source value deviates more than the specified trigger value.
				2	Rising. The Burst Message is triggered when source value Rises Above the specified trigger value.
				3	Falling. The Burst Message is triggered when the source value Falls Below the specified trigger value.
				4	On-Change. The burst message is triggered when any value in the message changes.

				WAP 19Aug2019 - The device can either burst at transition then each min time after<<<won't pass test!
				OR check every min time and see if it's outside the range.<<< will pass test
			*/

			float hi,lo;
			float USL, LSL;//val
			USL = ItemValue(pDV->upperSensorLimit, float);// in standard units
			LSL = ItemValue(pDV->lowerSensorLimit, float);// in standard units

			if (mod == 1)// windowed
			{
				float lastCommdValue = ItemValue(pDV->Value, float);
				hi = lastCommdValue + val;
				lo = lastCommdValue - val;
//printf("    DEBUG:  mode Windowed(1) hi: %8.2f lo: %8.2f\n", hi, lo);
			}
			else
			if (mod == 2)
			{
				hi = val;
				lo = LSL;
//printf("    DEBUG:  mode Rising(2) hi: %8.2f lo: %8.2f\n", hi, lo);
			}
			else
			if (mod == 3)
			{
				hi = USL;
				lo = val;
//printf("    DEBUG:  mode Falling(3) hi: %8.2f lo: %8.2f\n", hi, lo);
			}
			else
			{
				return RC_MULTIPLE_13; //Invalid trigger mode
			}


			if (pDV->devVarNumber() == 0 || pDV->devVarNumber() == 1 )// PV & SV
			{
				if (mod != 1 && val > USL)
				{
//printf("    DEBUG:  current trigger Value: %8.2f,  is > USL: %8.2f \n", val, USL);
						return RC_TOO_LRG;
				}
				if ( hi > USL || lo > USL)// windowed
				{
//printf("    DEBUG:  trigger Value: %8.2f, hi or lo is > USL: %8.2f \n", val,USL);
					return RC_TOO_LRG;
				}
				if (mod != 1 && val < LSL)
				{
//printf("    DEBUG:  current trigger Value: %8.2f,  is < LSL: %8.2f \n", val,LSL);
					return RC_TOO_SML;
				}
				if ( hi < LSL || lo < LSL)// windowed
				{
//printf("    DEBUG:  trigger Value: %8.2f, hi or lo is < LSL: %8.2f \n", val,LSL);
					return RC_TOO_SML;
				}
			}
			tempBurstMsg.risingTrigVal = hi;
			tempBurstMsg.fallingTrigVal= lo;
		}
	}
	
//---------------------------------------------------------------------
	if (cls != HART_NOT_CLASFD)// we are unclassified
	{
		if (! isContinue)
		{
			return RC_MULTIPLE_11;// invalid classification
		}// else we'll correct it below
	}

	if (isContinue)// continuous
	{// by spec
		//...we don't care...	ret = RC_WARN_8;// warn something changed
		valChanged = tempBurstMsg.resetTriggerValues();
	}
	if (ItemValue(tempBurstMsg.command_number, uint16_t) == 2)
	{// by spec
	printf("Cmd 2 reset section\n");
		uint8_t percent = 0x39;
		if ( (ItemValue(tempBurstMsg.trigLvlClass,uint8_t) != constant_ui8) ||
		     (ItemValue(tempBurstMsg.trigLvlUnits,uint8_t) != percent )      )
			valChanged = true;
		tempBurstMsg.trigLvlClass.setValue(constant_ui8);
		tempBurstMsg.trigLvlUnits.setValue(percent);
	}
	// if we get here the contens are valid
	burstMsgArray[burstMsgNumber] = tempBurstMsg;
			 
	if (ret)
	{
		printf( "Data extraction error in cmd %d. ret = %d.\n", number(), ret);
	}
	if (!ret && valChanged)
	{
		ret = RC_WARN_8;// warn something changed
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
uint8_t cmd_104::insert_Data(uint8_t &ByteCnt, uint8_t *pData) 
{
	int ret = 0;
	ByteCnt = 0; // just in case
	burstMessage * pBM = &(burstMsgArray[burstMsgNumber]);
	 *pData++ = burstMsgNumber; ByteCnt++;
	do // once
	{
		if (ret = pBM->TrigLvlMode. insertSelf(&pData, &ByteCnt)) break;
		if (ret = pBM->trigLvlClass.insertSelf(&pData, &ByteCnt)) break;
		if (ret = pBM->trigLvlUnits.insertSelf(&pData, &ByteCnt)) break;
		if (ret = pBM->trigLvlValue.insertSelf(&pData, &ByteCnt)) break;
	}
	while(false);// execute once

	if ( ret )
	{
		printf( "Data insertion error in cmd %d. ret = %d.\n", number(), ret);
	}

    return ret;
};

#endif // CMD_104_H_