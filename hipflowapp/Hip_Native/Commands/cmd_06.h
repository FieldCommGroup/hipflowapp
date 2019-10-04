/*************************************************************************************************
 *
 * Workfile: cmd_06.h
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
 * #include "cmd_06.h"
 */

#ifndef CMD_06_H_
#define CMD_06_H_

#include "command.h"



class cmd_06 : public cmd_base
{

public: // c.dtor
	cmd_06():cmd_base(06) {	};
	~cmd_06(){};

public: // work	
	virtual uint8_t extractData(uint8_t &ByteCnt, uint8_t *pData);
	virtual uint8_t insert_Data(uint8_t &ByteCnt, uint8_t *pData);

};

// extract ByteCnt bytes from data at pData
// return zero on success, response code on error
uint8_t cmd_06::extractData(uint8_t &ByteCnt, uint8_t *pData)
{
	uint8_t ret = 0, chngCnt = 0;
	do // once
	{
	//  if there is only poll address, DO NOT SET too-few-bytes-recieved....it's a V5 host
		if ( ret = pollAddr       .extractSelf( &pData, &ByteCnt, chngCnt)  ) break;
		if (ByteCnt != 0 && ret == RC_SUCCESS )// ie it's not a version 5 host
		     ret = loopCurrentMode.extractSelf( &pData, &ByteCnt, chngCnt);  //table 16
	}
	while(false);// execute once

	if (!isError(ret) && chngCnt != 0)// some config item changed in this extraction
	{
		bumpConfigCnt();
	}
	return ret;
}


// generate reply
// add bytes from data to pData, filling ByteCnt with the number added (caller adds RC/DS)
// return zero on success, response code (with byte count 0) on error
uint8_t cmd_06::insert_Data(uint8_t &ByteCnt, uint8_t *pData) 
{
	int ret = 0;
	ByteCnt = 0; // just in case
	do // once
	{
		if ( ret = pollAddr       .insertSelf( &pData, &ByteCnt )  ) break;
		     ret = loopCurrentMode.insertSelf( &pData, &ByteCnt );  //table 16
	}
	while(false);// execute once

	if ( ret )
	{
		printf( "Data insertion error in cmd %d. ret = %d.\n", number(), ret);
	}

    return ret;
};

#endif // CMD_06_H_