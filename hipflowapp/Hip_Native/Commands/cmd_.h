/*************************************************************************************************
 *
 * Workfile: cmd_00.h
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
 * #include "cmd_00.h"
 */

#ifndef CMD_00_H_
#define CMD_00_H_

#include "command.h"


class cmd_00 : public cmd_base
{

public: // c.dtor
	cmd_00():cmd_base(00) {	};
	~cmd_00(){};

public: // work	
	virtual uint8_t extractData(uint8_t &ByteCnt, uint8_t *pData);
	virtual uint8_t insert_Data(uint8_t &ByteCnt, uint8_t *pData);

};

// extract ByteCnt bytes from data at pData
// return zero on success, response code on error
uint8_t cmd_00::extractData(uint8_t &ByteCnt, uint8_t *pData)
{
	uint8_t ret = 0;

	// insert extraction here

	return ret;
}


// generate reply
// add bytes from data to pData, filling ByteCnt with the number added (caller adds RC/DS)
// return zero on success, response code (with byte count 0) on error
uint8_t cmd_00::insert_Data(uint8_t &ByteCnt, uint8_t *pData) 
{
	int ret = 0;
	ByteCnt = 0; // just in case
	do // once
	{
		if ( ret = expansion.      insertSelf( &pData, &ByteCnt )  ) break;
		// etc
	}
	while(false);// execute once

	if ( ret )
	{
		printf("Data insertion error. ret = %d.\n", ret);
		ret     = 6;//Device Specific Command error
		ByteCnt = 0;
	}

    return ret;
};

#endif // CMD_00_H_