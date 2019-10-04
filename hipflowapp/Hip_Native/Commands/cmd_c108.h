/*************************************************************************************************
 *
 * Workfile: cmd_108.h
 * 11Dec18 - stevev
 *
 *************************************************************************************************
 * The content of this file is the
 *     Proprietary and Confidential property of the FieldComm Group
 * Copyright (c) 2018, FieldComm Group, Inc., All Rights Reserved
 *************************************************************************************************
 *
 * Description: This holds one command
 *
 * #include "cmd_108.h"
 */

#ifndef CMD_108_H_
#define CMD_108_H_

#include "command.h"


class cmd_108 : public cmd_base
{
	uint16_t newCmd;
	uint8_t  burstMsgNumber;
	bool     isLegacy;

public: // c.dtor
	cmd_108():cmd_base(108) {	};
	~cmd_108(){};

public: // work	
	virtual uint8_t extractData(uint8_t &ByteCnt, uint8_t *pData);
	virtual uint8_t insert_Data(uint8_t &ByteCnt, uint8_t *pData);

};




////debug code
//extern uint8_t debugUnitchnge;
//
//void prDiff(int L) {
//	if ((debugUnitchnge != 0) && (debugUnitchnge != NONvolatileData.devVars[0].Units))
//		printf(">>Change %d\n", L);
//}
/////////////////// end debug code



// extract ByteCnt bytes from data at pData
// return zero on success, response code on error
uint8_t cmd_108::extractData(uint8_t &ByteCnt, uint8_t *pData)
{
	uint8_t ret = RC_SUCCESS;

	if (ByteCnt == 1)
	{
		isLegacy = true;
		burstMsgNumber = 0;

		uint8_t nc = *pData++;ByteCnt--;
		newCmd = nc;


	}
	else
	if (ByteCnt >= 3)
	{
		isLegacy = false;
		ret = extract(newCmd, &pData, ByteCnt);

		if (!ret)// success
		{
			ret = extract(burstMsgNumber, &pData, ByteCnt);
		}


	}
	else // byte count is 0 or 2
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
			return RC_MULTIPLE_9;//Invalid Burst Message
		}
		// else install it

		burstMessage * pBM = &(burstMsgArray[burstMsgNumber]);
		//uint16_t SUPPORTED[]{ 1,2,3,9,38,48 };

		if (!pBM->isAburstCmd(newCmd))
		{
			return RC_INVALID;
		}
		pBM->command_number.setValue( newCmd );

		uint8_t pre = ItemValue(pBM->TrigLvlMode, uint8_t);
		//trigger classification
		uint8_t tcl = ItemValue(pBM->trigLvlClass, uint8_t);
		// device variable classification
		uint8_t dvl = 0;//right now all the device variable classifications are 0, not classified

		//If the trigger mode is not "Continuous" and 
		//the trigger source's Device Variable Classification does not match
		// for the new command number

		if ((tcl != dvl) && (pre != 0))
		{
			pBM->resetTriggerValues();
			//The field device shall not return "Burst Condition Conflict" 
			//but it will reconfigure the attributes associated with Command 104.
			if (! isLegacy)
				ret = RC_WARN_8;
			else
				ret = RC_SUCCESS;
		}
	}

	return ret;
}

 
// generate reply
// add bytes from data to pData, filling ByteCnt with the number added (caller adds RC/DS)
// return zero on success, response code (with byte count 0) on error
uint8_t cmd_108::insert_Data(uint8_t &ByteCnt, uint8_t *pData) 
{
	int ret = 0;
	ByteCnt = 0; // just in case
	burstMessage * pBM = &(burstMsgArray[burstMsgNumber]);

	if (isLegacy)
	{
		uint8_t cn = (ItemValue(pBM->command_number, uint16_t) & 0xff);

		ret = insert( cn, &pData, ByteCnt);
		// that's all folks...

	}
	else
	{
		ret = pBM->command_number.insertSelf(&pData,&ByteCnt);

		if (!ret)
		{
			ret = insert(burstMsgNumber, &pData,ByteCnt);

		}
	}

	if ( ret )
	{
		printf( "Data insertion error in cmd %d. ret = %d.\n", number(), ret);
	}


    return ret;
};

#endif // CMD_108_H_