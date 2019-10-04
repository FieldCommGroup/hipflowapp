/*************************************************************************************************
 *
 * Workfile: cmd_13.h
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
 * #include "cmd_13.h"
 */

#ifndef CMD_13_H_
#define CMD_13_H_

#include "command.h"


class cmd_13 : public cmd_base
{

public: // c.dtor
	cmd_13():cmd_base(13) {	};
	~cmd_13(){};

public: // work	
	virtual uint8_t extractData(uint8_t &ByteCnt, uint8_t *pData);
	virtual uint8_t insert_Data(uint8_t &ByteCnt, uint8_t *pData);

};

// extract ByteCnt bytes from data at pData
// return zero on success, response code on error
uint8_t cmd_13::extractData(uint8_t &ByteCnt, uint8_t *pData)
{
	uint8_t ret = 0;

	// noting to extract

	return ret;
}


// generate reply
// add bytes from data to pData, filling ByteCnt with the number added (caller adds RC/DS)
// return zero on success, response code (with byte count 0) on error
uint8_t cmd_13::insert_Data(uint8_t &ByteCnt, uint8_t *pData) 
{
	int ret = 0;
	ByteCnt = 0; // just in case
	do // once
	{
		if ( ret = tag.       insertSelf( &pData, &ByteCnt )  ) break;//  6		
		if ( ret = descriptor.insertSelf( &pData, &ByteCnt )  ) break;//+12
		if ( ret = date.      insertSelf( &pData, &ByteCnt )  ) break;//+ 3
	}
	while(false);// execute once

	if ( ret )
	{
		printf("Data insertion error in cmd %d. ret = %d.\n", number(), ret);
	}

    return ret;
};

#endif // CMD_13_H_