/*************************************************************************************************
 *
 * Workfile: cmd_2693.h
 * 03May2019 - stevev
 *
 *************************************************************************************************
 * The content of this file is the
 *     Proprietary and Confidential property of the FieldComm Group
 * Copyright (c) 2018, FieldComm Group, Inc., All Rights Reserved
 *************************************************************************************************
 *
 * Description: This holds one command
 *
 * #include "cmd_2693.h"
 */

#ifndef CMD_2693_H_
#define CMD_2693_H_

#include "command.h"


class cmd_2693 : public cmd_base
{
	uint8_t activeDevVar;
public: // c.dtor
	cmd_2693():cmd_base(2693) {	};
	~cmd_2693(){};

public: // work	
	virtual uint8_t extractData(uint8_t &ByteCnt, uint8_t *pData);
	virtual uint8_t insert_Data(uint8_t &ByteCnt, uint8_t *pData);

};

// extract ByteCnt bytes from data at pData
// return zero on success, response code on error
uint8_t cmd_2693::extractData(uint8_t &ByteCnt, uint8_t *pData)
{
	uint8_t ret = 0, chngCnt = 0;
	ByteCnt = 0; // just in case
	if (ByteCnt < 1)
	{
		activeDevVar = 0xff;
		ret = RC_TOO_FEW;// too few data bytes
	}
	ret = extract(activeDevVar, &pData, ByteCnt);
	if (ret)// not success
	{
		printf( "Data extraction error in cmd %d. ret = %d.\n", number(), ret);
	}
	else
	if (NULL == deviceVar::devVarPtr(activeDevVar))
	{
		ret = RC_INVALID;
	}
	else
	if ((ret = isDevVarIndex2Totalizer(activeDevVar)) == RC_SUCCESS)
	{
		ret = totRelayInverted.extractSelf(&pData, &ByteCnt, chngCnt);
	}// else...just return ret

	if (!isError(ret) && chngCnt != 0)// something changed in this extraction
	{
		bumpConfigCnt();
	}
	return ret;
}


// generate reply
// add bytes from data to pData, filling ByteCnt with the number added (caller adds RC/DS)
// return zero on success, response code (with byte count 0) on error
uint8_t cmd_2693::insert_Data(uint8_t &ByteCnt, uint8_t *pData) 
{
	int ret = 0;
	ByteCnt = 0; // just in case
	do // once
	{
		if (ret = insert(activeDevVar, &pData, ByteCnt))  break;
		ret = totRelayInverted.insertSelf(&pData, &ByteCnt);
	}
	while(false);// execute once

	if ( ret )
	{
		printf( "Data insertion error in cmd %d. ret = %d.\n", number(), ret);
	}

    return ret;
};

#endif // CMD_2693_H_