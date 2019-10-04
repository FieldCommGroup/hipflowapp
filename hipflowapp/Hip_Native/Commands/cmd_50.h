/*************************************************************************************************
 *
 * Workfile: cmd_50.h
 * 14Dec18 - stevev
 *
 *************************************************************************************************
 * The content of this file is the
 *     Proprietary and Confidential property of the FieldComm Group
 * Copyright (c) 2018, FieldComm Group, Inc., All Rights Reserved
 *************************************************************************************************
 *
 * Description: This holds one command
 *
 * #include "cmd_50.h"
 */

#ifndef CMD_50_H_
#define CMD_50_H_

#include "command.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *  Command is not used - Mappable PV is not yet implemented
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

class cmd_50 : public cmd_base
{

public: // c.dtor
	cmd_50():cmd_base(50) {	};
	~cmd_50(){};

public: // work	
	virtual uint8_t extractData(uint8_t &ByteCnt, uint8_t *pData);
	virtual uint8_t insert_Data(uint8_t &ByteCnt, uint8_t *pData);

};

// extract ByteCnt bytes from data at pData
// return zero on success, response code on error
uint8_t cmd_50::extractData(uint8_t &ByteCnt, uint8_t *pData)
{
	uint8_t ret = 0;

	// no request bytes

	return ret;
}


// generate reply
// add bytes from data to pData, filling ByteCnt with the number added (caller adds RC/DS)
// return zero on success, response code (with byte count 0) on error
uint8_t cmd_50::insert_Data(uint8_t &ByteCnt, uint8_t *pData) 
{
	int ret = 0;
	ByteCnt = 0; // just in case
	do // once
	{
		if (ret = PVidx.insertSelf(&pData, &ByteCnt)) break;
		if (ret = SVidx.insertSelf(&pData, &ByteCnt)) break;
		if (ret = TVidx.insertSelf(&pData, &ByteCnt)) break;
		*pData++ = 250; ByteCnt++;
	}
	while(false);// execute once

	if ( ret )
	{
		printf("Data insertion error. ret = %d.\n", ret);
		ret     = RC_DEV_SPEC;//Device Specific Command error
		ByteCnt = 0;
	}

    return ret;
};

#endif // CMD_50_H_