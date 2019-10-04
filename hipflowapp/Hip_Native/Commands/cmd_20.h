
/*************************************************************************************************
 *
 * Workfile: cmd_20.h
 * 13Dec18 - stevev
 *
 *************************************************************************************************
 * The content of this file is the
 *     Proprietary and Confidential property of the FieldComm Group
 * Copyright (c) 2018, FieldComm Group, Inc., All Rights Reserved
 *************************************************************************************************
 *
 * Description: This holds one command, readLongTag
 *
 * #include "cmd_20.h"
 */

#ifndef CMD_20_H_
#define CMD_20_H_

#include "command.h"


class cmd_20 : public cmd_base
{
public: // c.dtor
	cmd_20():cmd_base(20) {	
	};
	~cmd_20(){};

public: // work
	virtual uint8_t extractData(uint8_t &ByteCnt, uint8_t *pData);
	virtual uint8_t insert_Data(uint8_t &ByteCnt, uint8_t *pData);

};

// extract ByteCnt bytes from data at pData
// return zero on success, response code on error
uint8_t cmd_20::extractData(uint8_t &ByteCnt, uint8_t *pData)
{
	int ret = 0;

	// there are no reads in this command

	return ret;
}

// read, handle, generate reply
// return response code 
//     return RC with High Bit SET if no response is required
uint8_t cmd_20::insert_Data(uint8_t &ByteCnt, uint8_t *pData)
{
	int ret = 0;
	ByteCnt = 0; // just in case

	ret = longTag.      insertSelf( &pData, &ByteCnt );

	if ( ret )
	{
		printf( "Data insertion error in cmd %d. ret = %d.\n", number(), ret);
	}

    return ret;
};

#endif // CMD_20_H_